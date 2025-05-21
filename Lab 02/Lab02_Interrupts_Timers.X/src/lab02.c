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

uint16_t counter = 0;// number of iteration
uint16_t second = 0;
uint16_t millisec = 0;
uint16_t cycles = 0;
double time_mm = 0;

uint16_t min = 0;
uint16_t sec = 0;


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
     
    
     // Setup Timer 2
    CLEARBIT(T2CONbits.TON);
    CLEARBIT(T2CONbits.TCS);
    CLEARBIT(T2CONbits.TGATE);
    TMR2 = 0x00;
    IPC1bits.T2IP = 0x01;
    T2CONbits.TCKPS = 0b11;
    CLEARBIT(IFS0bits.T2IF);
    IEC0bits.T2IE = 1;
    PR2 = 100;
    T2CONbits.TON = 1;



    // Setup Timer 1
    CLEARBIT(T1CONbits.TON); // disable the timer
    T1CONbits.TCKPS = 0b11; // 1:256 prescalar
    T1CONbits.TCS = 1; // use the external clock source of 32.768 KHz (manual page 11)
    CLEARBIT(T1CONbits.TGATE); // disable gated timer mode
    T1CONbits.TSYNC = 0; // disable syncro
    PR1 = 128; // set the PR1, same formula as timer 2
    TMR1 = 0x00; // rest TMR1 to 0
    IPC0bits.T1IP = 0x02; // set priority level: here is set to one but we have to decide
    IFS0bits.T1IF = 0; //clear flag
    IEC0bits.T1IE = 1; //enable interrupt 
    T1CONbits.TON = 1; // start the timer


    // Setup Timer 3

    CLEARBIT(T3CONbits.TON); // disable the timer
    T3CONbits.TCKPS = 0b00; // 1:1 prescalar
    CLEARBIT(T3CONbits.TCS); // Select internal instruction cycle clock
    CLEARBIT(T3CONbits.TGATE); // disable gated timer mode
    PR3 = 65535; //set the PR higher value possible (considering a 16 bit register)
    
    TMR3 = 0x00;
    T3CONbits.TON = 1;
    

}

void timer_loop()
{
    // print assignment information
    lcd_printf("Lab02: Int & Timer");
    lcd_locate(0, 1);
    lcd_printf("Group: CPS19");
    
   

    
    
    while(TRUE)
    {
        counting(counter);
        lcd_locate(0,2);
        lcd_printf("cycles: %u" , cycles);
        lcd_locate(0,3);
        lcd_printf("d: %.4f ms", time_mm );
        counter++;
       
        
        
        
        
        
       
        //min = second % 60;
        //sec = second - (60 * min);
        
        lcd_locate(0,6);
        lcd_printf("%02u:%02u:%03u", min, sec, millisec);
        
        
        
        
    }
    
}

void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T1Interrupt(void)
{ // invoked every 1 s
    TOGGLELED(LED2_PORT);   // Toggle LED2
    sec++;
    if (sec==60){
            sec = 0;
            min ++;
        }
        
    CLEARBIT(IFS0bits.T1IF);   // Clear interrupt flag   
    
}

void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T2Interrupt(void)
{ // invoked every 2 ms
    TOGGLELED(LED1_PORT);            // Toggle LED1
    millisec+=2;
    if (millisec==1000){
            millisec = 0;
            
        }
    
    CLEARBIT(IFS0bits.T2IF);   // Clear interrupt flag
}

// function that checks at which iteration we are and toggle the LED every 2000 iterations
void counting(int counter){
    if ((counter % 2000 )== 0){
            
            T3CONbits.TON = 0; // stop timer 3
            cycles = TMR3; // read TMR3
           
            
            time_mm = (double)cycles * 1000 / FCY;
           
            TOGGLELED(LED3_PORT);//Toggle LED3
            TMR3 = 0x00; 
            T3CONbits.TON = 1;
           
            
            
    }

}



