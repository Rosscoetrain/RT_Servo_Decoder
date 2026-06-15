/*
 * functions.h
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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "defines.h"

#include "variables.h"
#include "default_CVs.h"

/*
 * a function to read an analogue pin and return a boolean value depending on reading.
 * works the same as doing a digital read on a digital pin
 * > 512 = TRUE, <= 512 = FALSE
 */

bool dr (int pin)
 {
  int val = analogRead(pin);
  return ( val > 512 ) ;
 }

/*
 * setup the version number
 */

void setVersion()
 {
  const String versionString = VERSION;
  char versionArray[versionString.length() + 1];
  versionString.toCharArray(versionArray, versionString.length() + 1);
  version = strtok(versionArray, "."); // Split version on .
  versionBuffer[0] = atoi(version);  // Major first
  version = strtok(NULL, ".");
  versionBuffer[1] = atoi(version);  // Minor next
  version = strtok(NULL, ".");
  versionBuffer[2] = atoi(version);  // Patch last
 }


/**
 * this is just a function to show via the onboard PCB led, the state of the decoder
 */

void showAcknowledge(int nb)
 {
  for (int i=0;i<nb;i++) {
    digitalWrite(LEDCONTROL, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);               // wait for a second
    digitalWrite(LEDCONTROL, LOW);    // turn the LED off by making the voltage LOW
    delay(100);               // wait for a second
  }
}

void(* resetFunc) (void) = 0;                     // declare reset function at address 0


#ifdef ENABLE_SERIAL

/*
 * show the user CVs only
 */

void showUserCVs()
 {
  for(uint8_t i = 0; i < numberOfTurnouts1; i++)
   {
    MYSERIAL.print(F("CV"));
    MYSERIAL.print(CV_USER_BASE_ADDRESS + (i * CV_PER_OUTPUT));
    MYSERIAL.print(F(" = "));
    MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS + (i * CV_PER_OUTPUT)));
    MYSERIAL.print("    ");

    MYSERIAL.print(F("CV"));
    MYSERIAL.print(CV_USER_BASE_ADDRESS + 1 + (i * CV_PER_OUTPUT));
    MYSERIAL.print(F(" = "));
    MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS + 1 + (i * CV_PER_OUTPUT)));
    MYSERIAL.print("    ");

    MYSERIAL.print(F("CV"));
    MYSERIAL.print(CV_USER_BASE_ADDRESS + 2 + (i * CV_PER_OUTPUT));
    MYSERIAL.print(F(" = "));
    MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS + 2 + (i * CV_PER_OUTPUT)));
    MYSERIAL.print("    ");

    MYSERIAL.print(F("CV"));
    MYSERIAL.print(CV_USER_BASE_ADDRESS + 3 + (i * CV_PER_OUTPUT));
    MYSERIAL.print(F(" = "));
    MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS + 3 + (i * CV_PER_OUTPUT)));
    MYSERIAL.print("    ");

    MYSERIAL.print(F("CV"));
    MYSERIAL.print(CV_USER_BASE_ADDRESS + 4 + (i * CV_PER_OUTPUT));
    MYSERIAL.print(F(" = "));
    MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS + 4 + (i * CV_PER_OUTPUT)));
    MYSERIAL.println("    ");
   }

  if (pinPulser2)
   {
    for(uint8_t i = 0; i < numberOfTurnouts1; i++)
     {
      MYSERIAL.print(F("CV"));
      MYSERIAL.print(CV_USER_BASE_ADDRESS_2 + (i * CV_PER_OUTPUT));
      MYSERIAL.print(F(" = "));
      MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS_2 + (i * CV_PER_OUTPUT)));
      MYSERIAL.print("    ");

      MYSERIAL.print(F("CV"));
      MYSERIAL.print(CV_USER_BASE_ADDRESS_2 + 1 + (i * CV_PER_OUTPUT));
      MYSERIAL.print(F(" = "));
      MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS_2 + 1 + (i * CV_PER_OUTPUT)));
      MYSERIAL.print("    ");

      MYSERIAL.print(F("CV"));
      MYSERIAL.print(CV_USER_BASE_ADDRESS_2 + 2 + (i * CV_PER_OUTPUT));
      MYSERIAL.print(F(" = "));
      MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS_2 + 2 + (i * CV_PER_OUTPUT)));
      MYSERIAL.print("    ");

      MYSERIAL.print(F("CV"));
      MYSERIAL.print(CV_USER_BASE_ADDRESS_2 + 3 + (i * CV_PER_OUTPUT));
      MYSERIAL.print(F(" = "));
      MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS_2 + 3 + (i * CV_PER_OUTPUT)));
      MYSERIAL.print("    ");

      MYSERIAL.print(F("CV"));
      MYSERIAL.print(CV_USER_BASE_ADDRESS_2 + 4 + (i * CV_PER_OUTPUT));
      MYSERIAL.print(F(" = "));
      MYSERIAL.print(Dcc.getCV(CV_USER_BASE_ADDRESS_2 + 4 + (i * CV_PER_OUTPUT)));
      MYSERIAL.println("    ");
     }
   }
 }

#endif

/*
 * process the serial commands sent from the serial monitor
*/
#ifdef ENABLE_SERIAL


#include "StringSplitter.h"

void doSerialCommand(String readString)
 {
  readString.trim();
  readString.toUpperCase();

  MYSERIAL.println(readString);                    // so you can see the captured string

  if (readString == "<Z>")
   {

    MYSERIAL.println(F("Resetting"));

    resetFunc();
   }

#ifdef ENABLE_DCC_ACK

  if (readString == "<H>")                       // set DCC_ACK on/off
   {

    digitalWrite(ENABLE_DCC_ACK, !ackOn);
    ackOn = !ackOn;

    if (ackOn)
     {
      MYSERIAL.println("ackOn");
     }
    else
     {
      MYSERIAL.println("ackOff");
     }

   }
#endif

  if (readString == "<?>")
   {
    MYSERIAL.println(F("Help Text"));

    MYSERIAL.println(F("Close a turnout: <C address>"));
    MYSERIAL.println(F("Throw a turnout: <T address>"));

    MYSERIAL.println(F("Set decoder base address: <A address>"));

    MYSERIAL.println(F("Set decoder output closed position: <M output  mS / 10>"));
    MYSERIAL.println(F("Set decoder output thrown position: <N output mS / 10>"));
    MYSERIAL.println(F("Set decoder output move time: <O output S / 10>"));
    MYSERIAL.println(F("Set decoder output configuration: <P output [0:1:2:3]>"));

    MYSERIAL.println(F("Where output is 0 - 15 as on the decoder pcb"));

    MYSERIAL.println(F("Show current CVs: <>"));

    MYSERIAL.println(F("Soft Reset: <Z>"));

   }
  else
   {
    if (readString.startsWith("<>"))
     {
      MYSERIAL.println(F("CVs are:"));

      MYSERIAL.print(F("CV"));
      MYSERIAL.print(CV_ACCESSORY_DECODER_ADDRESS_LSB);
      MYSERIAL.print(F(" = "));
      MYSERIAL.println(Dcc.getCV(CV_ACCESSORY_DECODER_ADDRESS_LSB));
      MYSERIAL.print(F("CV"));
      MYSERIAL.print(CV_ACCESSORY_DECODER_ADDRESS_MSB);
      MYSERIAL.print(F(" = "));
      MYSERIAL.println(Dcc.getCV(CV_ACCESSORY_DECODER_ADDRESS_MSB));

      showUserCVs();
     }
    else
     {
      if (readString.startsWith("<"))
       {
// this is where commands are completed

// show only the user CVs  also to be used for RT_Uploader_Configure
        if (readString.startsWith("<U>"))
         {
          showUserCVs();
         }

// command to close turnout <C address>

        if (readString.startsWith("<C"))
         {
          StringSplitter *splitter = new StringSplitter(readString, ' ', 3);  // new StringSplitter(string_to_split, delimiter, limit)
          int itemCount = splitter->getItemCount();


          if ( itemCount == 2)
           {
            int addr = splitter->getItemAtIndex(1).toInt();
            notifyDccAccTurnoutOutput( addr, 0, 1 );
           }
          else
           {
            MYSERIAL.println(F("Invalid command: should be <C address>"));
           }
          delete splitter;
          splitter = NULL;
         }

// command to throw turnout <T address>

        if (readString.startsWith("<T"))
         {
          StringSplitter *splitter = new StringSplitter(readString, ' ', 3);  // new StringSplitter(string_to_split, delimiter, limit)
          int itemCount = splitter->getItemCount();

          if ( itemCount == 2)
           {
            int addr = splitter->getItemAtIndex(1).toInt();
            notifyDccAccTurnoutOutput( addr, 1, 1 );
           }
          else
           {
            MYSERIAL.println(F("Invalid command: should be <T address>"));
           }
          delete splitter;
          splitter = NULL;
         }


        if (readString.startsWith("<Y>"))
         {
          for (int a = 1; a <= NUM_TURNOUTS; a++)
           {
            notifyDccAccTurnoutOutput( a, 1, 1 );
            MYSERIAL.print(a);
            MYSERIAL.println(" throw");
            delay(10000);
            notifyDccAccTurnoutOutput( a, 0, 1 );
            MYSERIAL.print(a);
            MYSERIAL.println(" close");
            delay(10000);
           }
         }





// command to set address <A address>
// address will be adjusted to the correct base turnout address
// eg if address is 2 this will be corrected to 1 as the address are groups of 8 with an offset of 4
// ie 1..8, 5..12, ...

        if (readString.startsWith("<A"))
         {
          StringSplitter *splitter = new StringSplitter(readString, ' ', 3);  // new StringSplitter(string_to_split, delimiter, limit)
          int itemCount = splitter->getItemCount();

          if ( itemCount == 2)
           {
            int addr = splitter->getItemAtIndex(1).toInt();

            byte L = (addr + 3) / 4;
            byte H = (addr + 3) / 1024;

#ifdef DEBUG_MSG
            MYSERIAL.print(F("Value = ")); MYSERIAL.println(addr);
            MYSERIAL.print(F(" H = ")); MYSERIAL.println(H);
            MYSERIAL.print(F(" L = ")); MYSERIAL.println(L);
#endif

            Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, H);
            Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, L);
           }
          else
           {
            MYSERIAL.println(F("Invalid command: should be <A address>"));
           }
          delete splitter;
          splitter = NULL;
         }

/*
 * command to reset all CVs to factory default
 *
 *
 */

      if (readString == "<D>")
       {
        MYSERIAL.println(F("Reset factory default CVs"));
        notifyCVResetFactoryDefault();
       }


/*
 * command to set output closed value.
 * The value here is the number of milliseconds / 10
 * ie 100ms/10 = 10.
 *
 */

        if (readString.startsWith("<M"))
         {
          StringSplitter *splitter = new StringSplitter(readString, ' ', 3);  // new StringSplitter(string_to_split, delimiter, limit)
          int itemCount = splitter->getItemCount();


          if ( itemCount == 3)
           {
            int addr = splitter->getItemAtIndex(1).toInt();
            int value = splitter->getItemAtIndex(2).toInt();

#ifdef DEBUG_MSG
            MYSERIAL.print(F("Adress = ")); MYSERIAL.println(addr);
            MYSERIAL.print(F("Value = ")); MYSERIAL.println(value);
#endif
//            if ( addr >= 0 && addr <= 15 )
            if ( addr >= 0 && addr <= 31 )
             {
              Dcc.setCV(CV_USER_BASE_ADDRESS + (addr) * CV_PER_OUTPUT, value);
             }
            else
             {
              MYSERIAL.println(F("Invalid output: should be 0 to 15"));
             }
           }
          else
           {
            MYSERIAL.println(F("Invalid command: should be <M output ms/10>"));
           }
          delete splitter;
          splitter = NULL;
         }

/*
 * command to set output thrown value.
 * The value here is the number of milliseconds / 10
 * ie 100ms/10 = 10.
 *
 */

        if (readString.startsWith("<N"))
         {
          StringSplitter *splitter = new StringSplitter(readString, ' ', 3);  // new StringSplitter(string_to_split, delimiter, limit)
          int itemCount = splitter->getItemCount();


          if ( itemCount == 3)
           {
            int addr = splitter->getItemAtIndex(1).toInt();
            int value = splitter->getItemAtIndex(2).toInt();

#ifdef DEBUG_MSG
            MYSERIAL.print(F("Adress = ")); MYSERIAL.println(addr);
            MYSERIAL.print(F("Value = ")); MYSERIAL.println(value);
#endif
//            if ( addr >= 0 && addr <= 15 )
            if ( addr >= 0 && addr <= 31 )
             {
              Dcc.setCV(CV_USER_BASE_ADDRESS + 1 + (addr) * CV_PER_OUTPUT, value);
             }
            else
             {
              MYSERIAL.println(F("Invalid output: should be 0 to 15"));
             }
           }
          else
           {
            MYSERIAL.println(F("Invalid command: should be <N output ms/10>"));
           }
          delete splitter;
          splitter = NULL;
         }

/*
 * command to set output servo move time
 * The value here is the number of seconds / 10
 * ie 100/10 = 10.
 *
 */

        if (readString.startsWith("<O"))
         {
          StringSplitter *splitter = new StringSplitter(readString, ' ', 3);  // new StringSplitter(string_to_split, delimiter, limit)
          int itemCount = splitter->getItemCount();


          if ( itemCount == 3)
           {
            int addr = splitter->getItemAtIndex(1).toInt();
            int value = splitter->getItemAtIndex(2).toInt();

#ifdef DEBUG_MSG
            MYSERIAL.print(F("Adress = ")); MYSERIAL.println(addr);
            MYSERIAL.print(F("Value = ")); MYSERIAL.println(value);
#endif
//            if ( addr >= 0 && addr <= 15 )
            if ( addr >= 0 && addr <= 31 )
             {
              Dcc.setCV(CV_USER_BASE_ADDRESS + 2 + (addr) * CV_PER_OUTPUT, value);
             }
            else
             {
              MYSERIAL.println(F("Invalid output: should be 0 to 15"));
             }
           }
          else
           {
            MYSERIAL.println(F("Invalid command: should be <O output S/10>"));
           }
          delete splitter;
          splitter = NULL;
         }


/*
 * command to set output servo configuration
 *
 * the value here is one of:
 * 0 = default move servo at maximum speed
 * 1 = fast move 0.5 second
 * 2 = medium move 1.0 second
 * 3 = slow move 2.0 second
 * 4 = extra slow move 5.0 second
 *
 */

        if (readString.startsWith("<P"))
         {
          StringSplitter *splitter = new StringSplitter(readString, ' ', 3);  // new StringSplitter(string_to_split, delimiter, limit)
          int itemCount = splitter->getItemCount();


          if ( itemCount == 3)
           {
            int addr = splitter->getItemAtIndex(1).toInt();
            int value = splitter->getItemAtIndex(2).toInt();

#ifdef DEBUG_MSG
            MYSERIAL.print(F("Adress = ")); MYSERIAL.println(addr);
            MYSERIAL.print(F("Value = ")); MYSERIAL.println(value);
#endif
//            if ( addr >= 0 && addr <= 15 )
            if ( addr >= 0 && addr <= 31 )
             {

              Dcc.setCV(CV_USER_BASE_ADDRESS + 3 + (addr * CV_PER_OUTPUT), value);

             }
            else
             {
              MYSERIAL.println(F("Invalid output: should be 0 to 31"));
             }
           }
          else
           {
            MYSERIAL.println(F("Invalid command: should be <P output [0:1:2:4]>"));
           }
          delete splitter;
          splitter = NULL;
         }

        if (readString.startsWith("<X"))
         {
          MYSERIAL.println(F("Serial number is:"));

          MYSERIAL.println(Dcc.getCV(CV_ACCESSORY_DECODER_SERIAL_LSB) + (Dcc.getCV(CV_ACCESSORY_DECODER_SERIAL_MSB) * 256 ));

          MYSERIAL.println("");

         }
       }
      else
       {
        MYSERIAL.println(F("ERROR: Unknown command"));
       }
     }
   }
 }

#endif

/*
 * initialize the decoder
*/

void initPinPulser(void)
 {
  BaseTurnoutAddress = (((Dcc.getCV(CV_ACCESSORY_DECODER_ADDRESS_MSB) * 256) + Dcc.getCV(CV_ACCESSORY_DECODER_ADDRESS_LSB) - 1) * 4) + 1  ;

// read the CV's for each address
  if (pinPulser1)
   {
    for(uint8_t i = 0; i < numberOfTurnouts1; i++)
     {
      servoMin[i] = Dcc.getCV( CV_USER_BASE_ADDRESS + ( i * CV_PER_OUTPUT ) ) * 10;
      servoMax[i] = Dcc.getCV( CV_USER_BASE_ADDRESS + 1 + ( i * CV_PER_OUTPUT ) ) * 10;
      servoTime[i] = Dcc.getCV( CV_USER_BASE_ADDRESS + 2 + ( i * CV_PER_OUTPUT ) );
      servoConfig[i]  = Dcc.getCV( CV_USER_BASE_ADDRESS + 3 + ( i * CV_PER_OUTPUT ) );
      servoPosition[i]  = Dcc.getCV( CV_USER_BASE_ADDRESS + 4 + ( i * CV_PER_OUTPUT ) ) * 10;

#if DEBUG == 6
      MYSERIAL.print(F("ipp : "));
      MYSERIAL.print(F(" i : "));MYSERIAL.print(i);
      MYSERIAL.print(F(" servoMin : "));MYSERIAL.print(servoMin[i]);
      MYSERIAL.print(F(" servoMax : "));MYSERIAL.print(servoMax[i]);
      MYSERIAL.print(F(" servoConfig : "));MYSERIAL.println(servoConfig[i]);
#endif
     }
    pinPulser1->init(servoMin, servoMax, servoTime, servoConfig, servoPosition, numberOfTurnouts1, pwm1, 1);
    pinPulser1->printArrays();
   }

  if (pinPulser2)
   {
    for(uint8_t i = 0; i < numberOfTurnouts2; i++)
     {
      servoMin2[i] = Dcc.getCV( CV_USER_BASE_ADDRESS_2 + ( i * CV_PER_OUTPUT ) ) * 10;
      servoMax2[i] = Dcc.getCV( CV_USER_BASE_ADDRESS_2 + 1 + ( i * CV_PER_OUTPUT ) ) * 10;
      servoTime2[i] = Dcc.getCV( CV_USER_BASE_ADDRESS_2 + 2 + ( i * CV_PER_OUTPUT ) );
      servoConfig2[i]  = Dcc.getCV( CV_USER_BASE_ADDRESS_2 + 3 + ( i * CV_PER_OUTPUT ) );
      servoPosition2[i]  = Dcc.getCV( CV_USER_BASE_ADDRESS_2 + 4 + ( i * CV_PER_OUTPUT ) ) * 10;

#ifdef DEBUG_MSG
      MYSERIAL.print(F("ipp : "));
      MYSERIAL.print(F(" i : "));MYSERIAL.print(i);
      MYSERIAL.print(F(" servoMin : "));MYSERIAL.print(servoMin2[i]);
      MYSERIAL.print(F(" servoMax : "));MYSERIAL.print(servoMax2[i]);
      MYSERIAL.print(F(" servoConfig : "));MYSERIAL.println(servoConfig2[i]);
#endif

     }
    pinPulser2->init(servoMin2, servoMax2, servoTime2, servoConfig2, servoPosition2, numberOfTurnouts2, pwm2, 2);
    pinPulser2->printArrays();
   }

  MYSERIAL.print(F(" DCC Turnout Base Address: ")); MYSERIAL.println(BaseTurnoutAddress, DEC);

 }


/*
void applyFactoryDefaults() {
    for (size_t i = 0; i < NUM_TOTAL_CVS; ++i) {
        // Fetch the pointer to the struct item sitting inside the Flash memory
        const CVPair* flashPtr = &FactoryDefaultCVs[i];

        // Read the struct fields using pgm_read_word
        uint16_t currentCV    = pgm_read_word(&(flashPtr->cv));
        uint16_t currentValue = pgm_read_word(&(flashPtr->value));

        // Example: Write them to your library or EEPROM
        // Dcc.setCV(currentCV, currentValue);
    }
}
*/

uint16_t applyFactoryDefaults() {
    for (size_t i = 0; i < NUM_TOTAL_CVS; ++i) {
        // Point directly to the address of the element in flash memory
        const CVPair* flashPtr = &FactoryDefaultCVs[i];

        // Use pgm_read_word to copy the 16-bit values safely into local RAM variables
        uint16_t currentCV    = pgm_read_word(&(flashPtr->CV));
        uint8_t currentValue = pgm_read_word(&(flashPtr->Value));

        // Execute your logic (e.g., writing defaults back to EEPROM or your DCC Library)
        Dcc.setCV(currentCV, currentValue);
    }
  return 0;
}


#endif  // FUNCTIONS_H


