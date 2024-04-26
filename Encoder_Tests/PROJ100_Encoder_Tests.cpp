#include "mbed.h"
#include "PROJ100_Encoder.h"
#include "PROJ100_Encoder_Tests.h"
#include "motor.h"
#include <chrono>

//*************************************Simple test***********************************************//
// Uses the encoder to measure the time between pulses using a Timer object t.
// Uses non-blocking PROJ100_Encoder::pulseReceived() function which returns 1 if a pulse has been
// detected since the last call, 0 if no pulse has been detected since the last call, -1 if the 
// wheel is stationary and -2 if there is an error

DigitalIn microswitch1(D4);         //Instance of the DigitalIn class called 'microswitch1'
DigitalIn microswitch2(D3);         //Instance of the DigitalIn class called 'microswitch2'
/*
bool checkpress(){
    if (microswitch1 == 1){
        return true;
    }
}*/

void simple_test(){
    // Variables and objects
    DigitalOut led(LED1);       // We will toggle an LED every time we get a pulse
    Timer t;                    // We will use a timer to work out how much time between pulses
    t.start();                  
    uint8_t counter = 0;        // Variable to hold a count of the number of pulses received

    // Apply power to the left motor only
    Wheel.Speed(0.0f,0.6f);

    // This loops runs forever
    while(1){
        // Check to see if we have received a new pulse
        if(left_encoder.pulseReceived()>0){
            // If so, read the pulse time from the timer
            // This line is ugly but it just reads the timer and converts the time to milliseconds
            uint32_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count();
            
            // Reset the timer
            t.reset();

            // Toggle the LED
            led=!led;

            // Only print every 10 pulses
            counter++;
            if(counter % 10 == 0 ){
                printf("Pulse time %dms, Pulses received %d\r\n",ms,counter);   // Print the pulse time
            }
        }
    }
}



//*************************************Speed test***********************************************//
// Uses the encoder information to control the speed of the wheels and display it on the terminal
void speed_test(){

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Experiment with these values
    float target_rpm = 30.0f;       // What rpm are we going to try to achieve?
    float pwm_increment = 0.002f;   // This value affects how quickly we can reach the target but also the stability of our controller
    int loop_delay_ms = 1;          // This sets how often the loop runs
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // We will print the current and target speed periodically using a timer
    Timer print_timer;
    print_timer.start();

    // We will save the previous speed so that it can be printed
    float last_rpm = 0.0f;
    while(1){

        // Check to see if a pulse has been received and it's time in us
        int32_t time = right_encoder.getLastPulseTimeUs();

        // If a pulse has been received..
        if(time>0){

            // Calculate RPM
            int ppr = right_encoder.getPulsesPerRotation();      // To work out how fast we are going we need to know how many pulses are in a complete rotation
            float rpm = (60000000.0f/(ppr*time));               // 60000000us = 60secs
            
            // Calculate error term
            float err = target_rpm-rpm;

            if(target_rpm==0.0f){           // Is the target 0 rpm?...
                Wheel.Speed(0.0f,0.0f);     // If so stop the motors 
            }
            // Otherwise increase or decrease PWM if we are going too fast or slow
            else{
                if(rpm < (target_rpm)){
                    Wheel.Speed(Wheel.getSpeedRight()+pwm_increment,0.0f);
                }
                else if(rpm > target_rpm){
                    Wheel.Speed(Wheel.getSpeedRight()-pwm_increment,0.0f);
                }
            }  

            //Update value for printing
            last_rpm = rpm;
        }
 
        // Check to see if the wheel IS stationary
        else if(time==-2){
            //Update value for printing
            last_rpm = 0.0f;        

            // SHOULD the wheel be stationary? 
            if(target_rpm !=0.0f){
                Wheel.Speed(Wheel.getSpeedRight()+pwm_increment,0.0f);   // If not increase the power to turn the wheel
                                                                        // (This serves to start the motor if it is not yet turning)
            }
            else{
                Wheel.Speed(0.0f,0.0f);                                 //If so, ensure the motor is stopped
            }
        }
        
        // Time will only = -1 if there is an error
        else if(time==-1){
            last_rpm = -999.0f;
        } 
        
        // Print periodically
        if(std::chrono::duration_cast<std::chrono::milliseconds>(print_timer.elapsed_time()).count()>=500){ //Was print_timer.read_ms()
            printf("\033[2J\033[1;1H\r\n");
            printf("****PROJ100 Speed Test****\r\n");
            printf("Target: \t%4.3frpm\r\n",target_rpm);
            printf("Current:\t%4.3frpm\r\n",last_rpm);
            print_timer.reset();
        } 

        // A delay can be used to set how often the loop runs
        ThisThread::sleep_for(std::chrono::milliseconds(loop_delay_ms));
    }
}

//Home made Functions

float pwrIncrement = 0.001;


void driveForward(float dist, float tRPM, float circ){

    
    //Get constants
    int ppr = left_encoder.getPulsesPerRotation();
    float rRPM; //Right RPM
    float lRPM; //Left RPM
    int rPulseCount = 0; // Number of pulses on the Right
    int lPulseCount = 0; // Number of pulses on the Left
    float numRotations = dist/circ; // number of rotations needed
    int loop_delay_ms = 1;          // This sets how often the loop runs


    //Testing Variables
    float LastrRPM = 0;
    float LastlRPM = 0;
        Timer print_timer;
    print_timer.start();




    int pulseTarget = floor(numRotations*ppr); // Number of pulses needed to reach the target
    
    
    bool rolling = true; // Is the cart supposed to be driving
    
    
    int32_t lTime;
    int32_t rTime;

    float pwrIncrement = 0.02f; // Increment for changing power to the wheels
    
    Wheel.Speed(0.5,0.5);

    while(rolling){

        lTime = left_encoder.getLastPulseTimeUs();
        rTime = right_encoder.getLastPulseTimeUs();
        //Increment the pulse counts if the pulse reader gets a new pulse.
        if(lTime>0){
            lPulseCount++;
            lRPM = (60000000.0f/(ppr*lTime));
            if(lRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()-pwrIncrement);
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()+pwrIncrement);
            }
        }
        if(rTime>0){
        
            rPulseCount++;
            rRPM = (60000000.0f/(ppr*rTime));
            if(rRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight()-pwrIncrement,Wheel.getSpeedLeft());
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight()+pwrIncrement,Wheel.getSpeedLeft());
            }
        }

        
        //If both pulse counters are above or equal to the target, stop driving.
        if(rPulseCount>=pulseTarget && lPulseCount>=pulseTarget){
            rolling = false;
        }
        LastlRPM = lRPM;
        LastrRPM = rRPM;


        ThisThread::sleep_for(std::chrono::milliseconds(loop_delay_ms));

    }
    //Stop the wheels
    Wheel.Speed(0.0, 0.0);

}

void driveBackward(float dist, float tRPM, float circ){

    
    //Get constants
    int ppr = left_encoder.getPulsesPerRotation();
    float rRPM; //Right RPM
    float lRPM; //Left RPM
    int rPulseCount = 0; // Number of pulses on the Right
    int lPulseCount = 0; // Number of pulses on the Left
    float numRotations = dist/circ; // number of rotations needed
    int loop_delay_ms = 1;          // This sets how often the loop runs


    //Testing Variables
    float LastrRPM = 0;
    float LastlRPM = 0;
        Timer print_timer;
    print_timer.start();




    int pulseTarget = floor(numRotations*ppr); // Number of pulses needed to reach the target
    
    
    bool rolling = true; // Is the cart supposed to be driving
    
    
    int32_t lTime;
    int32_t rTime;

    float pwrIncrement = 0.02f; // Increment for changing power to the wheels
    
    Wheel.Speed(-0.5,-0.5);

    while(rolling){

        lTime = left_encoder.getLastPulseTimeUs();
        rTime = right_encoder.getLastPulseTimeUs();
        //Increment the pulse counts if the pulse reader gets a new pulse.
        if(lTime>0){
            lPulseCount++;
            lRPM = (60000000.0f/(ppr*lTime));
            if(lRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()+pwrIncrement);
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()-pwrIncrement);
            }
        }
        if(rTime>0){
        
            rPulseCount++;
            rRPM = (60000000.0f/(ppr*rTime));
            if(rRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight()+pwrIncrement,Wheel.getSpeedLeft());
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight()-pwrIncrement,Wheel.getSpeedLeft());
            }
        }

        
        //If both pulse counters are above or equal to the target, stop driving.
        if(rPulseCount>=pulseTarget && lPulseCount>=pulseTarget){
            rolling = false;
        }
        if(microswitch1 == 1 && microswitch2 == 1){
            wait_us(500000);
            rolling = false;
        }
        LastlRPM = lRPM;
        LastrRPM = rRPM;


        ThisThread::sleep_for(std::chrono::milliseconds(loop_delay_ms));

    }
    //Stop the wheels
    Wheel.Speed(0.0, 0.0);
}

void rotateClockwise(float angle, float tRPM, float circ, float width){

    
    //Get constants
    int ppr = left_encoder.getPulsesPerRotation();
    float rRPM; //Right RPM
    float lRPM; //Left RPM
    int rPulseCount = 0; // Number of pulses on the Right
    int lPulseCount = 0; // Number of pulses on the Left
    float dist = angle/360*width*3.141;
    float numRotations = dist/circ; // number of rotations needed
    int loop_delay_ms = 1;          // This sets how often the loop runs


    //Testing Variables
    float LastrRPM = 0;
    float LastlRPM = 0;
        Timer print_timer;
    print_timer.start();




    int pulseTarget = floor(numRotations*ppr); // Number of pulses needed to reach the target
    
    
    bool rolling = true; // Is the cart supposed to be driving
    
    
    int32_t lTime;
    int32_t rTime;

    float pwrIncrement = 0.02f; // Increment for changing power to the wheels
    
    Wheel.Speed(-0.5,0.5);

    while(rolling){

        lTime = left_encoder.getLastPulseTimeUs();
        rTime = right_encoder.getLastPulseTimeUs();
        //Increment the pulse counts if the pulse reader gets a new pulse.
        if(lTime>0){
            lPulseCount++;
            lRPM = (60000000.0f/(ppr*lTime));
            if(lRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()-pwrIncrement);
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()+pwrIncrement);
            }
        }
        if(rTime>0){
        
            rPulseCount++;
            rRPM = (60000000.0f/(ppr*rTime));
            if(rRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight()+pwrIncrement,Wheel.getSpeedLeft());
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight()-pwrIncrement,Wheel.getSpeedLeft());
            }
        }

        
        //If both pulse counters are above or equal to the target, stop driving.
        if(rPulseCount>=pulseTarget && lPulseCount>=pulseTarget){
            rolling = false;
        }
        LastlRPM = lRPM;
        LastrRPM = rRPM;


        ThisThread::sleep_for(std::chrono::milliseconds(loop_delay_ms));

    }
    //Stop the wheels
    Wheel.Speed(0.0, 0.0);


}

void rotateCounterClockwise(float angle, float tRPM, float circ, float width){

    
    //Get constants
    int ppr = left_encoder.getPulsesPerRotation();
    float rRPM; //Right RPM
    float lRPM; //Left RPM
    int rPulseCount = 0; // Number of pulses on the Right
    int lPulseCount = 0; // Number of pulses on the Left
    float dist = angle/360*width*3.141;
    float numRotations = dist/circ; // number of rotations needed
    int loop_delay_ms = 1;          // This sets how often the loop runs


    //Testing Variables
    float LastrRPM = 0;
    float LastlRPM = 0;
        Timer print_timer;
    print_timer.start();




    int pulseTarget = floor(numRotations*ppr); // Number of pulses needed to reach the target
    
    
    bool rolling = true; // Is the cart supposed to be driving
    
    
    int32_t lTime;
    int32_t rTime;

    float pwrIncrement = 0.02f; // Increment for changing power to the wheels
    
    Wheel.Speed(0.5,-0.5);

    while(rolling){

        lTime = left_encoder.getLastPulseTimeUs();
        rTime = right_encoder.getLastPulseTimeUs();
        //Increment the pulse counts if the pulse reader gets a new pulse.
        if(lTime>0){
            lPulseCount++;
            lRPM = (60000000.0f/(ppr*lTime));
            if(lRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()+pwrIncrement);
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight(),Wheel.getSpeedLeft()-pwrIncrement);
            }
        }
        if(rTime>0){
        
            rPulseCount++;
            rRPM = (60000000.0f/(ppr*rTime));
            if(rRPM>tRPM){
                Wheel.Speed(Wheel.getSpeedRight()-pwrIncrement,Wheel.getSpeedLeft());
            }
            else if(lRPM<tRPM){
                Wheel.Speed(Wheel.getSpeedRight()+pwrIncrement,Wheel.getSpeedLeft());
            }
        }

        
        //If both pulse counters are above or equal to the target, stop driving.
        if(rPulseCount>=pulseTarget && lPulseCount>=pulseTarget){
            rolling = false;
        }
        LastlRPM = lRPM;
        LastrRPM = rRPM;

        ThisThread::sleep_for(std::chrono::milliseconds(loop_delay_ms));

    }
    //Stop the wheels
    Wheel.Speed(0.0, 0.0);


}

