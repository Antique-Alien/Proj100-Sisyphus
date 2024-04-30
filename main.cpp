
#include "mbed-os/mbed.h"

#include "motor.h"
#include "tunes.h"
#include "pwm_tone.h"
#include "PROJ100_Encoder.h"
#include "PROJ100_Encoder_Tests.h"
#include <chrono>

#define TIME_PERIOD 10             //Constant compiler Values here 10 equates to 10ms or 100Hz base Frequency
#define ENCODER_PIN_LEFT            D6 // Swapped from D8 to D6
#define ENCODER_PIN_RIGHT           D8 // Swapped from D6 to D8
#define PULSES_PER_ROTATION         60
#define DEBOUNCE_US                 30000
#define circumference               209 // Circumference of wheel in mm
#define width                       113.5 // width of Cart in mm
#define tRPM                        60//Target RPM -  for mass change across bead pushing code

using chrono::seconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;

//DigitalIn microswitch1(D4);         //Instance of the DigitalIn class called 'microswitch1' moved to PROJ100_Encorder_Tests for use in functions
//DigitalIn microswitch2(D3);         //Instance of the DigitalIn class called 'microswitch2' moved to PROJ100_Encorder_Tests for use in functions
DigitalIn myButton(USER_BUTTON);    //Instance of the DigitalIn class called 'myButton'   
DigitalOut greenLED(LED1);          //Instance of the DigitalOut class called 'greenLED'
DigitalOut blueLED(LED2);           //Instance of the DigitalOut class called 'blueLED'
DigitalOut redLED(LED3);            //Instance of the DigitalOut class called 'redLED'

Motor Wheel(D13,A0,D9,D10);      //Instance of the Motor Class called 'Wheel' see motor.h and motor.cpp (The Pin A0 was originally D11, however this caused issues with the buzzer so I have set D0 as an input and then put a wire between A0 and D11)
DigitalIn Pin(D11);              // Buzzer pin to allow threading

PROJ100_Encoder right_encoder (ENCODER_PIN_RIGHT,PULSES_PER_ROTATION);  //Instance of the PROJ100Encoder class called 'right_encoder'
PROJ100_Encoder left_encoder(ENCODER_PIN_LEFT,PULSES_PER_ROTATION);     //Instance of the PROJ100Encoder class called 'left_encoder'

UnbufferedSerial ser(USBTX,USBRX,115200);   // Serial object for printing info


// function names to run bead pushing
void PushLane(); 
void NextLane(float fdist, float bdist);
void backtostart();
void TurnLeft(float angle);

//Thread music_thread; //Create a seperate thread to play music on
int main ()
{
    // Clear the terminal and print an intro
    printf("\033[2J\033[1;1H\r\n");
    printf("Plymouth University PROJ100 2023/24 Optical Encoder Demonstration\n\r");

    // Set the PWM frequency
    Wheel.Period_in_ms(TIME_PERIOD);                

    // Ensure our motors and encoders are stopped at the beginning
    right_encoder.stop();
    left_encoder.stop();
    Wheel.Stop();

    // The encoder pulses can be noisy and so are debounced within the class
    // You can experiment with changing the debouce time if you wish
    left_encoder.setDebounceTimeUs(DEBOUNCE_US);
    right_encoder.setDebounceTimeUs(DEBOUNCE_US);

    //Initialising menu and button state - menu slection on beadpushing for 
    int Menu = 2;
    bool buttonState = false;


    
    // Wait for the blue button to be pressed, with menu selection allowing you cycle through the task list, with led lights for visual indication of task selected
    printf("Press Blue Button To Begin\n\r");
    greenLED = 0;
    blueLED = 1;
    // use of microswitch to cycle through menu
    while (myButton == 0){
        buttonState = checkpress();
        if (buttonState == true ){
            wait_us(100000);
            Menu ++;
            if (Menu == 3)Menu = 0; 
            if(Menu == 0){
                redLED = 1;
                blueLED = 0;
                greenLED = 0;
            }else if(Menu == 1){
                redLED = 0;
                blueLED = 0;
                greenLED = 1;
            }else{
                redLED = 0;
                blueLED = 1;
                greenLED = 0;
            }
        }
    }

    // Start the encoders
    left_encoder.start();
    right_encoder.start();

    //music_thread.start(Rick); // start playing music on the music_thread thread

    // Write the parts of your code which should run in a loop between here..
    //speed_test(); 
    while(true)
    {
        //Straight Line code - Green LED

        if(Menu == 1){
            driveForward(1180, 30, circumference); // drive 1m (1000mm)
            rotateClockwise(180, 20, circumference, width); // changed to 202 for friction adjustment....rotate 180 degrees
            driveForward(1050, 30, circumference); // drive 1m (1000mm)
            rotateClockwise(180, 20, circumference, width); // changed to 202 for friction adjustment....rotate 180 degrees
            Victory();
            wait_us(5000000);
        }
        // Bead Pushing code - Blue LED
        else if(Menu == 2){

            for(int x = 1; x <= 3; x++) // run for 3 loops, as 4.28 lanes covers width of board so 5 lane pushes needed in total
            {
                PushLane();
                NextLane(200, 40); // push 200mm and reverse 40mm for total of 160mm across to have overlap
            }
            //next section does the last pushes and 0.28 (44.8mm) part of the board 
            PushLane(); //4th lane push
            wait_us(200000);
            rotateClockwise(90, tRPM, circumference, width); // rotate 90 degrees
            wait_us(200000);
            driveBackward(80, tRPM, circumference); // to allow room for turn
            wait_us(200000);
            TurnLeft(90); //curved turn to scoop more beads
            wait_us(200000);
            driveForward(200, tRPM, circumference);
            wait_us(200000);
            driveBackward(500, tRPM, circumference);
            wait_us(200000);
            backtostart();
        }
        // Parallel line for 1m - red LED
        else{
            driveForward(1000,40, circumference);
            Rick();
        }
    

    // ..and here
    }
       
}

// Pushes the lane and returns to start position, no varible input as lane length is always the same
void PushLane() 
{
    driveBackward(200, tRPM, circumference); // align with arena wall
    driveForward(380, tRPM, circumference); // drive 307 mm up to arena divider - edited to push over bump
    driveBackward(500, tRPM, circumference); // drive 500 mm back to arena wall, usually stopped by microswitches

}    

// moves over to next lane, with 20mm overlap on previous lane, clears some beads from new lane
void NextLane(float fdist, float bdist)
{       
    wait_us(200000); // Use of waits seems to help keep the turning more accurate
    driveForward(35, tRPM, circumference); // gets off the back wall to allow for turn
    wait_us(200000);
    rotateClockwise(100, tRPM, circumference, width); // rotate 90 degrees 
    wait_us(200000);
    driveForward(fdist, tRPM, circumference); // drive 200mm to clear bead in next lane
    wait_us(200000);
    driveBackward(bdist, tRPM, circumference); // drive 40 mm back to cause overlap after bead clearance
    wait_us(200000);
    rotateCounterClockwise(80, tRPM, circumference, width); // rotate 90 degrees back to push beads
}

void backtostart()
{
    wait_us(200000); // Use of waits seems to help keep the turning more accurate
    driveForward(30, tRPM, circumference); // gets off the back wall to allow for turn
    wait_us(200000);
    rotateCounterClockwise(90, tRPM, circumference, width);
    wait_us(200000);
    driveForward(500, tRPM, circumference); // drive most of length to loop again 
    wait_us(200000);
    rotateClockwise(90, tRPM, circumference, width);
}

void TurnLeft(float angle) //variable to turn on a curve left 
{

    // Determines the how many full and partial rotataions needed for the distance input and then how many pulses to achieve it
    float outwheel = width*2; //outer wheel moves twice as far
    // calculated distance for each wheel of buggy's axis
    float indist = (angle/360)*2*width*3.141; 
    float outdist = (angle/360)*2*outwheel*3.141; 
    // calculate number of rotations needed
    float inRotations = indist/circumference; 
    float outRotations = outdist/circumference;
    //calculates the number of pulses needed for each wheel
    float inpulse = floor(inRotations*PULSES_PER_ROTATION); 
    float outpulse = floor(outRotations*PULSES_PER_ROTATION); 
    int ppr = left_encoder.getPulsesPerRotation();
    float rRPM; //Right RPM
    float lRPM; //Left RPM
    float trRPM = tRPM;
    float tlRPM = float(tRPM)/2;
    int rPulseCount = 0; // Number of pulses on the Right
    int lPulseCount = 0; // Number of pulses on the Left
    
    int loop_delay_ms = 1;          // This sets how often the loop runs


    //Testing Variables
    float LastrRPM = 0;
    float LastlRPM = 0;
        Timer print_timer;
    print_timer.start();




    
    
    bool rolling = true; // Is the cart supposed to be driving
    
    // Light pulse time variables
    int32_t lTime;
    int32_t rTime;

    float pwrIncrement = 0.02f; // Increment for changing power to the wheels
    
    Wheel.Speed(0.8,0.4); //sets inital speeds, where right motor is twice as fast to get the correct turning radius

    while(rolling){

        lTime = left_encoder.getLastPulseTimeUs();
        rTime = right_encoder.getLastPulseTimeUs();
        //Increment the pulse counts if the pulse reader gets a new pulse.
        if(lTime>0){
            lPulseCount++;
            lRPM = (60000000.0f/(ppr*lTime)); // Calculates current RPM of Left wheel
            // Compares left wheel RPM to the left wheel target RPM and adjusts speed
            if(lRPM>tlRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()-pwrIncrement);
            }
            else if(lRPM<tlRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()+pwrIncrement);
            }
        }
        if(rTime>0){
        
            rPulseCount++;
            rRPM = (60000000.0f/(ppr*rTime)); // Calculates current RPM of Right wheel
            // Compares right wheel RPM to the right wheel target RPM and adjusts speed
            if(rRPM>trRPM){
                Wheel.Speed(Wheel.getSpeedRight()-pwrIncrement,Wheel.getSpeedLeft());
            }
            else if(lRPM<trRPM){
                Wheel.Speed(Wheel.getSpeedRight()+pwrIncrement,Wheel.getSpeedLeft());
            }
        }

        
        //If both pulse counters are above or equal to the target, stop driving.
        if(rPulseCount>=outpulse && lPulseCount>=inpulse){
            rolling = false;
        }
        LastlRPM = lRPM; //For testing
        LastrRPM = rRPM; //For testing


        ThisThread::sleep_for(std::chrono::milliseconds(loop_delay_ms));

    }
    //Stop the wheels
    Wheel.Speed(0.0, 0.0);

}



