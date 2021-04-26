# SIK-Rotary-Encoder
Light weight library to easily integrate and handle rotary encoders in your micros projects

#### Description
Originally it was created as part of several libraries and examples of the AVR Controller Shield by SIKTEC - [Arduino based Controller Shield](http://siktec.net "Arduino based Controller Shield").
This Library is as small as it gets and very well documented - It allows an easy integration of a rotary encoder in an "interrupt way".

## Features
- Initialize pin definition and interrupt handler.
- Supports Rotary Switch (push button).
- Dynamic Rotary Range declaration (upper and lower bounds).
- Programmatically manipulate rotary position.
- Enable / Disable states.
- Circular counting - Loop when reach bounds.
- Custom callbacks.
- Great well documented Examples.

## Installation:
You can install the library via one of the following:
1. Arduino library manager: Search for "SIKTEC Rotary Library" and click install.
2. Download the repositories as a ZIP file and install it on the Arduino IDE by:
	`Sketch -> Include library -> Add .ZIP Library.`
3. Download the library and include it in your project folder - Than you can Include it:
	`#include "src\SIKTEC_Rotary.h"`
