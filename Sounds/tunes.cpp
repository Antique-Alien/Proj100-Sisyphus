 #include "mbed.h"
 #include "pwm_tone.h"
 #include "motor.h"
 #define TIME_PERIOD 2             //Constant compiler Values here 2 equates to 2ms or 500Hz base Frequency
 PwmOut buzzer(D5);
  
 void tone1(void){
    Tune(buzzer,1e6/C6,4);
    buzzer.period_ms(TIME_PERIOD);
 }
 
void tone2(void){
    Tune(buzzer,1e6/E6,4);
    buzzer.period_ms(TIME_PERIOD);
 }

void close_encounter(int s){
    Tune(buzzer,1e6/Dd5,s*2); //It's a close encounter of the 3rd kind of!!!
    Tune(buzzer,1e6/E5,s*2);
    Tune(buzzer,1e6/C5,s*2);
    Tune(buzzer,1e6/C4,s*2);
    Tune(buzzer,1e6/G4,s*4);
    buzzer.period_ms(TIME_PERIOD);
 }
 
 void jingle_bells(int s){
    Tune(buzzer,1e6/E4,s*2); //Jingle Bells!!!
    Tune(buzzer,1e6/E4,s*2);
    Tune(buzzer,1e6/E4,s*4);
  
    Tune(buzzer,1e6/E4,s*2);
    Tune(buzzer,1e6/E4,s*2);
    Tune(buzzer,1e6/E4,s*4);
    
    Tune(buzzer,1e6/E4,s*2);
    Tune(buzzer,1e6/G4,s*2);
    Tune(buzzer,1e6/C4,s*3);
    Tune(buzzer,1e6/E4,s*1);
    
    Tune(buzzer,1e6/E4,s*8);
  
    Tune(buzzer,1e6/F4,s*2);
    Tune(buzzer,1e6/F4,s*2);
    Tune(buzzer,1e6/F4,s*3);
    Tune(buzzer,1e6/F4,s*1);
    
    Tune(buzzer,1e6/F4,s*2);
    Tune(buzzer,1e6/E4,s*2);
    Tune(buzzer,1e6/E4,s*2);
    Tune(buzzer,1e6/E4,s*1);
    Tune(buzzer,1e6/E4,s*1);
    
    Tune(buzzer,1e6/E4,s*2);
    Tune(buzzer,1e6/Dd4,s*2);
    Tune(buzzer,1e6/Dd4,s*2);
    Tune(buzzer,1e6/E4,s*2);
  
    Tune(buzzer,1e6/Dd4,s*4);
    Tune(buzzer,1e6/G4,s*4);
    buzzer.period_ms(TIME_PERIOD);
  }
void twinkle(int s){
    Tune(buzzer,1e6/C4,s*2); //Twinkle Little Star!!!!!
    Tune(buzzer,1e6/C4,s*2);
    Tune(buzzer,1e6/G4,s*2);
    Tune(buzzer,1e6/G4,s*2);
  
    Tune(buzzer,1e6/Aa4,s*2);
    Tune(buzzer,1e6/Aa4,s*2);
    Tune(buzzer,1e6/G4,s*4);
    
    Tune(buzzer,1e6/F4,s*2);
    Tune(buzzer,1e6/F4,s*2);
    Tune(buzzer,1e6/E4,s*2);
    Tune(buzzer,1e6/E4,s*2);

    Tune(buzzer,1e6/Dd4,s*2);
    Tune(buzzer,1e6/Dd4,s*2);
    Tune(buzzer,1e6/C4,s*4);
    buzzer.period_ms(TIME_PERIOD);
  }

void Rick(void){
    playRick(buzzer);
}

void Meglovania(void){
    playMeglovania(buzzer);
}

void Imperial(void){
    playImperialMarch(buzzer);
}

void RushE(void){
    playRushE(buzzer);
}

void Frog(void){
    playFrog(buzzer);
}

void Victory(void){
    playVictory(buzzer);
}

void PlayAll(void){          //Function to play all of the songs one after another.
    playMeglovania(buzzer);
    playRushE(buzzer);
    playImperialMarch(buzzer);
    playRick(buzzer);
    playFrog(buzzer);
}

void PlaySong(int i){        //function to play a specific song dependent on the integer passed to it
    switch (i){
        case 0:
        playRushE(buzzer);
        break;

        case 1:
        playMeglovania(buzzer);
        break;

        case 2:
        playImperialMarch(buzzer);
        break;

        case 3:
        playRick(buzzer);
        break;

        case 4:
        playFrog(buzzer);
        break;

        default:
        break;
    }
}
