 /*
 *  © 2023,2026 Ross Scanlon
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this code.  If not, see <https://www.gnu.org/licenses/>.
*/

// This is a DCC Accessory Decoder to drive 16 Servo Turnouts
// Based on the NMRA Pulsed 8 stationary decoder


#include "defines.h"

#include <NmraDcc.h>

#include <Wire.h>

#include <Adafruit_PWMServoDriver.h>

#include "PinPulser.h"


#include "version.h"

#include "variables.h"

#include "functions.h"



void setup()
{
#ifdef ENABLE_SERIAL
  MYSERIAL.begin(115200);
  uint8_t maxWaitLoops = 255;
  while(!Serial && maxWaitLoops--)
    delay(20);
#endif

  setVersion();

  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
  // Many Arduino Cores now support the digitalPinToInterrupt() function that makes it easier to figure out the
  // Interrupt Number for the Arduino Pin number, which reduces confusion. 

#ifdef digitalPinToInterrupt
  Dcc.pin(DCC_PIN, 0);
#else
  Dcc.pin(0, DCC_PIN, 1);
#endif

#ifdef ENABLE_DCC_ACK
  pinMode(ENABLE_DCC_ACK, OUTPUT);
#endif

  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, DCC_DECODER_VERSION_NUM, FLAGS_OUTPUT_ADDRESS_MODE | FLAGS_DCC_ACCESSORY_DECODER, 0 );

#ifdef ENABLE_SERIAL
  MYSERIAL.print("Rosscoe Train DCC 16 Servo Accessory Decoder. ");

  MYSERIAL.print(F("Version: "));
  MYSERIAL.print(versionBuffer[0]);
  MYSERIAL.print(F("."));
  MYSERIAL.print(versionBuffer[1]);
  MYSERIAL.print(F("."));
  MYSERIAL.println(versionBuffer[2]);
 
  MYSERIAL.println();
#endif

  Wire.begin();

  // Test and init 0x40
  Wire.beginTransmission(0x40);
  if (Wire.endTransmission() == 0)
   {
    MYSERIAL.println("PCA9685 at 0x40 detected.");
    pwm1 = new Adafruit_PWMServoDriver(0x40);
    pwm1->begin();
    pwm1->setPWMFreq(SERVO_FREQ);
    numberOfTurnouts1 = NUM_TURNOUTS;
    pinPulser1 = new PinPulser();
   }

  // Test and init 0x41
  Wire.beginTransmission(0x41);
  if (Wire.endTransmission() == 0)
   {
    MYSERIAL.println("PCA9685 at 0x41 detected.");
    pwm2 = new Adafruit_PWMServoDriver(0x41);
    pwm2->begin();
    pwm2->setPWMFreq(SERVO_FREQ);
    numberOfTurnouts2 = NUM_TURNOUTS;
    pinPulser2 = new PinPulser();
   }

  // Test and init 0x20
//  Wire.beginTransmission(0x20);
//  if (Wire.endTransmission() == 0)
//   {
//    MYSERIAL.println("PCA9555 at 0x20 detected.");

//   }

// set pins for shift register to output

#ifdef USE_SHIFT_REGISTER
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
 
#else

// set digital and analog pins defined in outputs to OUTPUT

  for (uint8_t i=0; i < NUM_OF_LEDS; i++)
   {
     pinMode(outputs[i], OUTPUT);
//     digitalWrite(outputs[i], LOW);
   }
#endif

// set DCC acknowledge pin to output

#ifdef ENABLE_DCC_ACK
  pinMode(ENABLE_DCC_ACK, OUTPUT);
#endif

#ifdef FORCE_RESET_FACTORY_DEFAULT_CV
  notifyCVResetFactoryDefault(); 
#ifdef ENABLE_SERIAL
  MYSERIAL.println("Resetting CVs to Factory Defaults");
#endif
#endif

  if( FactoryDefaultCVIndex == 0)	// Not forcing a reset CV Reset to Factory Defaults so initPinPulser
   {
	  initPinPulser();
   }

}


/*
 *
 * loop start
 *
*/

void loop()
 {
#ifdef LEARNING
  static int learningbuttonOldval = 0,learningbuttonVal = 0;
#endif

  // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  Dcc.process();

#ifndef FORCE_RESET_FACTORY_DEFAULT_CV  
  pinPulser1->process();
  if (pinPulser2)
   {
    pinPulser2->process();
   }
#endif

  if( FactoryDefaultCVIndex && Dcc.isSetCVReady())
   {
    FactoryDefaultCVIndex--; // Decrement first as initially it is the size of the array
    uint16_t cv = FactoryDefaultCVs[FactoryDefaultCVIndex].CV;
    uint8_t val = FactoryDefaultCVs[FactoryDefaultCVIndex].Value;
#ifdef ENABLE_SERIAL
#ifdef DEBUG_MSG
    MYSERIAL.print("loop: Write Default CV: "); MYSERIAL.print(cv,DEC); MYSERIAL.print(" Value: "); MYSERIAL.println(val,DEC);
#endif     
#endif
    Dcc.setCV( cv, val );
    
    if( FactoryDefaultCVIndex == 0)	// Is this the last Default CV to set? if so re-initPinPulser
	    initPinPulser();
   }

  ////////////////////////////////////////////////////////////////
  // check if the learning button has been enabled
  //
  // This is the jumper on A6 marked Learn on the PCB.
  // jumper must be in place to enter learning mode
  // When in learning mode send a close or throw command from the command station
  // and the address will be programmed into the decoder
  //
  ////////////////////////////////////////////////////////////////

#ifdef LEARNING

  learningbuttonVal = dr(LEARNINGBUTTON);

  if (learningbuttonOldval != learningbuttonVal)
   {
    learningMode = learningbuttonVal;
#ifndef ARDUINO_ARCH_ESP32
    if (learningMode == HIGH) showAcknowledge(3);
#endif

   }
  learningbuttonOldval = learningbuttonVal;
#endif


#ifdef ENABLE_SERIAL

// see if there are serial commands

  readString="";              //empty for next input

  while (MYSERIAL.available())
   {
    char c = MYSERIAL.read();     //gets one byte from serial buffer
    readString += c;            //makes the string readString
    delay(10);                   //slow looping to allow buffer to fill with next character
   }

// act on serial commands

  if (readString.length() >0)
   {
    doSerialCommand(readString);
   } 
#endif

  if (pinPulser1->getUpdatePosition())
   {
    for (int i = 0; i < numberOfTurnouts1; i++)
     {
      Dcc.setCV(CV_USER_BASE_ADDRESS + 4 + (i * CV_PER_OUTPUT), pinPulser1->getServoPosition(i) / 10);
#ifdef DEBUG_MSG
      MYSERIAL.print("i: ");MYSERIAL.print(i);MYSERIAL.print(" pos: ");MYSERIAL.println(pinPulser1->getServoPosition(i));
#endif
     }
    pinPulser1->setUpdatePosition();
   }

  if (pinPulser2)
   {
    if (pinPulser2->getUpdatePosition())
     {
      for (int i = 0; i < numberOfTurnouts1; i++)
       {
        Dcc.setCV(CV_USER_BASE_ADDRESS_2 + 4 + (i * CV_PER_OUTPUT), pinPulser2->getServoPosition(i) / 10);
#ifdef DEBUG_MSG
        MYSERIAL.print("i: ");MYSERIAL.print(i);MYSERIAL.print(" pos: ");MYSERIAL.println(pinPulser2->getServoPosition(i));
#endif
       }
      pinPulser2->setUpdatePosition();
     }
   }
  
 }
