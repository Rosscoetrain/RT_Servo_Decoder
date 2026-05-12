/*
 * variables.h
 */

/*
 *  © 2023, 2026 Ross Scanlon
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



#ifndef VARIABLES_H
#define VARIABLES_H

struct CVPair
{
  uint16_t  CV;
  uint8_t   Value;
};


/*
 * to use individual CV's for each address output requires five CV's
 * 
 * 1 servo minimum position
 * 2 servo maximum position
 * 3 time for servo to move
 * 4 servo config see below
 * 5 last servo position - this is so that on startup the servo is moved to it's last position
 * 
 * 4 servo config
 * 
 * // Config is as follows: (from DCC-EX IO_PCA9685.cpp)
 * Bit 7:     0=Set PWM to 0% to power off servo motor when finished
 *             1=Keep PWM pulses on (better when using PWM to drive an LED)
 *  Bits 6-0:  0           Use specified duration (defaults to 0 deciseconds)
 *             1 (Fast)    Move servo in 0.5 seconds
 *             2 (Medium)  Move servo in 1.0 seconds
 *             3 (Slow)    Move servo in 2.0 seconds
 *             4 (Bounce)  Servo 'bounces' at extremes.  (not implemented)
 *
 */


/*
 *           address
 *            1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16
 * CV     
 *  close    33   38   43   48   53   58   63   68   73   78   83   88   93   98   103  108
 *  throw    34   39   44   49   54   59   64   69   74   79   84   89   94   99   104  109
 *  time     35   40   45   50   55   60   65   70   75   80   85   90   95   100  105  110
 *  config   36   41   46   51   56   61   66   71   76   81   86   91   96   101  106  111
 *  position 37   42   47   52   57   62   67   72   77   82   87   92   97   102  107  112
 */


/*
 * These are stored in an array of CV pairs.
 * Default array is as below
 * 
 */


/*
*/


uint8_t FactoryDefaultCVIndex = 0;

// This is the Arduino Pin Mapping to Turnout LEDs with 1 pin per turnout 
// A1 is missing in the sequence as it is used for the DCC ACK


#ifndef USE_SHIFT_REGISTER
byte outputs[] = { 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 1, 0, 13, 14, 16, 17};
//   pins         D12 D11 D10 D9 D8 D7 D6 D5 D4 D3 D1 D0 D13  A0  A2  A3
#endif

NmraDcc  Dcc ;
DCC_MSG  Packet ;


//PinPulser pinPulser;
PinPulser *pinPulser1 = NULL;
PinPulser *pinPulser2 = NULL;

uint16_t BaseTurnoutAddress;

uint8_t numberOfTurnouts1 = NUM_TURNOUTS;
uint8_t numberOfTurnouts2 = NUM_TURNOUTS;

/*
 * Rosscoe Train functions and variables
 */
// for address learning mode
#ifdef LEARNING
int LEARNINGBUTTON = A6;    // pin A6

int learningMode = LOW;
#endif

#define LEDCONTROL LED_BUILTIN

// buffer to hold serial commands
String readString;
bool ackOn = false;


static uint16_t servoMin[NUM_TURNOUTS] = {};
static uint16_t servoMax[NUM_TURNOUTS] = {};
static uint8_t  servoTime[NUM_TURNOUTS] = {};
static uint16_t  servoPosition[NUM_TURNOUTS] = {};

static uint8_t servoConfig[NUM_TURNOUTS] = {};

char* version;
uint8_t versionBuffer[3];



// called this way, it uses the default address 0x40
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Define pointers (initialize to NULL)
Adafruit_PWMServoDriver* pwm1 = NULL;
Adafruit_PWMServoDriver* pwm2 = NULL;


#endif

  
