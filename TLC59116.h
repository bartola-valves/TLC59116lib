#ifndef TLC59116_H
#define TLC59116_H

#pragma once
#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"

/**
 *  @brief  TLC59116 class
 * 			Library for the TLC59116 I2C LED driver
 * 			https://www.ti.com/lit/ds/symlink/tlc59116.pdf
 *
 *
 * @param  i2c_port: I2C port number
 * @param  sda_pin: SDA pin number
 * @param  scl_pin: SCL pin number
 *
 *
 */
class TLC59116
{
public:
	TLC59116(uint8_t address, uint8_t resetPin); // Constructor. Set the I2C address and the RESET pin

	void begin();								 // Initialize the I2C bus to interface with the TLC59116
	void setPWM(uint8_t channel, uint8_t value); // Set the PWM value of a single channel
	// void setAllPWM(uint8_t value);					 // Set the PWM value of all channels
	// void setChannel(uint8_t channel, uint8_t value); // Set the output value of a single channel
	// void setAllChannels(uint8_t value);				 // Set the output value of all channels
	// void update();									 // Update the output values of the channels

	void LEDOn(uint8_t LED);								  // Turn on a single LED
	void LEDOff(uint8_t LED);								  // Turn off a single LED
	void LEDPWM(uint8_t LED);								  // Set a single LED to PWM mode
	void LEDGroup(uint8_t LED);								  // Set a single LED to group mode
	void setGroupBlink(uint8_t freq, uint8_t duty);			  // Set the blink frequency and duty cycle of all group mode LEDs
	void clearErrors();										  // Clear the error flags of the TLC59116
	void reportErrors();									  // Report the error flags of the TLC59116 via the serial port
	bool checkErrors();										  // Check for errors in the TLC59116. Use the reportErrors() function to print them
	void resetDriver();										  // Reset the TLC59116. Also enables it at the end
	void turnOffAllLEDs();									  // Turn off all LEDs
	void resetAllTLCs();									  // Reset all TLC59116s
	void enableTLC();										  // Enable the TLC59116. First thing to do.
	void setPWM(uint8_t pin, uint8_t duty);					  // Set the PWM value of a single LED
	void setGroupPWM(uint8_t duty);							  // Set the PWM value of all group mode LEDs
	void writeToDevice(uint8_t reg, uint8_t newLEDOUT);		  // Write a value to a register of the TLC59116
	uint8_t readFromDevice(uint8_t reg);					  // Read a value from a register of the TLC59116
	uint8_t modifyLEDOutputState(uint8_t LED, uint8_t state); // Modify the output state of a single LED
	// destructors
	~TLC59116(); // Destructor
private:
	uint8_t i2c_port; // I2C port number
	uint8_t sda_pin;  // SDA pin number
	uint8_t scl_pin;  // SCL pin number

	// void writeRegister(uint8_t reg, uint8_t value); // Write a value to a register of the TLC59116.
	// uint8_t readRegister(uint8_t reg);				// Read a value from a register of the TLC59116.

	uint8_t groupMode; // Group mode flag
	uint8_t LEDOUT[4]; // LED output drive state registers
	uint16_t ERR_FLAG; // Error flag
	uint8_t TLC_ADDR;  // I2C address of the TLC59116
	uint8_t RESET;	   // RESET pin
};

#endif // TLC59116_H
