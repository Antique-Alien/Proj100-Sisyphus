
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

using chrono::seconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;

//DigitalIn microswitch1(D4);         //Instance of the DigitalIn class called 'microswitch1'
//DigitalIn microswitch2(D3);         //Instance of the DigitalIn class called 'microswitch2'
DigitalIn myButton(USER_BUTTON);    //Instance of the DigitalIn class called 'myButton'   
DigitalOut greenLED(LED1);          //Instance of the DigitalOut class called 'greenLED'

Motor Wheel(D13,A0,D9,D10);      //Instance of the Motor Class called 'Wheel' see motor.h and motor.cpp (The Pin A0 was originally D11, however this caused issues with the buzzer so I have set D0 as an input and then put a wire between A0 and D11)
DigitalIn Pin(D11);

PROJ100_Encoder right_encoder (ENCODER_PIN_RIGHT,PULSES_PER_ROTATION);  //Instance of the PROJ100Encoder class called 'right_encoder'
PROJ100_Encoder left_encoder(ENCODER_PIN_LEFT,PULSES_PER_ROTATION);     //Instance of the PROJ100Encoder class called 'left_encoder'

UnbufferedSerial ser(USBTX,USBRX,115200);   // Serial object for printing info

// function names to run bead pushing
void PushLane(); 
void NextLane(float fdist, float bdist);
void backtostart();
void TurnLeft(float angle);

Thread music_thread; //Create a seperate thread to play music on
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

    // Wait for the blue button to be pressed
    printf("Press Blue Button To Begin\n\r");
    while (myButton == 0){greenLED = !greenLED; ThisThread::sleep_for(100ms);}

    // Start the encoders
    left_encoder.start();
    right_encoder.start();

    /*********************ENCODER TESTS***************/
    // These contain while(1) loops so ensure that they are removed or commented out when running your own code
    // If these lines are left in the lines below will never run
    /*************************************************/

    //simple_test();
    //speed_test();

    music_thread.start(Imperial); // start playing music on the music_thread thread

    /*rotateCounterClockwise(90,50,circumference,width);
    while(true){
        while(myButton == 0);
        rotateCounterClockwise(90,50,circumference,width);
        */
        
        // Write the parts of your code which should run in a loop between here..
        while(true)
        {
            //driveForward(1000, 20, circumference); // drive 1m (1000mm)
            rotateClockwise(202, 20, circumference, width); // changed to 202 for friction adjustment....rotate 180 degrees
            //driveForward(1000, 20, circumference); // drive 1m (1000mm)
            wait_us(5000000);

            /*for(int x = 1; x <= 3; x++) // run for 3 loops, as 4.28 lanes covers width of board so 5 lane pushes needed in total
            {
                PushLane();
                NextLane(200, 40); // push 200mm and reverse 40mm for total of 160mm across to have overlap
            }
           //next section does the last pushes and 0.28 (44.8mm) part of the board 
            PushLane(); //4th lane push
            rotateClockwise(90, 20, circumference, width); // rotate 90 degrees
            TurnLeft(90);
            driveForward(200, 20, circumference);
            driveBackward(320, 20, circumference);
            //To add a curved turn to scoop more beads
            //wait_us(100000000);
            //NextLane(40, 0); // drive 40mm to clear last part, 44.8mm left if placed perfectly
            //PushLane(); // final lane push
            // return to start and loop again?
            backtostart();*/
        
        /*//1 meter parallel line test
        while(true)
        {
            driveForward(1000, 50, circumference) // drive 1m (1000mm)
            rotateClockwise(180, 50, circumference, width); // rotate 180 degrees
            driveForward(1000, 50, circumference) // drive 1m (1000mm)
        } */
        // ..and here
        }
/*while(Parallel == true) //menu selection boolean

}*/
       
}

// Pushes the lane and returns to start position, no varible input as lane length is always the same
void PushLane() 
{
    driveForward(307, 20, circumference); // drive 307 mm up to arena divider
    driveBackward(320, 20, circumference); // drive 307 mm back to arena beginning

}    

// moves over to next lane, with 20mm overlap on previous lane, clears some beads from new lane
void NextLane(float fdist, float bdist)
{       
    wait_us(200000); // Use of waits seems to help keep the turning more accurate
    driveForward(30, 20, circumference); // gets off the back wall to allow for turn
    wait_us(200000);
    rotateClockwise(90, 20, circumference, width); // rotate 90 degrees 
    wait_us(200000);
    driveForward(fdist, 20, circumference); // drive 200mm to clear bead in next lane
    wait_us(200000);
    driveBackward(bdist, 20, circumference); // drive 40 mm back to cause overlap after bead clearance
    wait_us(200000);
    rotateCounterClockwise(90, 50, circumference, width); // rotate 90 degrees back to push beads
}

void backtostart()
{
    wait_us(200000); // Use of waits seems to help keep the turning more accurate
    driveForward(30, 20, circumference); // gets off the back wall to allow for turn
    wait_us(200000);
    rotateCounterClockwise(90, 20, circumference, width);
    wait_us(200000);
    driveForward(500, 20, circumference); // drive most of length to loop again 
    wait_us(200000);
    rotateClockwise(90, 20, circumference, width);
}

void TurnLeft(float angle) // to be tested
{
    //Initialzing variables
    int rPulseCount = 0; // Number of pulses on the Right, zeroed to ensure count is accurate
    int lPulseCount = 0; // Number of pulses on the Left, zeroed to ensure count is accurate
    
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
    printf("inpulse = %f\n", inpulse); 
    printf("outpulse = %f\n", outpulse); 
    // Run until number of pulses calcualted in functions are reached
    Wheel.Speed(0.8,0.4); // out wheel needs to move twice as fast to turn the total pulses the same number of times
    while(rPulseCount<=outpulse || lPulseCount<=inpulse)
    {
        //SpeedControl(); Working on it currently
        if (right_encoder.pulseReceived() == 1) // encoders were mounted backwards, changed digital input definition.
        {
            rPulseCount++;
            
        }
        if (left_encoder.pulseReceived() == 1)
        {
            lPulseCount++;
            
        }
        //For each of these if statements: If the wheel is faster than the target rpm, slow it down, and vice versa
        /*    if(lRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()-pwrIncrement);
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()+pwrIncrement);
            }
            if(rRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight()-pwrIncrement,Wheel.getSpeedLeft());
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight()+pwrIncrement,Wheel.getSpeedLeft());
            }
        printf("lPulsecount = %d\n", lPulseCount);
        printf("rPulsecount = %d\n", rPulseCount);*/
    }
    //Stop the wheels
    Wheel.Speed(0.0, 0.0);
}



