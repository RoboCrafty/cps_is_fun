#include "lab03.h"

#include <xc.h>
//do not change the order of the following 2 definitions
#define FCY 12800000UL
#include <libpic30.h>

#include "types.h"
#include "lcd.h"
#include "led.h"

/*
 * DAC code
 */

#define DAC_CS_TRIS TRISDbits.TRISD8
#define DAC_SDI_TRIS TRISBbits.TRISB10
#define DAC_SCK_TRIS TRISBbits.TRISB11
#define DAC_LDAC_TRIS TRISBbits.TRISB13
    
#define DAC_CS_PORT PORTDbits.RD8
#define DAC_SDI_PORT PORTBbits.RB10
#define DAC_SCK_PORT PORTBbits.RB11
#define DAC_LDAC_PORT PORTBbits.RB13

#define DAC_SDI_AD1CFG AD1PCFGLbits.PCFG10
#define DAC_SCK_AD1CFG AD1PCFGLbits.PCFG11
#define DAC_LDAC_AD1CFG AD1PCFGLbits.PCFG13

#define DAC_SDI_AD2CFG AD2PCFGLbits.PCFG10
#define DAC_SCK_AD2CFG AD2PCFGLbits.PCFG11
#define DAC_LDAC_AD2CFG AD2PCFGLbits.PCFG13

void dac_initialize()
{
    // set AN10, AN11 AN13 to digital mode
    // this means AN10 will become RB10, AN11->RB11, AN13->RB13
    // see datasheet 11.3
    SETBIT(DAC_SDI_AD1CFG);
    SETBIT(DAC_SCK_AD1CFG);
    SETBIT(DAC_LDAC_AD1CFG);
    SETBIT(DAC_SDI_AD2CFG);
    SETBIT(DAC_SCK_AD2CFG);
    SETBIT(DAC_LDAC_AD2CFG);
    //set RD8, RB10, RB11, RB13 as output pins
    CLEARBIT(DAC_CS_TRIS);
    CLEARBIT(DAC_SDI_TRIS);
    CLEARBIT(DAC_SCK_TRIS);
    CLEARBIT(DAC_LDAC_TRIS);
    
    
    // set default state: CS=??, SCK=??, SDI=??, LDAC=??
    // Set default state:
    SETBIT(DAC_CS_PORT);    // CS high (inactive)
    CLEARBIT(DAC_SDI_PORT); // SDI low
    CLEARBIT(DAC_SCK_PORT); // SCK low
    SETBIT(DAC_LDAC_PORT);  // LDAC high
    
}

void dac_write(uint16_t value) {
    // Construct 16-bit command for DAC A, gain=1, output enabled
    uint16_t command = (1 << 15) |    // DAC A
                      (0 << 14) |    // Unbuffered
                      (0 << 13) |    // Gain = 1 (0-4.095V range)
                      (1 << 12) |    // Output enabled
                      (value & 0x0FFF); // 12-bit value
    
    // Begin SPI transmission
    CLEARBIT(DAC_CS_PORT); // CS low
    
    // Send 16 bits MSB first
    int i;
    for(i = 15; i >= 0; i--) {
        if(command & (1 << i)) {
            SETBIT(DAC_SDI_PORT);
        } else {
            CLEARBIT(DAC_SDI_PORT);
        }
        
        // Generate clock pulse
        Nop();   // Give time for SDI to settle
        SETBIT(DAC_SCK_PORT);
        Nop();   // Hold clock high
        CLEARBIT(DAC_SCK_PORT);
    }
        
    
    
    // End transmission
    SETBIT(DAC_CS_PORT); // CS high
    CLEARBIT(DAC_SDI_PORT);
    Nop();
    
    // Update DAC output
    CLEARBIT(DAC_LDAC_PORT);
    Nop();
    Nop();
    SETBIT(DAC_LDAC_PORT);
}

void dac_output_voltage(float voltage) {
    if (voltage < 0.0f) voltage = 0.0f;
    if (voltage > 4.095f) voltage = 4.095f;
    
    uint16_t dac_value = (uint16_t)((voltage / 4.095f) * 4095.0f);
    
    
    //lcd_clear();
    lcd_locate(0, 5);
    lcd_printf("DAC=%04X", dac_value); // Print voltage and value in hex
    
    dac_write(dac_value);
}


/*
 * Timer code
 */

#define FCY_EXT   32768UL

#define TCKPS_1   0x00
#define TCKPS_8   0x01
#define TCKPS_64  0x02
#define TCKPS_256 0x03

void timer_initialize()
{
    // Enable RTC Oscillator -> this effectively does OSCCONbits.LPOSCEN = 1
    // but the OSCCON register is lock protected. That means you would have to 
    // write a specific sequence of numbers to the register OSCCONL. After that 
    // the write access to OSCCONL will be enabled for one instruction cycle.
    // The function __builtin_write_OSCCONL(val) does the unlocking sequence and
    // afterwards writes the value val to that register. (OSCCONL represents the
    // lower 8 bits of the register OSCCON)
    __builtin_write_OSCCONL(OSCCONL | 2);
    // configure timer
    
}

// interrupt service routine?

/*
 * main loop
 */

void main_loop()
{
    lcd_printf("Lab03: DAC");
    lcd_locate(0, 1);
    lcd_printf("Group: 19");
    
    __delay_ms(1000);  // Give time to read message


    
    
    while (1) {        
    
        // Toggle LED1 at start of each iteration
        //led_toggle(LED1);
        
        // 1V output with 500ms delay
        dac_output_voltage(1.0f);
        __delay_ms(500);  // Slow enough to read updates
        
        // 2.5V output with 2000ms delay
        dac_output_voltage(2.5f);
         __delay_ms(2000);  // Slow enough to read updates
        
        // 3.5V output with 1000ms delay
        dac_output_voltage(3.5f);
         __delay_ms(1000);  // Slow enough to read updates
    }
    
}