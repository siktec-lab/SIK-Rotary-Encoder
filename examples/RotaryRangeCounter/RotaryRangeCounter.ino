/*
  Rotary encoder - Counting inside a range 
  Output position ad more usefull information to the Serial monitor
 */
#include <Arduino.h>
#include <SIKTEC_Rotary.h>

//Define base values: 
#define ROTARY_ENCODER_A_PIN 2
#define ROTARY_ENCODER_B_PIN 3
#define ROTARY_ENCODER_BUTTON_PIN 4
#define STARTING_INTERVAL 500
enum IntervalLimits { 
	MAX = 1200, 
	MIN = 20, 
	STEP = 10 
};

//Rotary global variables 
volatile int prevPos = -1;  // Variable to store last measured Rotary encoder position

//Led state:
int ledState = LOW;         // The LED state that is being toggled in the loop

//the current rotary state:
SIKtec::RotaryState interval;

//Intiate the Rotary encoder helper class:
SIKtec::Rotary rotary = SIKtec::Rotary(
    ROTARY_ENCODER_A_PIN, 
    ROTARY_ENCODER_B_PIN, 
    ROTARY_ENCODER_BUTTON_PIN,
    false, // Circular mode? -> infinite loop counting.
    {IntervalLimits::MIN, IntervalLimits::MAX, IntervalLimits::STEP}
);

/***************************************************************************/
/**** Functions ************************************************************/
/***************************************************************************/
//ISR to handle rotary change interupt over the button switch
ISR (PCINT2_vect) 
{
    if ((millis() - rotary.lastDebounceTime) > SIKtec::Rotary::debounceDelay && digitalRead(ROTARY_ENCODER_BUTTON_PIN) == LOW) {
        cli();
        Serial.println("Rotary Push Button ISR");
        rotary.lastDebounceTime = millis();
        SIKtec::RotaryState currnet = rotary.readState();
        customCallback(currnet);
        sei();
    }
}

/*! customCallback
    @brief  Example of a user based callback attached to the rotary
    @param  state the current state of the rotary encoder 
*/
void customCallback(SIKtec::RotaryState state) {
    Serial.print(F("Rotary State -> Delta: "));
    Serial.print(state.delta);
    Serial.print(F(" Previous: "));
    Serial.print(state.prev);
    Serial.print(F(" Current: "));
    Serial.println(state.pos);
}

/***************************************************************************/
/**** MAIN LOGIC - Setup + Main Loop ***************************************/
/***************************************************************************/

void setup()   {
    
    //Enable PIE2 change int:
    PCICR |= 0B00000100;
    PCMSK2 |= 0B00010000; // Enable D4

    //Initiate serial port for debuging:
    Serial.begin(9600);
    while (!Serial) { ; }

    //Initiate hardware controls:
    //Rotary + integrated push button - Pin modes are handled in the class
    rotary.attachInterupts(
        []{ rotary.interA(); }, // void lambda function that calls the Rotary interA
        []{ rotary.interB(); } // void lambda function that calls the Rotary interA
    );
    rotary.setPos(STARTING_INTERVAL); // Sets default position of the rotary
    rotary.setCallback(customCallback);
}

void loop() {

    //Check if rotation happend:
    if (rotary.changed) {
        //Update current interval which is the rotary position
        interval = rotary.readState(); 
	Serial.println(F("doing someting from main loop - After Rotary changed"));
    }
}
