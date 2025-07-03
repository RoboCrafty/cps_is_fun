#include "lab05.h"

#include <xc.h>
//do not change the order of the following 2 definitions
#define FCY 12800000UL
#include <libpic30.h>

#include "types.h"
#include "lcd.h"
#include "led.h"

/*
 * PWM code
 */
void servo_init(char servo_number){
    CLEARBIT(T2CONbits.TON); // Disable Timer
    CLEARBIT(T2CONbits.TCS); // Select internal instruction cycle clock
    CLEARBIT(T2CONbits.TGATE); // Disable Gated Timer mode
    TMR2 = 0x00; // Clear timer register
    T2CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
    CLEARBIT(IFS0bits.T2IF); // Clear Timer2 interrupt status flag
    CLEARBIT(IEC0bits.T2IE); // Disable Timer2 interrupt enable control bit
    PR2 = 4000; // Set timer period 20 ms:
        // 8000 = 40*10^-3 * 12.8*10^6 * 1/64
    
    if (servo_number == 'X')
    {
        // Setup OC8
        CLEARBIT(TRISDbits.TRISD7); // Set OC8 as output
        OC8R = 4000-1.75*200; // Set the initial duty cycle to 1.5 ms
        lcd_printf("XDD" );
        OC8RS = 4000-1.75*200; // Load OCRS: next pwm duty cycle
        OC8CON = 0x0006; // Set OC8: PWM, no fault check, Timer2
        
        
    }
    else if (servo_number == 'Y')
    {
    
        // Setup OC7
        CLEARBIT(TRISDbits.TRISD6); // Set OC7 as output
        OC7R = 4000-1.5*200; // Set the initial duty cycle to 5 ms
        OC7RS = 4000-1.5*200; // Load OCRS: next pwm duty cycle
        OC7CON = 0x0006; // Set OC7: PWM, no fault check, Timer1
        
    }
    SETBIT(T2CONbits.TON); // Turn Timer 1 on    
 }   
    
void set_duty_cycle(char servo_number, float duty_cycle){
    if (servo_number == 'X')
    {
        OC8RS=4000-(duty_cycle*200);
    }
    else if (servo_number == 'Y')
    {
        OC7RS=4000-(duty_cycle*200);
    }
}



#define TCKPS_1   0x00
#define TCKPS_8   0x01
#define TCKPS_64  0x02
#define TCKPS_256 0x03

#define PWM_MIN_US 1000
#define PWM_MID_US 1500
#define PWM_MAX_US 2000
#define PWM_CYC_US 20000



void touchscreen_init()
{
    CLEARBIT(AD1CON1bits.ADON);
    //initialize PIN
    SETBIT(TRISEbits.TRISB15); // Set TRISE RE15 to input
    CLEARBIT(AD1PCFGHbits.PCFG15); // Set AD1 AN20 input pin as analog
    
    SETBIT(TRISEbits.TRISB9); // Set TRISE RE9 to input
    CLEARBIT(AD1PCFGHbits.PCFG9); // Set AD1 AN20 input pin as analog
    //Configure AD1CON1
    CLEARBIT(AD1CON1bits.AD12B); // Set 10b Operation Mode
    AD1CON1bits.FORM = 0; // Set integer output
    AD1CON1bits.SSRC = 0x7; // Set automatic conversion
    // Configure AD1CON2
    AD1CON2 = 0; // Not using scanning sampling
    //Configure AD1CON3
    CLEARBIT(AD1CON3bits.ADRC); // Internal clock source
    AD1CON3bits.SAMC = 0x1F; // Sample-to-conversion clock = 31Tad
    AD1CON3bits.ADCS = 0x2; // Tad = 3Tcy (Time cycles)
    // Leave AD1CON4 at its default value
    // Enable ADC
    SETBIT(AD1CON1bits.ADON);
    
   
    
}

void dimention_touchscreen(char touchscreen_dimention){
    AD1CHS0bits.CH0SA = 0x014; // Set ADC to Sample AN20 pin
    SETBIT(AD1CON1bits.SAMP); // Start to sample
    while(!AD1CON1bits.DONE); // Wait for conversion to finish
    CLEARBIT(AD1CON1bits.DONE); // MUST HAVE! Clear conversion done bit
}

/*
 * main loop
 */

void main_loop()
{
    // print assignment information
    lcd_printf("Lab05: Touchscreen &\r\n");
    lcd_printf("       Servos");
    lcd_locate(0, 2);
    lcd_printf("Group: GroupName");
    
    servo_init('X');
    servo_init('Y');
    
    

    
    while(TRUE) {
     set_duty_cycle('X',1.4);    
     set_duty_cycle('Y',1.2); 
     
     __delay_ms(5000);
     
     set_duty_cycle('X',1.4);    
     set_duty_cycle('Y',1.7); 
     
     __delay_ms(5000);
     
     set_duty_cycle('X',2.0);    
     set_duty_cycle('Y',1.7); 
     
     __delay_ms(5000);
     
     set_duty_cycle('X',2.0);    
     set_duty_cycle('Y',1.2); 
     
     __delay_ms(5000);
        
    }
}
