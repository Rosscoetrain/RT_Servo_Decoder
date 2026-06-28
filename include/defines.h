/*
 * defines.h
 */

/*
 *  © 2023 Ross Scanlon
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

#ifndef DEFINES_H
#define DEFINES_H

// If we haven't got a custom config.h, use the example.
#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif


#ifdef ENABLE_SERIAL

// this is the Serial to use
#define MYSERIAL Serial

// You can print every DCC packet by un-commenting the line below
//#define NOTIFY_DCC_MSG

// You can print every notifyDccAccTurnoutOutput call-back by un-commenting the line below
//#define NOTIFY_TURNOUT_MSG

// You can also print other Debug Messages uncommenting the line below this prints most debug messages
//#define DEBUG_MSG

// level of debug messages 0 - 5
//#define DEBUG 6

#endif

// Un-Comment the line below to include learning function
#define LEARNING

// Un-Comment the line below to use 74HC595 shift registers for the LEDS
#define USE_SHIFT_REGISTER

// Un-Comment the lines below to Enable DCC ACK for Service Mode Programming Read CV Capablilty
#define ENABLE_DCC_ACK  15  // This is A1 on the Iowa Scaled Engineering ARD-DCCSHIELD DCC Shield

#define DCC_PIN     2

#define NUM_TURNOUTS 16                // Set Number of Turnouts (Pairs of Pins)

#define DCC_DECODER_VERSION_NUM 10    // Set the Decoder Version - Used by JMRI to Identify the decoder

#define CV_ACCESSORY_DECODER_OUTPUT_PULSE_TIME 2  // CV for the Output Pulse ON ms
#define CV_ACCESSORY_DECODER_SERVO_MOVE_TIME 3  // CV for the delay in ms to allow a CDU to recharge

//#define CV_ACCESSORY_DECODER_SERIAL_LSB 255       // lsb for board serial number
//#define CV_ACCESSORY_DECODER_SERIAL_MSB 256       // msb for board serial number


#define CV_PER_OUTPUT 5                           // how many CV's are used per decoder output

#define CV_USER_GROUP_1 33                        // CV  33 -  81   maximum  48 CVs
#define CV_USER_GROUP_2 112                       // CV 112 - 128   maximum  16 CVs
#define CV_USER_GROUP_3 129                       // CV 129 - 256   maximum 127 CVs
#define CV_USER_GROUP_4 513                       // CV 513 - 895   maximum 382 CVs

//#define CV_MAX_OUTPUTS (CV_USER_GROUP_4 + NUM_TURNOUTS * CVS_PER_OUTPUT)

#define CV_USER_BASE_ADDRESS CV_USER_GROUP_4                                            // base of user assigned CV's
#define CV_USER_BASE_ADDRESS_2 CV_USER_GROUP_4 + (NUM_TURNOUTS * CV_PER_OUTPUT)         // base of CV's for pwm2

#define NUM_TOTAL_CVS 4 + (NUM_TURNOUTS * CV_PER_OUTPUT) + (NUM_TURNOUTS * CV_PER_OUTPUT) + 2

#define CV_ACCESSORY_DECODER_SERIAL_LSB 255       // lsb for board serial number
#define CV_ACCESSORY_DECODER_SERIAL_MSB 256       // msb for board serial number



/*
 * servo defines
*/

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!

#define SERVO_MIN  15        // This is the 'minimum' pulse length count (out of 4096) * 10
#define SERVO_MAX  25        // This is the 'maximum' pulse length count (out of 4096) * 10
#define SERVO_FREQ 50        // Analog servos run at ~50 Hz updates
#define SERVO_CONFIG 0       // defaults to move at maximum speed

// Config is as follows: (from DCC-EX IO_PCA9685.cpp)
//  Bit 7:     0=Set PWM to 0% to power off servo motor when finished
//             1=Keep PWM pulses on (better when using PWM to drive an LED)
//  Bits 6-0:  0           Use specified duration (defaults to 0 deciseconds)
//             1 (Fast)    Move servo in 0.5 seconds
//             2 (Medium)  Move servo in 1.0 seconds
//             3 (Slow)    Move servo in 2.0 seconds
//             4 (Extra Slow)  Move servo in 5.0 seconds

#define CURRENTSTEP 1        // servo movement step size

#define SERVO_TIME 10        // the time it takes the servo to move in tenths of a second eg 10 = 1 second

#define NUM_OF_SERVOS 16     // total number of servo channels

#define NUM_OF_LEDS 16       // number of leds that can be connected

/*
 * shift register defines
*/

#define LATCH_PIN 5
#define DATA_PIN 6
#define CLOCK_PIN 4

#endif   // defines.h
