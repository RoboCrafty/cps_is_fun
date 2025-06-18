#include <xc.h>
#include <p33Fxxxx.h>
//do not change the order of the following 2 definitions
#define FCY 12800000UL 
#include <libpic30.h>

#include "lcd.h"
#include "led.h"

/* Configuration of the Chip */
// Initial Oscillator Source Selection = Primary (XT, HS, EC) Oscillator with PLL
#pragma config FNOSC = PRIPLL
// Primary Oscillator Mode Select = XT Crystal Oscillator mode
#pragma config POSCMD = XT
// Watchdog Timer Enable = Watchdog Timer enabled/disabled by user software
// (LPRC can be disabled by clearing the SWDTEN bit in the RCON register)
#pragma config FWDTEN = OFF



int main(){
    //Init LCD and LEDs
    lcd_initialize();
    led_init();
    
	
    // Clear the Screen and reset the cursor
    lcd_clear();
    lcd_locate(0, 0);

    // Print Hello World
    lcd_printf("Group Members:");
    lcd_locate(0, 2);
    lcd_printf("* Yunhan Zheng");
    lcd_locate(0, 3);
    lcd_printf("* Fuzail Hamid");
    lcd_locate(0,4);
    lcd_printf("* Maria Frosini");
    
    int counter=0; 
    while(1)
    {
        
        
        lcd_locate(0, 6);
        lcd_printf("Counter: %d   ", counter);
        
        LED1_PORT=(counter & 0x01) ? 1:0;
        Nop();
        LED2_PORT=(counter & 0x02) ? 1:0;
        Nop();
        LED3_PORT=(counter & 0x04) ? 1:0;
        Nop();
        LED4_PORT=(counter & 0x08) ? 1:0;
        Nop();
        LED5_PORT=(counter & 0x10) ? 1:0;
        Nop();
        __delay_ms(300);
        counter=(counter+1);
    }
   
}
