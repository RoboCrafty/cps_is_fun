#include "lab03.h"

#include <xc.h>
// Do not change the order of the following 2 definitions
#define FCY 12800000UL
#include <libpic30.h>

#include "types.h"
#include "lcd.h"
#include "led.h"

/*
 * DAC (Digital to Analog Converter) Interface
 */

// Define pin directions (TRIS) for DAC signals
#define DAC_CS_TRIS    TRISDbits.TRISD8
#define DAC_SDI_TRIS   TRISBbits.TRISB10
#define DAC_SCK_TRIS   TRISBbits.TRISB11
#define DAC_LDAC_TRIS  TRISBbits.TRISB13

// Define ports for DAC signals
#define DAC_CS_PORT    PORTDbits.RD8
#define DAC_SDI_PORT   PORTBbits.RB10
#define DAC_SCK_PORT   PORTBbits.RB11
#define DAC_LDAC_PORT  PORTBbits.RB13

// ADC configuration bits for switching AN pins to digital mode
#define DAC_SDI_AD1CFG AD1PCFGLbits.PCFG10
#define DAC_SCK_AD1CFG AD1PCFGLbits.PCFG11
#define DAC_LDAC_AD1CFG AD1PCFGLbits.PCFG13

#define DAC_SDI_AD2CFG AD2PCFGLbits.PCFG10
#define DAC_SCK_AD2CFG AD2PCFGLbits.PCFG11
#define DAC_LDAC_AD2CFG AD2PCFGLbits.PCFG13

/**
 * Initialize DAC pins and set initial states.
 * AN10, AN11, and AN13 are set to digital I/O.
 * Set corresponding pins as outputs and initialize output levels.
 */
void dac_initialize()
{
    // Set analog pins AN10, AN11, AN13 to digital mode (RB10, RB11, RB13)
    SETBIT(DAC_SDI_AD1CFG);
    SETBIT(DAC_SCK_AD1CFG);
    SETBIT(DAC_LDAC_AD1CFG);
    //SETBIT(DAC_SDI_AD2CFG);
    //SETBIT(DAC_SCK_AD2CFG);
    //SETBIT(DAC_LDAC_AD2CFG);

    // Configure pins as outputs
    CLEARBIT(DAC_CS_TRIS);    // CS pin output
    CLEARBIT(DAC_SDI_TRIS);   // SDI pin output
    CLEARBIT(DAC_SCK_TRIS);   // SCK pin output
    CLEARBIT(DAC_LDAC_TRIS);  // LDAC pin output

    // Set default states for pins:
    // CS high (inactive), SDI low, SCK low, LDAC high
    SETBIT(DAC_CS_PORT);
    CLEARBIT(DAC_SDI_PORT);
    CLEARBIT(DAC_SCK_PORT);
    SETBIT(DAC_LDAC_PORT);
}

/**
 * Send a 12-bit value to the DAC via bit-banged SPI.
 * The command format for DAC: 
 * Bit 15 = 0 (DAC A)
 * Bit 14 = 0 (Unbuffered)
 * Bit 13 = 0 (Gain = 1x)
 * Bit 12 = 1 (Output enabled)
 * Bits 11-0 = 12-bit DAC value
 * 
 * @param value 12-bit DAC data to send
 */
void dac_write(uint16_t value) 
{
    uint16_t command = (0 << 15) |    // Select DAC A
                       (0 << 14) |    // Unbuffered input
                       (0 << 13) |    // Gain = 1
                       (1 << 12) |    // Output enabled
                       (value & 0x0FFF); // 12-bit data

    // Start SPI transaction by pulling CS low
    CLEARBIT(DAC_CS_PORT);
    // Define variable for loop.
    int i;
    // Transmit 16 bits MSB first
    for (i = 15; i >= 0; i--) {
        if (command & (1 << i)) {
            SETBIT(DAC_SDI_PORT);  // Set SDI high if bit is 1
        } else {
            CLEARBIT(DAC_SDI_PORT); // Otherwise low
        }

        Nop();                // Small delay to settle data line
        SETBIT(DAC_SCK_PORT); // Clock high - data read by DAC on rising edge
        Nop();                // Hold clock high briefly
        CLEARBIT(DAC_SCK_PORT);// Clock low - prepare for next bit
    }

    // End SPI transaction by setting CS high
    SETBIT(DAC_CS_PORT);

    // Clear SDI line after transmission
    CLEARBIT(DAC_SDI_PORT);
    Nop();

    // Pulse LDAC low to update DAC output with new data
    CLEARBIT(DAC_LDAC_PORT);
    Nop();
    Nop();
    SETBIT(DAC_LDAC_PORT);
}

/**
 * Convert a desired voltage to a DAC value and output it.
 * Voltage range is clamped between 0 and 4.095 V.
 * The DAC is 12-bit with a 4.095V reference.
 * 
 * @param voltage Desired output voltage (0.0 to 4.095 V)
 */
void dac_output_voltage(float voltage) 
{
    if (voltage < 0.0f) voltage = 0.0f;
    if (voltage > 4.095f) voltage = 4.095f;

    uint16_t dac_value = (uint16_t)((voltage / 4.095f) * 4095.0f);

    // Display DAC value on LCD for debugging
    lcd_locate(0, 5);
    lcd_printf("DAC=%04X", dac_value);

    // Send the value to the DAC
    dac_write(dac_value);
}

/*
 * Timer configuration and interrupt for delay management
 */

#define FCY_EXT   32768UL  // External clock frequency (RTC oscillator)
#define TCKPS_1   0x00
#define TCKPS_8   0x01
#define TCKPS_64  0x02
#define TCKPS_256 0x03

/**
 * Initialize Timer1 with external 32.768 kHz clock.
 * Timer interrupt triggers every 0.5 seconds.
 */
void timer_initialize()
{
    // Enable RTC oscillator (LP Oscillator) safely via OSCCONL unlocking
    __builtin_write_OSCCONL(OSCCONL | 2);

    // Configure Timer1
    T1CONbits.TON = 0;      // Disable timer during config
    T1CONbits.TCS = 1;      // Use external clock source (RTC oscillator)
    T1CONbits.TGATE = 0;    // Gated time accumulation disabled
    T1CONbits.TCKPS = 0b00; // Prescaler 1:1

    TMR1 = 0x00;            // Clear timer register
    PR1 = 0.5 * FCY_EXT;    // Period register: 0.5 seconds

    IPC0bits.T1IP = 0x01;   // Set Timer1 interrupt priority low
    IFS0bits.T1IF = 0;      // Clear interrupt flag
    IEC0bits.T1IE = 1;      // Enable Timer1 interrupt

    T1CONbits.TON = 1;      // Start Timer1
}

// Global counter incremented by Timer1 ISR every 0.5 seconds
volatile uint8_t global_counter = 0;

/**
 * Timer1 Interrupt Service Routine
 * Increment global counter and clear interrupt flag.
 */
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    global_counter++;
    IFS0bits.T1IF = 0;  // Clear Timer1 interrupt flag
}

/*
 * Main application loop
 */
void main_loop()
{
    // Display initial message on LCD
    lcd_printf("Lab03: DAC");
    lcd_locate(0, 1);
    lcd_printf("Group: 19");

    __delay_ms(1000);  // Wait 1 second for user to read

    while (1) {
        // Output 1.0V, wait ~500ms, toggle LED1
        dac_output_voltage(1.0f);
        global_counter = 0;
        while(global_counter < 1) {} // wait 0.5s (1 * 0.5s)
        

        // Output 2.5V, wait ~2s, toggle LED1
        dac_output_voltage(2.5f);
        global_counter = 0;
        while(global_counter < 4) {} // wait 2.0s (4 * 0.5s)
        

        // Output 3.5V, wait ~1s, toggle LED1
        dac_output_voltage(3.5f);
        global_counter = 0;
        while(global_counter < 2) {} // wait 1.0s (2 * 0.5s)
        TOGGLELED(LED1_PORT);
    }
}
