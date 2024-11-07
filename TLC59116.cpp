#include "TLC59116.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <cstdio> // for printf

/**
 * @brief  TLC59116 class
 *       Library for the TLC59116 I2C LED driver
 *      https://www.ti.com/lit/ds/symlink/tlc59116.pdf
 *
 */

/**
 * Defines for the Class
 *
 */

// I2C defines
// This example will use I2C0 on GPIO0 (SDA) and GPIO1 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0 // I2C port number
#define I2C_SDA 0     // SDA pin number
#define I2C_SCL 1     // SCL pin number

// Register Addresses

const uint8_t MODE1 = 0x00;      // Mode register 1. Read/Write
const uint8_t MODE2 = 0x01;      // Mode register 2. Read/Write
const uint8_t PWM0 = 0x02;       // PWM0 register - brightness for LED0. Read/Write
const uint8_t PWM1 = 0x03;       // PWM1 register - brightness for LED1. Read/Write
const uint8_t PWM2 = 0x04;       // PWM2 register - brightness for LED2. Read/Write
const uint8_t PWM3 = 0x05;       // PWM3 register - brightness for LED3. Read/Write
const uint8_t PWM4 = 0x06;       // PWM4 register - brightness for LED4. Read/Write
const uint8_t PWM5 = 0x07;       // PWM5 register - brightness for LED5. Read/Write
const uint8_t PWM6 = 0x08;       // PWM6 register - brightness for LED6. Read/Write
const uint8_t PWM7 = 0x09;       // PWM7 register - brightness for LED7. Read/Write
const uint8_t PWM8 = 0x0A;       // PWM8 register - brightness for LED8. Read/Write
const uint8_t PWM9 = 0x0B;       // PWM9 register - brightness for LED9. Read/Write
const uint8_t PWM10 = 0x0C;      // PWM10 register - brightness for LED10. Read/Write
const uint8_t PWM11 = 0x0D;      // PWM11 register - brightness for LED11. Read/Write
const uint8_t PWM12 = 0x0E;      // PWM12 register - brightness for LED12. Read/Write
const uint8_t PWM13 = 0x0F;      // PWM13 register - brightness for LED13. Read/Write
const uint8_t PWM14 = 0x10;      // PWM14 register - brightness for LED14. Read/Write
const uint8_t PWM15 = 0x11;      // PWM15 register - brightness for LED15. Read/Write
const uint8_t GRPPWM = 0x12;     // Group PWM brightness -duty cycle. Read/Write
const uint8_t GRPFREQ = 0x13;    // Group frequency. Read/Write
const uint8_t LEDOUT0 = 0x14;    // LED output state register 0. Read/Write
const uint8_t LEDOUT1 = 0x15;    // LED output state register 1. Read/Write
const uint8_t LEDOUT2 = 0x16;    // LED output state register 2. Read/Write
const uint8_t LEDOUT3 = 0x17;    // LED output state register 3. Read/Write
const uint8_t SUBADR1 = 0x18;    // I2C-bus subaddress 1. Read/Write
const uint8_t SUBADR2 = 0x19;    // I2C-bus subaddress 2. Read/Write
const uint8_t SUBADR3 = 0x1A;    // I2C-bus subaddress 3. Read/Write
const uint8_t ALLCALLADR = 0x1B; // LED All Call I2C-bus address. Read/Write
const uint8_t IREF = 0x1C;       // IREF configuration register. Read/Write
const uint8_t ERRFLAG1 = 0x1D;   // Error flag 1. Read only
const uint8_t ERRFLAG2 = 0x1E;   // Error flag 2. Read only

const uint8_t TLC_RESET = 0x6B; // Reset all TLC59116s

// Possible LED Output States
const uint8_t LED_OFF = 0x00;   // LED off . Default state of the LED during power up
const uint8_t LED_ON = 0x01;    // LED on. LED is on at full brightness
const uint8_t LED_PWM = 0x02;   // LED PWM. LED driver is controlled by the PWM register
const uint8_t LED_GROUP = 0x03; // LED group. LED driver is controlled by the group PWM and group FREQ registers (blinking)

//<<constructor>> setup the TLC59116
TLC59116::TLC59116(uint8_t address, uint8_t resetPin)
    : TLC_ADDR(address),
      RESET(resetPin),
      ERR_FLAG(0),
      groupMode(0)
{
  gpio_init(RESET); // initialize the RESET pin. Output direction
  gpio_set_dir(RESET, GPIO_OUT);
  gpio_put(RESET, 1); // Reset is active low. Set it high to start with.

  // Initialise the class variables
  // I2C address of the TLC59116
  groupMode = 0; // Group dimming(0) or blinking(1)
  LEDOUT[0] = 0x00;
  LEDOUT[1] = 0x00;
  LEDOUT[2] = 0x00;
  LEDOUT[3] = 0x00;  // LED output drive state registers
  ERR_FLAG = 0xFFFF; // Error flag
}

//<<destructor>>
TLC59116::~TLC59116() {}

/**
 * @brief  Initialize the I2C bus to interface with the TLC59116
 *
 * I2C Initialisation. Using it at 400Khz and defined I2C port, SDA and SCL pins.
 * Pull-up resistors are enabled on the SDA and SCL lines.
 * Finally enable the TLC59116 driver.
 * @param  None
 * @retval None
 *
 */

void TLC59116::begin()
{

  // use the defines for the I2C port and pins
  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C

  // Enable the pull-up resistors on the I2C bus
  gpio_pull_up(I2C_SDA); // Enable the pull-up resistor on the SDA line
  gpio_pull_up(I2C_SCL); // Enable the pull-up resistor on the SCL line
  enableTLC();           // Start up the driver in a fresh state
}

// Write a value to a LED register of the TLC59116.

void TLC59116::writeToDevice(uint8_t reg, uint8_t newLEDOUT)
{
  uint8_t data[2] = {reg, newLEDOUT};
  i2c_write_blocking(i2c0, TLC_ADDR, data, 2, false);
}

uint8_t TLC59116::readFromDevice(uint8_t reg)
{
  uint8_t result;
  i2c_write_blocking(i2c0, TLC_ADDR, &reg, 1, true); // true to keep master control of bus
  i2c_read_blocking(i2c0, TLC_ADDR, &result, 1, false);
  return result;
}

// Datasheet 9.5.6
// --------------------------------------------------------------------
// When modifying a channel output, maintain all other channel outputs.
// --------------------------------------------------------------------

uint8_t TLC59116::modifyLEDOutputState(uint8_t LED, uint8_t state)
{
  uint8_t pos = ((LED % 4) * 2); // Position of the LED in the register. Each LED takes 2 bits.
  uint8_t regLoc = LED / 4;      // Register location of the LED. Each register holds 4 LEDs.
  uint8_t mask = 0x03 << pos;    // Mask to clear the LED bits in the register.
  uint8_t newLEDOUT;             // New LED output state register value.

  newLEDOUT = LEDOUT[regLoc] | mask;  // Clear the LED bits in the register.
  newLEDOUT = LEDOUT[regLoc] & ~mask; // Clear the LED bits in the register.
  if (state != 0x00)
    newLEDOUT = LEDOUT[regLoc] | (state << pos); // Set the LED bits in the register.

  LEDOUT[regLoc] = newLEDOUT;                 // Update the LED output state register value.
  writeToDevice(LEDOUT0 + regLoc, newLEDOUT); // Write the new LED output state register value to the TLC59116.
  return newLEDOUT;
}

void TLC59116::LEDOn(uint8_t LED)
{
  modifyLEDOutputState(LED, LED_ON);
}

void TLC59116::LEDOff(uint8_t LED)
{
  modifyLEDOutputState(LED, LED_OFF);
}

void TLC59116::LEDPWM(uint8_t LED)
{
  setPWM(LED, 255);
  modifyLEDOutputState(LED, LED_PWM);
}

void TLC59116::LEDGroup(uint8_t LED)
{
  setPWM(LED, 255);
  modifyLEDOutputState(LED, LED_GROUP);
}
// --------------------------------------------------------------------

// Datasheet 9.6.3
void TLC59116::setPWM(uint8_t pin, uint8_t duty)
{
  writeToDevice(PWM0 + pin, duty);
}

// Datasheet 9.6.4

/**
 *  When DMBLNK bit is set to 0 (MODE2 register), a 190 Hz signal is superimposed on the PWM signal.
 *  GRPPWM register sets the duty cycle of the overall global brightness control.
 * Allowing the LEDs to be dimmed with the same value. The value of GRPFREQ is don't care.
 *
 *  General brightness is control for the 16 outputs from 00h (0%) to FFh (100%).
 *  This is applicable to LED outputs programmed with LDRx= 11 (LEDOUT0, LEDOUT1, LEDOUT2, LEDOUT3 registers).
 *
 */

void TLC59116::setGroupPWM(uint8_t duty)
{
  // test if the group mode is already set to global blinking. If so, set it to global PWM.
  if (groupMode == 1)
  {
    writeToDevice(MODE2, 0x00); // Set the group mode to global PWM
    groupMode = 0;              // Set the group mode flag to global PWM
  }
  // Set the global PWM duty cycle with the argument value duty
  writeToDevice(GRPPWM, duty);
}

/**
 *  When DMBLNK bit is set to 1 (MODE2 register), the GRPPWM and GRPFREQ registers control the blinking pattern
 *  GRPFREQ register sets the frequency of the blinking signal from 24Hz to 10.73s. and GPRPWM sets the duty cycle.
 *  The blinking period is controlled through 256 linear steps from 00h (41 ms, frequency 24 Hz) to FFh (10.73 s).
 *
 *  Global blinking period (seconds) = (GRPFREQ + 1) / 24. if blinking period is BLINKPERIOD, then
 *  we can calculate BLINKPERIOD = (GRPFREQ + 1) / 24. The frequency is 1/BLINKPERIOD.
 *  GRPFREQ as a function of BLINKPERIOD is GRPFREQ = (BLINKPERIOD * 24) - 1.
 *  BLINKFREQ is the frequency of the blinking signal in Hz. It is calculated as 1/BLINKPERIOD.
 *  BLINKFREQ as a function of GRPFREQ is BLINKFREQ = 1 / ((GRPFREQ + 1) / 24).
 *  Then GRPFREQ as a function of BLINKFREQ is GRPFREQ = (24 / BLINKFREQ) - 1.
 *  @param freq: The blinking frequency parameter GRPFREQ (From 0x00 to 0xFF).
 *  freq is then setting blinking frequency from 0.0932Hz (0xFF) to 24 Hz (0x00).
 */

void TLC59116::setGroupBlink(uint8_t freq, uint8_t duty)
{
  setGroupPWM(duty);  // Set the global PWM duty cycle with the argument value duty
  if (groupMode == 0) // test if the group mode is already set to global PWM. If so, set it to global blinking.
  {
    // bit 5 of the MODE2 register is the DMBLNK bit. Set it to 1 to enable global blinking.
    writeToDevice(MODE2, 0x20); // Set the group mode to global blinking
    groupMode = 1;              // Set the group mode flag to global blinking
  }

  writeToDevice(GRPFREQ, freq); // Set the global blinking frequency with the argument value freq
}

// Datasheet 9.5.2
void TLC59116::clearErrors()
{
  writeToDevice(0x01, 0x80);
  writeToDevice(0x01, 0x00);
}

// Datasheet 9.3.2 & 9.5.10
bool TLC59116::checkErrors()
{
  uint8_t flag1 = readFromDevice(ERRFLAG1);
  uint8_t flag2 = readFromDevice(ERRFLAG2);
  printf("Flag1: 0x%02X\n", flag1);
  printf("Flag2: 0x%02X\n", flag2);
  ERR_FLAG = (flag2 << 8) + flag1;

  if (ERR_FLAG == 0xFFFF)
    return false;
  return true;
}

void TLC59116::reportErrors()
{
  if (ERR_FLAG == 0xFFFF)
  {
    printf("No errors detected.\n");
  }

  else
  {
    printf("Error Flag: 0x%04X\n", ERR_FLAG);
    for (uint8_t i = 0; i < 16; i++)
    {
      if (!((ERR_FLAG >> i) & 0x1))
      {
        printf("Channel %d overtemperature or open circuit.\n", i);
      }
    }
  }
  clearErrors();
}

// Start up the driver in a fresh state
void TLC59116::enableTLC()
{
  writeToDevice(0x00, 0x80);
}

// Reset the driver either by the RESET pin, and if it was not defined just turn off all the LEDs.
void TLC59116::resetDriver()
{
  for (int i = 0; i < 4; i++)
  {
    LEDOUT[i] = 0x00;
  }

  gpio_put(RESET, 0); // digitalWrite(RESET, LOW);
  sleep_ms(1);        // delay(1); // Allow a millisecond for the device to power down.
  gpio_put(RESET, 1); // digitalWrite(RESET, HIGH);
  enableTLC();
}

// Nice function to turn off all LED channels.
void TLC59116::turnOffAllLEDs()
{
  for (uint8_t i = 0; i < 16; i++)
  {
    LEDOff(i);
  }
}

// Datasheet 9.3.5
void TLC59116::resetAllTLCs()
{
  for (uint8_t i = 0; i < 4; i++)
    LEDOUT[i] = 0x00;

  uint8_t data[2] = {0xA5, 0x5A};                 // Magic reset sequence
  i2c_write_blocking(i2c0, 0x6B, data, 2, false); // Reset all TLC59116s

  enableTLC(); // Start up the driver in a fresh state
}
