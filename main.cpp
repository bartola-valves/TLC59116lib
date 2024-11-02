#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "TLC59116.h" // Include the TLC59116 library

#define RESET_PIN 2 // Set the RESET pin (GPIO2)
// All TLC59116 address pins are connected to GND. The I2C address is 0x60.
#define TLC_ADDRESS 0x60 // Set the I2C address of the TLC59116

// LED0 is Blue
// LED1 is Red
// LED2 is Green

// there are 2 RGB LEDs (RGB-LED1 and RGB-LED2) connected to the TLC59116 using the following configuration
#define BLUE_1 0  // Set the TLC59116 output LED0 to Blue for RGB-LED1
#define RED_1 1   // Set the TLC59116 output LED1 to Red for RGB-LED1
#define GREEN_1 2 // Set the TLC59116 output LED2 to Green for RGB-LED1
#define BLUE_2 3  // Set the TLC59116 output LED3 to Blue for RGB-LED2
#define RED_2 4   // Set the TLC59116 output LED4 to Red for RGB-LED2
#define GREEN_2 5 // Set the TLC59116 output LED5 to Green for RGB-LED2

const uint8_t IREF = 0x1C; // IREF configuration register. Read/Write

int main()
{
    stdio_init_all();

    // create a TLC59116 object controller
    TLC59116 tlc(TLC_ADDRESS, RESET_PIN); // Create a TLC59116 object with the I2C address and the RESET pin
    tlc.begin();                          // Initialize the TLC59116
    // reset all the TLC59116s
    tlc.resetAllTLCs(); // Reset all the TLC59116s
    // configure the Output Gain Control Register (IREF) to VG=0.5. External resistor is 1K
    tlc.writeToDevice(IREF, 0x02); // Set the Output Gain Control Register (IREF) to VG=0.5
    // turn off all the LEDs first
    tlc.turnOffAllLEDs(); // Turn off all the LEDs

    // assign the RGB-LED1 to blinking group 1. Set the frequency to 1Hz and the duty cycle to 50%
    tlc.LEDGroup(BLUE_1);  // Set the TLC59116 output LED0 to group mode for RGB-LED1
    tlc.LEDGroup(RED_1);   // Set the TLC59116 output LED1 to group mode for RGB-LED1
    tlc.LEDGroup(GREEN_1); // Set the TLC59116 output LED2 to group mode for RGB-LED1

    // Set the blink frequency to 10Hz and the duty cycle to 30%
    tlc.setGroupBlink(10, 30); // Set the blink frequency to 10Hz and the duty cycle to 30%

    // assign the RGB-LED2 to PWM mode
    tlc.LEDPWM(BLUE_2);  // Set the TLC59116 output LED3 to PWM mode for RGB-LED2
    tlc.LEDPWM(RED_2);   // Set the TLC59116 output LED4 to PWM mode for RGB-LED2
    tlc.LEDPWM(GREEN_2); // Set the TLC59116 output LED5 to PWM mode for RGB-LED2

    // set the PWM value of the RGB-LED2. I want a Violet color
    tlc.setPWM(BLUE_2, 255); // Set the PWM value of the Blue LED to 255
    tlc.setPWM(RED_2, 255);  // Set the PWM value of the Red LED to 255
    tlc.setPWM(GREEN_2, 0);  // Set the PWM value of the Green LED to 0

    while (1)
    {
        // morph the colour of RGB-LED2 from Violet to Red in 2 seconds
        for (int i = 255; i >= 0; i--)
        {
            tlc.setPWM(BLUE_2, i);  // Set the PWM value of the Blue LED to i
            tlc.setPWM(RED_2, 255); // Set the PWM value of the Red LED to 255
            tlc.setPWM(GREEN_2, 0); // Set the PWM value of the Green LED to 0
            sleep_ms(8);            // wait for 8 milliseconds
        }

        // check if there are any errors on the TLC59116
        if (!tlc.checkErrors())
        {
            tlc.reportErrors(); // Report the errors on the TLC59116
            // wait for 1 second
            sleep_ms(1000);
        }
    }
}
