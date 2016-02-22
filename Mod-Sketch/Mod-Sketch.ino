/* Mod Sketch v0.2.2 -- written by Craig Durkin / Comingle. */
/* Additional Commands & Documentation added by Holger 'Lynxx' Hippenstiel */
/* This software comes pre-loaded on Comingle Mod sex toys */

// Include necessary libraries: Wire.h and WiiChuck.h for the nunchuck, OSSex.h for the Mod library
#include <Wire.h>
#include <OSSex.h>
#include <WiiChuck.h>

// Add the pattern library.
#include "patterns.h"

/*
Available commands over Serial:
- <n>         = Set Value for all Motors : Returns nothing
0 <n>         = Set Value for Motor 1 : Return current Value
1 <n>         = Set Value for Motor 2 : Return current Value
2 <n>         = Set Value for Motor 3 : Return current Value
c             = Cycle Pattern : Returns running pattern
g             = Get number of currently running pattern : Returns running pattern
i             = Read input 0 or 1 : Returns value of input
l <n>         = Set LED power : Return current Value
p             = Decrease Power : Returns new Powerlevel
P             = Increase Power : Returns new Powerlevel
r <n>         = Run a specific pattern, r,0; r,1; etc. : Returns running pattern
s             = Stop all Motors, reset Patternnumber, Power & Timescale : Returns nothing
t             = Decrease pattern time, as in everything goes faster : Returns new Timescale
T             = Increase pattern time, as in everything goes slower : Returns new Timescale
{<n>,<n>,<n>} = Set Value for Motor1,2 and 3 in a single line : Returns nothing

New from v0.2.2:
G = get Powerlevel : Returns Powerlevel
v = get Version : Returns Version information
*/

const String txtVersion = "Dilduino v0.2.2";

void setup() {
	Toy.setID(1); // Beta Version
	
  // Button will increase/decrease power by 20%
  Toy.setPowerScaleStep(0.2);
  
  // Blip all the motors and flash the LED to show that everything is working and the device is on.
  startupSequence();

  // Set the patterns that the button will cycle through. Toy will do nothing on startup, and clicking the button once will run the 'first' pattern
  // Clicking again will run 'second', etc.

  Toy.addPattern(first);
  Toy.addPattern(second);
  Toy.addPattern(third);
  Toy.addPattern(pulse);
  Toy.addPattern(fadeCos);
  Toy.addPattern(weird3);
  Toy.addPattern(sharpRamp);
  Toy.addPattern(flicker);
  Toy.addPattern(weird2);
  Toy.addPattern(fadeOffset);
  Toy.addPattern(pulse2);

  // Set up the button click handlers
  Toy.attachClick(click);
  Toy.attachDoubleClick(doubleClick);
  Toy.attachLongPressStart(longPress);

  // Start the Serial console
  Serial.begin(9600);
}

void loop() {
  serialProcessor();
}

// Serial console. Read a character in to inChar, and a value in to val
void serialProcessor() { 
  if (Serial.available() > 0) {
  	char inChar = Serial.read();
    byte val;
    switch (inChar) {
    	case 'l': // Set LED-Power
        val = Serial.parseInt();
        Toy.setLED(0,val);
        Serial.println(val);
        break;
      case '0': // Set Power for Motor1-3 directly
      case '1':
      case '2':
        val = Serial.parseInt();
        Toy.setOutput(inChar, val);
        Serial.println(val);
        break;
      case '-':  // Catch '-1', set power of all motors
      {
        int out = Serial.parseInt();
        out *= -1;
        val = Serial.parseInt();
        Toy.setOutput(out,val);
        break;
      }
      case '{': // Set Power for all 3 Motors in a single line
        int motors[3];
        motors[0] = Serial.parseInt();
        motors[1] = Serial.parseInt();
        motors[2] = Serial.parseInt();
        for (int i = 0; i < 3; i++) {
          if (motors[i] >= 0) {
            Toy.setOutput(i, motors[i]);
          }
        }
        break;
      case 'p': // Decrease Toy power, same as longpressing the button
        Serial.println(Toy.decreasePower());
        break;
      case 'P': // Increase Toy power, same as doubleclicking the button
        Serial.println(Toy.increasePower());
        break;
      case 't': // Decrease pattern time, as in everything goes faster
        Serial.println(Toy.decreaseTime());
        break;
      case 'T': // Increase pattern time, as in everything goes slower : Returns new Timescale
        Serial.println(Toy.increaseTime());
        break;
      case 'r': // Run a specific pattern, r,0; r,1; r 2 etc.
        val = Serial.parseInt();
        Toy.runPattern(val);
        Serial.println(Toy.getPattern());
        break;
      case 'g': // Get number of currently running pattern
        Serial.println(Toy.getPattern());
        break;
      case 's': // Stop all Motors, reset Patternnumber, Power & Timescale
        Toy.stop();
        break;
      case 'c': // Cycle Pattern, same as single clicking the button
        Toy.cyclePattern();
        Serial.println(Toy.getPattern());
        break;
      case 'i': // Read input 0 or 1 and print it to serial port
      {
        int in = Serial.parseInt();
        in %= Toy.device.inCount;
        Serial.println(Toy.getInput(in));
        break;
      }
      case 'v': // Print Version information
        Serial.println(txtVersion);
        break;
      case 'G': // Get the Powerlevel
      	Serial.println(Toy.getPowerScaleFactor());
      	break;
      default:
      	Serial.println("Unknown command: "+inChar);
    }
  }
}

// Cycle through all the outputs, turn the LED on and leave it on to show that we're on
void startupSequence() {
  int outs = Toy.device.outCount;
  int outInterval = 100 / outs;

  Toy.setLED(0,128);

  for (int i = 0; i < outs; i++) {
    Toy.setOutput(i, 100+outInterval*i);
    delay(100);
    Toy.setOutput(i, 0);
    delay(50);
  }
  for (int i = 0; i < 3; i++) {
    Toy.setLED(0,0);
    delay(100);
    Toy.setLED(0,128);
    delay(100);
  }
}

// Click handler. Currently moves to next pattern.
void click() {
  Toy.cyclePattern();
}

// Double click handler Currently increases power.
void doubleClick() {
 Toy.increasePower();
}

// Click and hold handler. Currently decreases power.
void longPress() {
  Toy.decreasePower();
}
