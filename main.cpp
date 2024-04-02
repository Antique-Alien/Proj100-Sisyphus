
#include "mbed-os/mbed.h"

#include "motor.h"
#include "tunes.h"
#include "pwm_tone.h"
#include "PROJ100_Encoder.h"
#include "PROJ100_Encoder_Tests.h"
#include <chrono>

#define TIME_PERIOD 10             //Constant compiler Values here 10 equates to 10ms or 100Hz base Frequency
#define ENCODER_PIN_LEFT            D8
#define ENCODER_PIN_RIGHT           D6 
#define PULSES_PER_ROTATION         60
#define DEBOUNCE_US                 30000
#define circumference               209 // Circumference of wheel in mm
#define width                       113.5 // width of Cart in mm

using chrono::seconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;

DigitalIn microswitch1(D4);         //Instance of the DigitalIn class called 'microswitch1'
DigitalIn microswitch2(D3);         //Instance of the DigitalIn class called 'microswitch2'
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

void fd(int gates);
void bk(int gates);
void rt(int gates);
void lt(int gates);

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

    //music_thread.start(Imperial); // start playing music on the music_thread thread

    /*rotateCounterClockwise(90,50,circumference,width);
    while(true){
        while(myButton == 0);
        rotateCounterClockwise(90,50,circumference,width);
        */
        
        // Write the parts of your code which should run in a loop between here..
        while(true)
        {
            

            for(int x = 1; x <= 3; x++) // run for 3 loops, as 4.28 lanes covers width of board so 5 lane pushes needed in total
            {
                PushLane();
                NextLane(200, 40); // push 200mm and reverse 40mm for total of 160mm across to have overlap
            }
           //next section does the last pushes and 0.28 (44.8mm) part of the board 
            PushLane(); //4th lane push
            TurnLeft(90);
            driveBackward(100, 20, circumference);
            //To add a curved turn to scoop more beads

            //NextLane(40, 0); // drive 40mm to clear last part, 44.8mm left if placed perfectly
            PushLane(); // final lane push
            // return to start and loop again?
            backtostart();
        
        /*//1 meter parallel line test
        while(true)
        {
            driveForward(1000, 50, circumference) // drive 1m (1000mm)
            rotateClockwise(180, 50, circumference, width); // rotate 180 degrees
            driveForward(1000, 50, circumference) // drive 1m (1000mm)
        } */
        // ..and here
        }

       
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
    driveForward(30, 20, circumference);
    wait_us(200000);
    rotateClockwise(90, 20, circumference, width); // rotate 90 degrees 
    wait_us(200000);
    driveForward(fdist, 20, circumference); // drive 200mm to clear bead in next lane
    wait_us(200000);
    driveBackward(bdist, 20, circumference); // drive 40 mm back to cause overlap after bead clearance
    wait_us(200000);
    rotateCounterClockwise(90, 50, circumference, width); // rotate 90 degrees back to push beads
    wait_us(200000);
    driveBackward(60, 20, circumference);
}

void backtostart()
{
    rotateCounterClockwise(90, 20, circumference, width);
    wait_us(200000);
    driveForward(500, 20, circumference); // drive most of length to loop again 
    wait_us(200000);
    rotateClockwise(90, 20, circumference, width);
}




//For Wheel.Speed it's (right motor, left motor)
/*for(int i = 0; i < 4; i++){
                bk(20);
                fd(100);
                bk(120);
                fd(5);
                rt(25);
                fd(50);
                bk(10);
                lt(25);
            }
void fd(int gates){
    Timer timeout;
    timeout.start();
    int lcount = 0;
    int rcount = 0;
    int timeouttime = gates / 20;
    if (timeouttime == 0) timeouttime = 1;

    Wheel.Speed(0.81,0.8);
                while(true){
                  microseconds time = timeout.elapsed_time();
                  if (left_encoder.pulseReceived() > 0) {
                    lcount++;
                    }
                    if(right_encoder.pulseReceived()>0){
                        rcount++;
                    }
                    if ((rcount >= gates && lcount >= gates) || (time > seconds(timeouttime))) {
                      Wheel.Speed(0.0, 0.0);
                      break;
                    }
                }
}


void bk(int gates){
    Timer timeout;
    timeout.start();
    int lcount = 0;
    int rcount = 0;
    int timeouttime = gates / 20;
    if (timeouttime == 0) timeouttime = 1;

    Wheel.Speed(-0.8,-0.81);
                while(true){
                  microseconds time = timeout.elapsed_time();
                  if (left_encoder.pulseReceived() > 0) {
                    lcount++;
                    }
                    if(right_encoder.pulseReceived()>0){
                        rcount++;
                    }
                    if ((rcount >= gates && lcount >= gates) || (time > seconds(timeouttime))) {
                      Wheel.Speed(0.0, 0.0);
                      break;
                    }
                }
}


void rt(int gates){
    Timer timeout;
    timeout.start();
    int lcount = 0;
    int rcount = 0;
    int timeouttime = gates / 10;
    if (timeouttime == 0) timeouttime = 1;

    Wheel.Speed(-0.8,0.8);
                while(true){
                  microseconds time = timeout.elapsed_time();
                  if (left_encoder.pulseReceived() > 0) {
                    lcount++;
                    }
                    if(right_encoder.pulseReceived()>0){
                        rcount++;
                    }
                    if ((rcount >= gates && lcount >= gates) || (time > seconds(timeouttime))) {
                      Wheel.Speed(0.0, 0.0);
                      break;
                    }
                }
}

void lt(int gates){
    Timer timeout;
    timeout.start();
    int lcount = 0;
    int rcount = 0;
    int timeouttime = gates / 10;
    if (timeouttime == 0) timeouttime = 1;
    
    Wheel.Speed(0.8,-0.8);
                while(true){
                  microseconds time = timeout.elapsed_time();
                  if (left_encoder.pulseReceived() > 0) {
                    lcount++;
                    }
                    if(right_encoder.pulseReceived()>0){
                        rcount++;
                    }
                    if ((rcount >= gates && lcount >= gates) || (time > seconds(timeouttime))) {
                      Wheel.Speed(0.0, 0.0);
                      break;
                    }
                }
}
*/
void TurnLeft(float angle) // to be tested
{
    //Initialzing variables
    int rPulseCount = 0; // Number of pulses on the Right, zeroed to ensure count is accurate
    int lPulseCount = 0; // Number of pulses on the Left, zeroed to ensure count is accurate
    
    // Determines the how many full and partial rotataions needed for the distance input and then how many pulses to achieve it
    float outwheel = width*2; // wheels need different turning radius for total number of rotations
    float indist = angle/360*width*3.141; // calculated to turn for inner wheel of buggy's axis
    float outdist = angle/360*outwheel*3.141; // calculated to turn for outer of buggy's axis
    float inRotations = indist/circumference; // calculate number of rotations needed
    float outRotations = outdist/circumference;
    float inpulse = floor(inRotations*PULSES_PER_ROTATION); //calculatesthe number of pulses needed
    float outpulse = floor(outRotations*PULSES_PER_ROTATION);
    // Run until number of pulses calcualted in fucntion are reached
    Wheel.Speed(0.4,0.8); // out wheel needs to move twice as fast to turn the total pulses the same number of times
    while(rPulseCount<=outpulse && lPulseCount<=inpulse)
    {
        //SpeedControl(); Working on it currently
        if (left_encoder.pulseReceived() == 1)
        {
            lPulseCount++;
        }
        if (right_encoder.pulseReceived() == 1)
        {
            rPulseCount++;
        }
    }
    //Stop the wheels
    Wheel.Speed(0.0, 0.0);
}