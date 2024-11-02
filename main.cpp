#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "TLC59116.h" // Include the TLC59116 library

#define RESET_PIN 2      // Set the RESET pin (GPIO2)
#define TLC_ADDRESS 0x60 // Set the I2C address of the TLC59116

int main()
{
    stdio_init_all();

    // create a TLC59116 object controller
    TLC59116 tlc(TLC_ADDRESS, RESET_PIN); // Create a TLC59116 object with the I2C address and the RESET pin
    tlc.begin();                          // Initialize the TLC59116
    tlc.LEDOn(0);                         // Turn on LED 0

    while (1)
    {
        sleep_ms(1000); // Delay for 1 second
        tlc.LEDOff(0);  // Turn off LED 0
        sleep_ms(1000); // Delay for 1 second
        tlc.LEDOn(0);   // Turn on LED 0
    }
}
