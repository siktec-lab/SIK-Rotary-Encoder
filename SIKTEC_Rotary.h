/******************************************************************************/
// Created by: Shlomo Hassid.
// Release Version : 1.0.6
// Creation Date: 2021-04-18
// Copyright 2021, SIKTEC / SIKDEV.
/******************************************************************************/
/*****************************      Changelog       ****************************
version 1.0.6:
    ->initial, supports callbacks, circular + Push button support
    ->Debounce integrated/
*******************************************************************************/

#ifndef _ARDUINOROTARYENCODER_h
#define _ARDUINOROTARYENCODER_h

#include <Arduino.h>

namespace SIKtec {

    /***************************************************************************/
    /**** Some Defintions - ****************************************************/
    /***************************************************************************/
    // Don't edit - Most of those can be set by the constructor - or define them 
    // prior to the class include.
    #ifndef ROTARY_DEBOUNCE_DELAY_DEFAULT
        // Debounce time in milliseconds:
        #define ROTARY_DEBOUNCE_DELAY_DEFAULT  500 
    #endif
    #ifndef ROTARY_RANGE_DEFAULT
        // default counter range − make sure you are in range of the counter type
        // min, max, step
        #define ROTARY_RANGE_DEFAULT {0,250,1}  
    #endif
    #ifndef ROTARY_COUNTER_TYPE
        // default counter type
        #define ROTARY_COUNTER_TYPE volatile int32_t 
    #endif

    /***************************************************************************/
    /**** Types to be used *****************************************************/
    /***************************************************************************/
    /* 
    * RotaryRange: Struct for defining counter range.
    */
    typedef struct 
    {
        ROTARY_COUNTER_TYPE min; // Minimum included
        ROTARY_COUNTER_TYPE max; // Maximum included
        ROTARY_COUNTER_TYPE step; // Increment / Decrement by each rotary step
    } RotaryRange;

    /* 
    * RotaryState: Struct of the returned state.
    */
    typedef struct
    {
        int8_t                delta; // Indicates the direction of the last rotary step
        ROTARY_COUNTER_TYPE    prev; // Previous counter value (previous since last readState)
        ROTARY_COUNTER_TYPE    pos;  // Current counter position
    } RotaryState;

    /***************************************************************************/
    /**** SikRot CLASS *********************************************************/
    /***************************************************************************/
    class Rotary {
        
    private:
    //public:
        /* Notes:
            -> uint8_t is the definition of byte -> you can still use defined byte
            -> uint16_t is the unsigned int value
        */
        //Pins passed from the constructor
        uint8_t pinA    = 0;
        uint8_t pinB    = 0;
        uint8_t pinSW   = 0;

        volatile uint8_t aFlag      = 0; // Flag when A rotation direction
        volatile uint8_t bFlag      = 0; // Flag when B rotation direction
        ROTARY_COUNTER_TYPE encoderPos     = 0; // Saves current counter position MAX 65,536
        ROTARY_COUNTER_TYPE encoderPosPrev = 0; // Stores previous Position for calculating the delta
        volatile uint8_t reading    = 0; // Used by the ISR's to store last reading of the register

        RotaryRange     range;  // The definition range supplied by the constructor

        
        
        void (*localPointerToCallback)(const RotaryState);
        
    public:

        bool circular   = false; // endless circular counting flag - set by the constructor
        bool enabled    = true;  // A flag to store the state activation of the encoder.
        bool changed    = false; // indicates if change has occurred since last readState
        volatile int32_t lastDebounceTime; // Stores the las time in milli seconds a tick happend
        volatile inline static int32_t debounceDelay;    // Defines teh debounce delay to be use    
    
        /*
        * Constructor: initiate the Rotary Class with some required definitions
        * ----------------------------
        *   encoderAPin:        The pin attached to A gate 
        *   encoderBPin:        The pin attached to B gate
        *   encoderButtonPin:   The pin attached to SW leg - if none set to 0
        *   circularRange:      Set a circular counting - default False
        *   rangeOf:            The range to use min and max values - default `ROTARY_RANGE_DEFAULT`
        *   _steps:             Counting Steps for each tick - default ROTARY_COUNTING_STEPS_DEFAULT
        *   returns: self 
        */
        Rotary(
            uint8_t encoderAPin, 
            uint8_t encoderBPin,
            uint8_t encoderButtonPin,
            bool circularRange = false,
            RotaryRange rangeOf = ROTARY_RANGE_DEFAULT
        );
        /*
        * attachInterupts: registers the interrupts 
        * ----------------------------
        * the reason we are forcing this call is to attach an instance 
        * this way we can create multiple instances of this class (use several rotary encoders)
        *   A_callback: void (*A_callback)(void)
        *   B_callback: void (*B_callback)(void)
        *   returns: void 
        */
        void attachInterupts(void (*A_callback)(void), void (*B_callback)(void));
        
        /*
        * setCallback: attach a user custom callback that takes `RotaryState` and returns void
        * ----------------------------
        *   userDefinedCallback: function pointer
        * 
        *   returns: void 
        */
        inline void setCallback(void(*userDefinedCallback)(const RotaryState)) {
            localPointerToCallback = userDefinedCallback; 
        }
        /*
        * activate: enables and disables the rotary counting 
        * ----------------------------
        *   state: boolean state
        * 
        *   returns: void 
        */
        void activate(bool state);
        /*
        * enable: enables the rotary counting 
        * ----------------------------
        *   returns: void 
        */
        void enable();
        /*
        * disable: disables the rotary counting 
        * ----------------------------
        *   returns: void 
        */
        void disable();

        /*
        * setRange: sets the range of the rotary dynamicly 
        * ----------------------------
        * If position is out of the range it will be set to min
        * 
        *   rangeOf: The range to use min and max values
        *   returns: current position, type is set by ROTARY_COUNTER_TYPE
        */
        ROTARY_COUNTER_TYPE setRange(const RotaryRange rangeOf);

        /*
        * setPos: sets programmatically the rotary counting position
        * ----------------------------
        * If position is out of the range it will be ignored
        * 
        *   pos: the counting position to set
        *   returns: current position, type is set by ROTARY_COUNTER_TYPE
        */
        ROTARY_COUNTER_TYPE setPos(const ROTARY_COUNTER_TYPE pos);

        /*
        * stepDown: performs a step decrement N times
        * ----------------------------
        * If position is out of the range it will be handled by this function
        * also depends if its circular or not
        * 
        *   times: How many steps to take?
        *   returns: current position, type is set by ROTARY_COUNTER_TYPE
        */
        ROTARY_COUNTER_TYPE stepDown(uint8_t times = 1, bool callback = false);

        /*
        * stepUp: performs a step increment N times
        * ----------------------------
        * If position is out of the range it will be handled by this function
        * also depends if its circular or not
        * 
        *   times: How many steps to take?
        *   returns: current position, type is set by ROTARY_COUNTER_TYPE
        */
        ROTARY_COUNTER_TYPE stepUp(uint8_t times = 1, bool callback = false);

        /*
        * currentState: reads safely the rotary counting position 
        * ----------------------------
        * If position is out of the range it will be ignored
        * 
        *   returns: RotaryState => {delta, pos, prev}
        */
        RotaryState currentState();

        /*
        * readState: reads the rotary counting position and updates flags
        * ----------------------------
        * NOTE -> prev position is updated each read state
        * it can be handy if you need to check how many ticks happend since last check.
        * 
        *   returns: RotaryState => {delta, pos, prev}
        */
        RotaryState readState();
        
        //Default interrupts handlers:
        void interA();
        void interB();
    };

}
#endif
