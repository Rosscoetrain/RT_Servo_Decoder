/*
 *
 */

#ifndef CONFIG_H
#define CONFIG_H


// this is the serial number for the board

#warning "Have you set the serial number"

#define SERIAL_NUMBER 50

// Comment the line below to remove serial commands and display of messages on console
#define ENABLE_SERIAL

// Un-Comment the line below to include learning function
#define LEARNING

// Un-Comment the line below to force CVs to be written to the Factory Default values
// defined in the FactoryDefaultCVs below on Start-Up
// THIS NEEDS to be un-commented and uploaded once to setup the eeprom
// after uploading comment out the line and upload again for normal operation
// This is not normally needed.  If ENABLE_SERIAL is defined then once uploaded use
// the factory reset default <D> command to do this.
//#define FORCE_RESET_FACTORY_DEFAULT_CV

// These are only applicable to boards produced after %%%DATE%%%

// Un-Comment below for pcb at and after REV I for Pulse 16 Servo
//#define PCB_REV_D



// debug level

//#define DEBUG 6



#endif
