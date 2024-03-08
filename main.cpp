
#include "mbed-os/mbed.h"

#include "motor.h"
#include "tunes.h"
#include "pwm_tone.h"
#include "PROJ100_Encoder.h"
#include "PROJ100_Encoder_Tests.h"

#define TIME_PERIOD 10             //Constant compiler Values here 10 equates to 10ms or 100Hz base Frequency
#define ENCODER_PIN_LEFT            D8
#define ENCODER_PIN_RIGHT           D6 
#define PULSES_PER_ROTATION         60
#define DEBOUNCE_US                 30000
#define circumference                209 // Circumference of wheel in mm
#define width                       113.5 // width of Cart in mm

DigitalIn microswitch1(D4);         //Instance of the DigitalIn class called 'microswitch1'
DigitalIn microswitch2(D3);         //Instance of the DigitalIn class called 'microswitch2'
DigitalIn myButton(USER_BUTTON);    //Instance of the DigitalIn class called 'myButton'   
DigitalOut greenLED(LED1);          //Instance of the DigitalOut class called 'greenLED'

Motor Wheel(D13,D11,D9,D10);      //Instance of the Motor Class called 'Wheel' see motor.h and motor.cpp

PROJ100_Encoder right_encoder (ENCODER_PIN_RIGHT,PULSES_PER_ROTATION);  //Instance of the PROJ100Encoder class called 'right_encoder'
PROJ100_Encoder left_encoder(ENCODER_PIN_LEFT,PULSES_PER_ROTATION);     //Instance of the PROJ100Encoder class called 'left_encoder'

UnbufferedSerial ser(USBTX,USBRX,115200);   // Serial object for printing info

// function names to run bead pushing
void PushLane(); 
void NextLane();
void backtostart():

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

    // simple_test();
    //speed_test();
    music_thread.start(Rick); // start playing Never Gonna Give You Up by Rick Astley on the music_thread thread

    /*rotateCounterClockwise(90,50,circumference,width);
    while(true){
        while(myButton == 0);
        rotateCounterClockwise(90,50,circumference,width);*/
        
        // Write the parts of your code which should run in a loop between here..
        while(true)
        {
            for(int x = 1; x <= 3; x++;) // run for 3 loops, as 4.28 loops covers width of board so 5 lane pushes needed in total
            {
                PushLane(); 
                NextLane(200, 40);
            }
            //next section does the last pushes and 0.28 part of the board Maybe an boolean to only run once to allow looping to noext crash?
            PushLane(); //4th lane push
            NextLane(40, 0); // drive 40mm to clear last part
            PushLane(); // final lane push
            // return to start and loop again?
            backtostart();
        }
        // ..and here

    };
}

// Pushes the lane and returns to start position, no varible as lane length is always the same
void PushLane() 
{
    driveForward(307, 50, circumference); // drive 307 mm up to arena divider
    driveBackward(307, 50, circumference); // drive 307 mm back to arena beginning
}    

// moves over to next lane, with 20mm overlap on previous lane, clears some beads from new lane
void NextLane(float fdist, float bdist)
{
    rotateClockwise(90, 50, circumference, width); // rotate 90 degrees 
    driveForward(fdist, 50, circumference); // drive 200mm to clear bead in next lane
    driveBackward(bdist, 50, circumference); // drive 40 mm back to cause overlap after bead clearance
    rotateCounterClockwise(90, 50, circumference, width); // rotate 90 degrees back to push beads
}

void backtostart()
{
   rotateCounterClockwise(90, 50, circumference, width);
    driveForward(670, 50, circumference); // drive most of length to loop again 
}