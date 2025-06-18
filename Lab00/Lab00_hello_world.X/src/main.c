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
    //Init LCD
    lcd_initialize();
    lcd_clear();
    //
    //
    //
    lcd_clear_row(1);
    //
    //
    lcd_locate(7, 3);
    
    
    // Clear the Screen and reset the cursor
       /*
     Add code here
     */
	
    
    
	lcd_printf("Hello Wld!");
    
    // Stop
    while(1);
}

