/******************************************************************************/
// Created by: Shlomo Hassid.
// Release Version : 1.0.6
// Creation Date: 2021-04-18
// Copyright 2021, SIKTEC / SIKDEV.
/******************************************************************************/

#include "SIKTEC_Rotary.h"

namespace SIKtec {
    /*
    * Constructor: initiate the Rotary Class with some required definitions
    */
    Rotary::Rotary (
        uint8_t encoderAPin,
        uint8_t encoderBPin,
        uint8_t encoderButtonPin,
        bool circularRange,
        RotaryRange rangeOf
    ) {
        //Debounce realated:
        Rotary::debounceDelay = ROTARY_DEBOUNCE_DELAY_DEFAULT;
        Rotary::lastDebounceTime = millis();

        //Define pins:
        if (0 != encoderAPin) this->pinA = encoderAPin;
        if (0 != encoderBPin) this->pinB = encoderBPin;
        if (0 != encoderButtonPin) this->pinSW = encoderButtonPin;
        pinMode(this->pinA, INPUT);
        pinMode(this->pinB, INPUT);
        pinMode(this->pinSW, INPUT);

        //Save encoder range / steps / circular:
        this->circular = circularRange;
        this->setRange(rangeOf);
        this->setPos(rangeOf.min);

        //Define default callback:
        this->setCallback([](const RotaryState){  });
    }
    /*
    * attachInterupts: registers the interrupts  
    */
    void Rotary::attachInterupts(void (*A_callback)(void), void (*B_callback)(void)) {
        attachInterrupt(digitalPinToInterrupt(this->pinA),  A_callback,  RISING);
        attachInterrupt(digitalPinToInterrupt(this->pinB),  B_callback,  RISING);
    }
    /*
    * activate: enables and disables the rotary counting 
    */
    void Rotary::activate(bool state) {
        this->enabled = state;
        this->changed = state;
    }
    /*
    * enable: enables the rotary counting 
    */
    void Rotary::enable() {
        this->enabled = true;
        this->changed = true;
    }
    /*
    * disable: disables the rotary counting 
    */
    void Rotary::disable() {
        this->enabled = false;
        this->changed = false;
    }
    /*
    * setRange: sets the range of the rotary dynamicly 
    */
    ROTARY_COUNTER_TYPE Rotary::setRange(const RotaryRange rangeOf) {
        cli();
        this->range = rangeOf;
        if (this->encoderPos > rangeOf.max || this->encoderPos < rangeOf.min) {
            this->encoderPos = rangeOf.min;
            this->encoderPosPrev = rangeOf.min;
            if (this->enabled) this->changed = true;
        }
        sei();
        return this->encoderPos;
    }
    /*
    * setPos: sets programmatically the rotary counting position
    */
    ROTARY_COUNTER_TYPE Rotary::setPos(const ROTARY_COUNTER_TYPE pos) {
        cli();
        if (pos <= this->range.max && pos >= this->range.min) {
            this->encoderPos = pos;
            this->encoderPosPrev = pos;
            if (this->changed) this->changed = true;
        }
        sei();
        return this->encoderPos;
    }
    /*
    * currentState: reads safely the rotary counting position 
    */
    RotaryState Rotary::currentState() {
        RotaryState ret = {0, 0, 0};
        ret.pos = this->encoderPos;
        ret.prev = this->encoderPosPrev;
        if (ret.pos > ret.prev) {
            ret.delta = 1;
        }
        else if (ret.pos < ret.prev) {
            ret.delta = -1;
        }
        else if (ret.pos == ret.prev) {
            ret.delta = 0;
        }
        return ret;
    }
    /*
    * readState: reads the rotary counting position and updates flags
    */
    RotaryState Rotary::readState() {
        cli();
        RotaryState ret = this->currentState();
        this->changed = false;
        if (ret.delta != 0) this->encoderPosPrev = this->encoderPos;
        sei();
        return ret;
    }
    /*
    * stepDown: performs a step decrement N times
    */
    ROTARY_COUNTER_TYPE Rotary::stepDown(uint8_t times, bool callback) {
        ROTARY_COUNTER_TYPE candid;
        for (uint8_t i = 0; i < times; i++) {
            candid = this->encoderPos - this->range.step;
            if (!this->circular && candid < this->range.min) {
                this->encoderPos = this->range.min;
                //We avoid change flag here because its not circular.
            } else if (this->circular && candid < this->range.min) {
                this->encoderPos = this->range.max - (this->range.step - (this->encoderPos - this->range.min));
                this->changed = true;
            } else {
                this->encoderPos -= this->range.step;
                this->changed = true;
            }
        }
        
        //Call attached callback:
        if (callback)
            this->localPointerToCallback(
                this->currentState()
            );
        return this->encoderPos;
    }
    /*
    * stepUp: performs a step increment N times
    */
    ROTARY_COUNTER_TYPE Rotary::stepUp(uint8_t times, bool callback) {
        ROTARY_COUNTER_TYPE candid;
        for (uint8_t i = 0; i < times; i++) {
            candid = this->encoderPos + this->range.step;
            if (!this->circular && candid > this->range.max) {
                this->encoderPos = this->range.max;
                //We avoid change flag here because its not circular.
            } else if (this->circular && candid > this->range.max) {
                this->encoderPos = this->range.min + (this->range.step - (this->range.max - this->encoderPos));
                this->changed = true;
            } else {
                this->encoderPos += this->range.step;
                this->changed = true;
            }
        }
        //Call attached callback:
        if (callback)
            this->localPointerToCallback(
                this->currentState()
            );
        return this->encoderPos;
    }

    void Rotary::interA()
    {
    if (!this->enabled) return;
    cli();
    this->reading = PIND & 0xC;
    if (this->reading == B00001100 && this->aFlag)
    {
        this->stepDown(1, true);
        this->bFlag = 0;
        this->aFlag = 0;
    } else if (this->reading == B00000100)
        this->bFlag = 1;
    sei();
    }

    void Rotary::interB()
    {
        if (!this->enabled) return;
        cli();
        this->reading = PIND & 0xC;
        if (this->reading == B00001100 && this->bFlag)
        {
            this->stepUp(1, true);
            this->bFlag = 0;
            this->aFlag = 0;
        }
        else if (this->reading == B00001000)
            this->aFlag = 1;
        sei();
    }

}
