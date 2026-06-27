/*
 * @file functions_dcc.h
 *
 *
 *  DCC functions
*/

#ifndef FUNCTIONS_DCC_H
#define FUNCTIONS_DCC_H

#include <EEPROM.h>
#include "defines.h"


// This function is called whenever a normal DCC Turnout Packet is received

void notifyDccAccTurnoutOutput( uint16_t Addr, uint8_t Direction, uint8_t OutputPower )
 {
#ifdef  NOTIFY_TURNOUT_MSG
  MYSERIAL.print("notifyDccAccTurnoutOutput: Turnout: ") ;
  MYSERIAL.print(Addr,DEC) ;
  MYSERIAL.print(" Direction: ");
  MYSERIAL.print(Direction ? "Thrown" : "Closed") ;
  MYSERIAL.print(" Output: ");
  MYSERIAL.println(OutputPower ? "On" : "Off") ;
#endif

// check to see if in learning mode and update address

#ifdef LEARNING
  if (learningMode == HIGH) {

//    int H = (Addr - 1) / 64;
//    int L = Addr - (H * 64);
    byte L = (Addr + 3) / 4;
    byte H = (Addr + 3) / 1024;

#ifdef DEBUG_MSG
    MYSERIAL.println("");
    MYSERIAL.print(F("Value = ")); MYSERIAL.println(Addr,DEC);
    MYSERIAL.print(F(" H = ")); MYSERIAL.println(H,DEC);
    MYSERIAL.print(F(" L = ")); MYSERIAL.println(L,DEC);
#endif

    Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, H);
    Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, L);

   }
  else
#endif

   {
    if(( Addr >= BaseTurnoutAddress ) && ( Addr < (BaseTurnoutAddress + numberOfTurnouts1)) && OutputPower )
     {
      uint16_t pinIndex = ( (Addr - BaseTurnoutAddress) << 1 ) + Direction ;
/*
 *   To get close or throw for addPin
 *   pinIndex * 2 + Direction
 *   to decode in PinPulser direction = pinIndex % 2, servo = int(pinIndex / 2)
 *
*/
      pinPulser1->addPin(pinIndex);

#ifdef  NOTIFY_TURNOUT_MSG
      MYSERIAL.print(" Pin Index: ");
      MYSERIAL.print(pinIndex,DEC);
//      MYSERIAL.print(" Pin: ");
//      MYSERIAL.println(outputs[pinIndex / 2],DEC);
#endif
     }

    if (pinPulser2)
     {
      if(( Addr >= BaseTurnoutAddress + numberOfTurnouts1 ) && ( Addr < (BaseTurnoutAddress + numberOfTurnouts1 + numberOfTurnouts2)) && OutputPower )
       {
        uint16_t pinIndex = ( (Addr - BaseTurnoutAddress - numberOfTurnouts1) << 1 ) + Direction ;
/*
 *   To get close or throw for addPin
 *   pinIndex * 2 + Direction
 *   to decode in PinPulser direction = pinIndex % 2, servo = int(pinIndex / 2)
 *
*/
       pinPulser2->addPin(pinIndex);

#ifdef  NOTIFY_TURNOUT_MSG
      MYSERIAL.print(" Pin Index: ");
      MYSERIAL.print(pinIndex,DEC);
//      MYSERIAL.print(" Pin: ");
//      MYSERIAL.println(outputs[pinIndex / 2],DEC);
#endif
      }

     }

   }
#ifdef  NOTIFY_TURNOUT_MSG
  MYSERIAL.println();
#endif
 }

/*
void notifyCVChange(uint16_t CV, uint8_t Value)
 {
#ifdef DEBUG_MSG
  MYSERIAL.print("notifyCVChange: CV: ") ;
  MYSERIAL.print(CV,DEC) ;
  MYSERIAL.print(" Value: ") ;
  MYSERIAL.println(Value, DEC) ;
#endif

  Value = Value;  // Silence Compiler Warnings...

  if( (CV == CV_ACCESSORY_DECODER_ADDRESS_MSB) || (CV == CV_ACCESSORY_DECODER_ADDRESS_LSB) ||
		  (CV == CV_ACCESSORY_DECODER_SERVO_MOVE_TIME)  )// ||
//      ( ( CV - 33 ) % 5 == 0 ) ||
//      ( ( CV - 34 ) % 5 == 0 ) ||
//      ( ( CV - 35 ) % 5 == 0 ) ||
//      ( ( CV - 36 ) % 5 == 0 ) )
   {
		initPinPulser();	                                 // Some CV we care about changed so re-init the PinPulser with the new CV settings
   }
 }
*/


void notifyCVResetFactoryDefault()
{
  // Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset
  // to flag to the loop() function that a reset to Factory Defaults needs to be done
//  FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs)/sizeof(CVPair);
  setAddress = true;
  FactoryDefaultCVIndex = NUM_TOTAL_CVS;

#ifdef ENABLE_SERIAL
  MYSERIAL.println("Resetting Factory Default");
#endif

};

// This function is called by the NmraDcc library when a DCC ACK needs to be sent
// Calling this function should cause an increased 60ma current drain on the power supply for 6ms to ACK a CV Read

#ifdef  ENABLE_DCC_ACK
void notifyCVAck(void)
 {
#ifdef DEBUG_MSG
  MYSERIAL.println("notifyCVAck") ;
#endif
  // Configure the DCC CV Programing ACK pin for an output
  pinMode( ENABLE_DCC_ACK, OUTPUT );

  // Generate the DCC ACK 60mA pulse
  digitalWrite( ENABLE_DCC_ACK, HIGH );
  delay( 10 );  // The DCC Spec says 6ms but 10 makes sure... ;)
  digitalWrite( ENABLE_DCC_ACK, LOW );
 }
#endif


void notifyDccMsg(DCC_MSG *Msg)
 {
// ignore idle messages and service mode reset

  if (Msg->Data[0] == 0xFF || Msg->Data[0] == 0x7F || Msg->Data[0] == 0x00)
   {
    return;
   }

  #ifdef NOTIFY_DCC_MSG
    MYSERIAL.print("notifyDccMsg: ") ;
    for(uint8_t i = 0; i < Msg->Size; i++)
     {
      MYSERIAL.print(Msg->Data[i], HEX);
      MYSERIAL.write(' ');
     }
    MYSERIAL.println();
  #endif

/*
// 1. Service Mode CV Write (Pattern 0x70)
  if ((Msg->Data[0] & 0xF0) == 0x70)
   {
    uint16_t cv = (((Msg->Data[0] & 0x03) << 8) | Msg->Data[1]) + 1;
    uint8_t val = Msg->Data[2];
    Dcc.setCV(cv, val);
    BaseTurnoutAddress = Dcc.getAddr();
   }

// 2. POM (Programming on Main) (Pattern 0xE0)
  if (Msg->Size >= 4)
   {
    if (Msg->Data[0] == BaseTurnoutAddress)
     {
      if ((Msg->Data[1] & 0xF0) == 0xE0)
       {
        uint16_t cv = (((Msg->Data[1] & 0x03) << 8) | Msg->Data[2]) + 1;
        uint8_t val = Msg->Data[3];
#ifdef NOTIFY_DCC_MSG
        MYSERIAL.print("CV : ");
        MYSERIAL.print(cv);
        MYSERIAL.print(" value : ");
        MYSERIAL.println(val);
#endif
        if ((cv == 8) && (val == 8))
         {
#ifdef ENABLE_SERIAL
          MYSERIAL.println(F("Reset factory default CVs"));
#endif
          notifyCVResetFactoryDefault();
          return;
         }
        Dcc.setCV(cv, val);
        BaseTurnoutAddress = Dcc.getAddr();
       }
     }
   }
*/
 }



uint8_t notifyCVWrite (uint16_t CV, uint8_t Value)
 {

#if DEBUG == 6
  MYSERIAL.print("notifyCVWrite 1 cv : ");
  MYSERIAL.print(CV);
  MYSERIAL.print(" value : ");
  MYSERIAL.println(Value);
  MYSERIAL.print(" setAddress : ");
  MYSERIAL.println(setAddress);
#endif

  if ((CV == 8) && (Value == 8))  // factory reset return before writing to CV
   {
    notifyCVResetFactoryDefault();
    return Value;
   }

  if (CV == 8)                    // ignore any attempts to write to CV8
   {
    return Value;
   }

  if (((CV == 1) || (CV == 9)) && (!setAddress))     // ignore writes to CV1 and CV9 causes problems with actual address
   {
#if DEBUG == 6
    MYSERIAL.print(" not setAddress : ");
    MYSERIAL.println(setAddress);
#endif
    return Value;
   }


  if ((CV == 1) && (setAddress))
   {

#if DEBUG == 6
    MYSERIAL.print("notifyCVWrite setAddress LSB cv : ");
    MYSERIAL.print(CV);
    MYSERIAL.print(" value : ");
    MYSERIAL.println(Value);
#endif

    EEPROM.update(CV, Value);
    return Value;
   }

  if ((CV == 9) && (setAddress))
   {

#if DEBUG == 6
    MYSERIAL.print("notifyCVWrite setAddress MSB cv : ");
    MYSERIAL.print(CV);
    MYSERIAL.print(" value : ");
    MYSERIAL.println(Value);
#endif

    EEPROM.update(CV, Value);
    return Value;
   }

#if DEBUG == 6
    MYSERIAL.println("past MSB ");
#endif


  if ((CV >= CV_USER_GROUP_4) && (CV <= CV_USER_GROUP_4 + NUM_TOTAL_CVS))
   {
    if (((CV - CV_USER_GROUP_4) % 5) == 4)   // ignore writes to store current servo position
     {
      return Value;
     }

#if DEBUG == 6
  MYSERIAL.print(" value : ");
  MYSERIAL.println(Value);
#endif
    if (((CV - CV_USER_GROUP_4) % 5) == 3)
     {
      if (Value > 5)              // ignore values that are greater than 3 only accepts 0, 1, 2, 3, 4, 5
       {
        return Value;
       }
     }

    if (Dcc.getCV(CV) != Value)
     {

#if DEBUG == 6
  MYSERIAL.print(" value : ");
  MYSERIAL.println(Value);
#endif

      EEPROM.update(CV, Value);
      initPinPulser();
     }
   }
  return Value;
 }

#endif  // FUNCTIONS_DCC_H
