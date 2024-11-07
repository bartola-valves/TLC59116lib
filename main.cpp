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

// define LED RGB colour structure for PWM use. Each colour has 8 bits of resolution
// 0x00 is off, 0xFF is full brightness
struct RGB
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// define RGB colour for light orange (255, 165, 0)
RGB lightOrange = {255, 165, 0};
// define RGB colour for white (255, 255, 255)
RGB white = {255, 255, 255};

// define the RGB colour for dark orange (255, 140, 0)
RGB darkOrange = {255, 140, 0};

// define the RGB colour for red (255, 0, 0)
RGB red = {255, 0, 0};

// declare the HSL to RGB conversion function
void hsl_to_rgb(float h, float s, float l, uint8_t &r, uint8_t &g, uint8_t &b);

int main()
{
    stdio_init_all();

    // create a TLC59116 object controller
    TLC59116 tlc(TLC_ADDRESS, RESET_PIN); // Create a TLC59116 object with the I2C address and the RESET pin
    tlc.begin();                          // Initialize the TLC59116

    tlc.enableTLC(); // Enable the TLC59116
    // reset all the TLC59116s
    tlc.resetAllTLCs(); // Reset all the TLC59116s
    // configure the Output Gain Control Register (IREF) to VG=0.5. External resistor is 1K
    // tlc.writeToDevice(IREF, 0x05); // Set the Output Gain Control Register (IREF) to VG=0.5

    // configure RGB LEDs in PWM mode
    // tlc.LEDPWM(BLUE_1);  // Set the TLC59116 output LED0 to PWM mode for RGB-LED1
    // tlc.LEDPWM(RED_1);   // Set the TLC59116 output LED1 to PWM mode for RGB-LED1
    // tlc.LEDPWM(GREEN_1); // Set the TLC59116 output LED2 to PWM mode for RGB-LED1
    // tlc.LEDPWM(BLUE_2);  // Set the TLC59116 output LED3 to PWM mode for RGB-LED2
    // tlc.LEDPWM(RED_2);   // Set the TLC59116 output LED4 to PWM mode for RGB-LED2
    // tlc.LEDPWM(GREEN_2); // Set the TLC59116 output LED5 to PWM mode for RGB-LED2

    // set the LED red 1 to group mode so it will blink with global settings
    tlc.LEDGroup(RED_1); // Set the LED red 1 to group mode

    // configure the global PWM frequency only
    tlc.setGroupPWM(0x80); // Set the global PWM frequency to 50%

    while (1)
    {

        // wait for 5 seconds
        sleep_ms(5000);

        // set the global PWM to 20%
        tlc.setGroupPWM(0x33); // Set the global PWM duty cycle to 20%

        // wait for 5 seconds
        sleep_ms(5000);
        // turn on the LED red 2
        tlc.LEDOn(RED_2); // Turn on the LED red 2

        // wait for 5 seconds
        sleep_ms(5000);

        // turn off the red led 2
        tlc.LEDOff(RED_2); // Turn off the LED red 2

        // set the global PWM to 50%
        tlc.setGroupPWM(0x80); // Set the global PWM duty cycle to 50%
        // check if there are any errors on the TLC59116
        if (!tlc.checkErrors())
        {
            tlc.reportErrors(); // Report the errors on the TLC59116
            // wait for 1 second
            sleep_ms(1000);
        }
    }
}
