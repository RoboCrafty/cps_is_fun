#include "lab02.h"

#include <xc.h>
#include <p33Fxxxx.h>
//do not change the order of the following 2 definitions
#define FCY 12800000UL
#include <libpic30.h>

#include "types.h"
#include "lcd.h"
#include "led.h"

#define FCY_EXT 32768

void initialize_timer()
{
    // Enable RTC Oscillator -> this effectively does OSCCONbits.LPOSCEN = 1
    // but the OSCCON register is lock protected. That means you would have to 
    // write a specific sequence of numbers to the register OSCCONL. After that 
    // the write access to OSCCONL will be enabled for one instruction cycle.
    // The function __builtin_write_OSCCONL(val) does the unlocking sequence and
    // afterwards writes the value val to that register. (OSCCONL represents the
    // lower 8 bits of the register OSCCON)
    __builtin_write_OSCCONL(OSCCONL | 2);
    // Disable the Timers


//Disable Timer
//Select internal instruction cycle clock
//Disable Gated Timer mode
//Clear timer register
//Select 1:64 Prescaler
//Load the period value
//Set Timer1 Interrupt Priority Level
//Clear Timer1 Interrupt Flag
//Enable Timer1 interrupt
//Start Timer
//    
    // Set Prescaler

    // Set Clock Source
    
    // Set Gated Timer Mode -> don't use gating

    // T1: Set External Clock Input Synchronization -> no sync

    // Load Timer Periods
    
    // Reset Timer Values

    // Set Interrupt Priority

    // Clear Interrupt Flags

    // Enable Interrupts

    // Enable the Timers
    
    
     // Setup Timer 2
    CLEARBIT(T2CONbits.TON);
    CLEARBIT(T2CONbits.TCS);
    CLEARBIT(T2CONbits.TGATE);
    TMR2 = 0x00;
    IPC0bits.T1IP = 0x01;
    T2CONbits.TCKPS = 0b11;
    CLEARBIT(IFS0bits.T2IF);
    IEC0bits.T2IE = 1;
    PR2 = 100;
    T2CONbits.TON = 1;

// Disable Timer

}

void timer_loop()
{
    // print assignment information
    lcd_printf("Lab02: Int & Timer");
    lcd_locate(0, 1);
    lcd_printf("Group: CPS19");
    
    while(TRUE)
    {
        
    }
}

void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T1Interrupt(void)
{ // invoked every ??
    
}

void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T2Interrupt(void)
{ // invoked every ??
    LED1_PORT = !LED1_PORT;             // Toggle LED1
    CLEARBIT(IFS0bits.T2IF);   // Clear interrupt flag
}
