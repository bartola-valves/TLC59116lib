#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "TLC59116.h" // Include the TLC59116 library
#include <cmath>      // for fabs and fmod

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

// define the HSL colour structure
struct HSL
{
    float hue;
    float saturation;
    float lightness;
};

// define the HSL colour for light orange
HSL lightOrangeHSL = {30.0f, 1.0f, 0.7f};
// define the HSL colour for dark orange
HSL darkOrangeHSL = {30.0f, 1.0f, 0.5f};

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
    tlc.writeToDevice(IREF, 0x05); // Set the Output Gain Control Register (IREF) to VG=0.5

    // configure RGB LEDs in PWM mode
    tlc.LEDPWM(BLUE_1);  // Set the TLC59116 output LED0 to PWM mode for RGB-LED1
    tlc.LEDPWM(RED_1);   // Set the TLC59116 output LED1 to PWM mode for RGB-LED1
    tlc.LEDPWM(GREEN_1); // Set the TLC59116 output LED2 to PWM mode for RGB-LED1
    tlc.LEDPWM(BLUE_2);  // Set the TLC59116 output LED3 to PWM mode for RGB-LED2
    tlc.LEDPWM(RED_2);   // Set the TLC59116 output LED4 to PWM mode for RGB-LED2
    tlc.LEDPWM(GREEN_2); // Set the TLC59116 output LED5 to PWM mode for RGB-LED2

    // configure the RGB-LED-1 to light orange using the HSL to RGB conversion function
    uint8_t r, g, b;
    hsl_to_rgb(lightOrangeHSL.hue, lightOrangeHSL.saturation, lightOrangeHSL.lightness, r, g, b);
    tlc.setPWM(BLUE_1, b);  // Set the PWM value of the TLC59116 output LED0 to Blue for RGB-LED1
    tlc.setPWM(RED_1, r);   // Set the PWM value of the TLC59116 output LED1 to Red for RGB-LED1
    tlc.setPWM(GREEN_1, g); // Set the PWM value of the TLC59116 output LED2 to Green for RGB-LED1

    // configure the RGB-LED-2 to dark orange using the HSL to RGB conversion function
    hsl_to_rgb(darkOrangeHSL.hue, darkOrangeHSL.saturation, darkOrangeHSL.lightness, r, g, b);
    tlc.setPWM(BLUE_2, b);  // Set the PWM value of the TLC59116 output LED3 to Blue for RGB-LED2
    tlc.setPWM(RED_2, r);   // Set the PWM value of the TLC59116 output LED4 to Red for RGB-LED2
    tlc.setPWM(GREEN_2, g); // Set the PWM value of the TLC59116 output LED5 to Green for RGB-LED2

    while (1)
    {

        // wait for 1 second
        sleep_ms(1000);

        // check if there are any errors on the TLC59116
        if (!tlc.checkErrors())
        {
            tlc.reportErrors(); // Report the errors on the TLC59116
            // wait for 1 second
            sleep_ms(1000);
        }
    }
}

// Define the HSL to RGB conversion function
void hsl_to_rgb(float h, float s, float l, uint8_t &r, uint8_t &g, uint8_t &b)
{
    float c = (1.0f - fabs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;
    float r1, g1, b1;

    if (h >= 0.0f && h < 60.0f)
    {
        r1 = c;
        g1 = x;
        b1 = 0.0f;
    }
    else if (h >= 60.0f && h < 120.0f)
    {
        r1 = x;
        g1 = c;
        b1 = 0.0f;
    }
    else if (h >= 120.0f && h < 180.0f)
    {
        r1 = 0.0f;
        g1 = c;
        b1 = x;
    }
    else if (h >= 180.0f && h < 240.0f)
    {
        r1 = 0.0f;
        g1 = x;
        b1 = c;
    }
    else if (h >= 240.0f && h < 300.0f)
    {
        r1 = x;
        g1 = 0.0f;
        b1 = c;
    }
    else
    {
        r1 = c;
        g1 = 0.0f;
        b1 = x;
    }

    r = static_cast<uint8_t>((r1 + m) * 255.0f);
    g = static_cast<uint8_t>((g1 + m) * 255.0f);
    b = static_cast<uint8_t>((b1 + m) * 255.0f);

    // Print intermediate values for debugging
    printf("HSL: (%f, %f, %f)\n", h, s, l);
    printf("Intermediate RGB: (%f, %f, %f)\n", r1, g1, b1);
    printf("RGB: (%d, %d, %d)\n", r, g, b);
}