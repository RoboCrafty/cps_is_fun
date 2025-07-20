#include "lab06.h"

#include <xc.h>
//do not change the order of the following 2 definitions
#define FCY 12800000UL
#include <libpic30.h>
#include <stdint.h>

#include <math.h>

#include "types.h"
#include "lcd.h"
#include "led.h"
#include <stdbool.h>


/*
 * Parameter
 */



/*
 * Common Definitions
 */
#define TCKPS_1   0x00
#define TCKPS_8   0x01
#define TCKPS_64  0x02
#define TCKPS_256 0x03

#define CIRCLE_RADIUS 70.0f
#define CIRCLE_SPEED -3.0f         // radians per second
#define PI 3.1415926f
#define center_x 500
#define center_y 235




/*
 * Global Variables
 */
volatile uint16_t x_touchscreen;
volatile uint16_t y_touchscreen;
volatile char dimensionB;
int counter = -1;
double x_raw = 0.0;
double y_raw = 0.0;

int missed_deadline= 0;
int md_counter=0;
float x_filtered = 400.0f;
float y_filtered = 400.0f;
float t_seconds = 0.0f;

bool task_ready = false;



/*
 * Timer Code
 */
void initialize_timer1()
{
    // Setup Timer 1
    CLEARBIT(T1CONbits.TON); // disable the timer
    T1CONbits.TCKPS = 0b11; // 1:256 prescalar
    T1CONbits.TCS = 0;
    CLEARBIT(T1CONbits.TGATE); // disable gated timer mode
    T1CONbits.TSYNC = 0; // disable syncro
    PR1 = 500; // Set Timer1 period for 10 ms
    TMR1 = 0x00; // rest TMR1 to 0
    IPC0bits.T1IP = 0x06; // set priority level: here is set to one but we have to decide
    IFS0bits.T1IF = 0; //clear flag
    IEC0bits.T1IE = 1; //enable interrupt 
    T1CONbits.TON = 1; // start the timer
    
}

/*
 * Interrupt Code
 */
void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _T1Interrupt(void)
{ 
    counter++;
    task_ready = true; // Signal that it's time to run a task
    if (counter > 2) {
        counter = 1; // Reset counter after 3 iterations
    }
    CLEARBIT(IFS0bits.T1IF);   // Clear interrupt flag
}

/**
 * @brief Initialize the servo PWM signals
 * @param servo_number 'X' or 'Y' to select the servo
 */

void servo_init(char servo_number) {
    CLEARBIT(T2CONbits.TON);    // Disable Timer2
    CLEARBIT(T2CONbits.TCS);    // Select internal instruction cycle clock
    CLEARBIT(T2CONbits.TGATE);  // Disable Gated Timer mode
    TMR2 = 0x00;                // Clear Timer2 register
    T2CONbits.TCKPS = 0b10;     // Select 1:64 Prescaler
    CLEARBIT(IFS0bits.T2IF);    // Clear Timer2 interrupt flag
    CLEARBIT(IEC0bits.T2IE);    // Disable Timer2 interrupt
    PR2 = 4000;                 // Set Timer2 period for 20 ms (PWM period)

    if (servo_number == 'X') {
        // Setup OC8 for X servo PWM output
        CLEARBIT(TRISDbits.TRISD7); // Set OC8 pin as output
        OC8R = 4000 - (1.75 * 200); // Initial duty cycle (~1.75 ms pulse)
        OC8RS = 4000 - (1.75 * 200); // Next PWM duty cycle
        OC8CON = 0x0006;             // OC8 in PWM mode, no fault check, Timer2 as clock source
    }
    else if (servo_number == 'Y') {
        // Setup OC7 for Y servo PWM output
        CLEARBIT(TRISDbits.TRISD6); // Set OC7 pin as output
        OC7R = 4000 - (1.5 * 200);  // Initial duty cycle (~1.5 ms pulse)
        OC7RS = 4000 - (1.5 * 200); // Next PWM duty cycle
        OC7CON = 0x0006;            // OC7 in PWM mode, no fault check, Timer2 as clock source
    }

    SETBIT(T2CONbits.TON);  // Enable Timer2
}

/**
 * @brief Set the PWM duty cycle for the specified servo
 * @param servo_number 'X' or 'Y'
 * @param duty_cycle Duty cycle in milliseconds (e.g., 1.5 for 1.5 ms pulse)
 */
void set_duty_cycle(char servo_number, float duty_cycle) {
    if (servo_number == 'X') {
        OC8RS = 4000 - (duty_cycle * 200);
    }
    else if (servo_number == 'Y') {
        OC7RS = 4000 - (duty_cycle * 200);
    }
}

/**
 * @brief Initialize the touchscreen hardware and ADC
 */
void touchscreen_init() {
    CLEARBIT(AD1CON1bits.ADON);  // Disable ADC before configuration

    // Configure touchscreen input pins as analog inputs
    SETBIT(TRISBbits.TRISB15);    // Set RB15 as input (AN15)
    CLEARBIT(AD1PCFGLbits.PCFG15); // Configure RB15 as analog
    SETBIT(TRISBbits.TRISB9);     // Set RB9 as input (AN9)
    CLEARBIT(AD1PCFGLbits.PCFG9);  // Configure RB9 as analog

    // ADC Configuration
    CLEARBIT(AD1CON1bits.AD12B);  // 10-bit operation mode
    AD1CON1bits.FORM = 0;         // Integer output format
    AD1CON1bits.SSRC = 0x7;       // Auto-conversion mode (auto start conversion)
    
    AD1CON2 = 0;                  // No scanning, use MUX A only
    
    CLEARBIT(AD1CON3bits.ADRC);   // Use internal clock
    AD1CON3bits.SAMC = 0x1F;      // Sample time = 31 Tad
    AD1CON3bits.ADCS = 0x2;       // Tad = 3*Tcy (instruction cycles)

    // Enable ADC
    SETBIT(AD1CON1bits.ADON);

    // Configure PORTE pins E1, E2, E3 as outputs for touchscreen control
    CLEARBIT(TRISEbits.TRISE1);
    CLEARBIT(TRISEbits.TRISE2);
    CLEARBIT(TRISEbits.TRISE3);

    // Set initial touchscreen control pins to standby mode
    SETBIT(PORTEbits.RE1);
    SETBIT(PORTEbits.RE2);
    CLEARBIT(PORTEbits.RE3);
}

/**
 * @brief Set the touchscreen measurement dimension (X or Y)
 * @param dimension 'X' or 'Y'
 */
void touchscreen_dimension(char dimension) {
    if (dimension == 'X') {
        CLEARBIT(PORTEbits.RE1);
        Nop();
        SETBIT(PORTEbits.RE2);
        Nop();
        SETBIT(PORTEbits.RE3);
        Nop();
    }
    else if (dimension == 'Y') {
        SETBIT(PORTEbits.RE1);
        Nop();
        CLEARBIT(PORTEbits.RE2);
        Nop();
        CLEARBIT(PORTEbits.RE3);
        Nop();
    }
}

/**
 * @brief Read the touchscreen ADC value for the current dimension
 * @return ADC reading (0 - 1023)
 */
double touchscreen_read() {
   

    if (dimensionB == 'X') {
        AD1CHS0bits.CH0SA = 0x0F; // Select AN15 for X dimension reading
    }
    else if (dimensionB == 'Y') {
        AD1CHS0bits.CH0SA = 0x09; // Select AN9 for Y dimension reading
    }

    
    
    SETBIT(AD1CON1bits.SAMP);  // Start sampling
    AD1CON1bits.SAMP = 0;      // Start conversion
    
    while (!AD1CON1bits.DONE); // Wait until conversion completes
    CLEARBIT(AD1CON1bits.DONE); // Clear the done flag
    
    return ADC1BUF0;           // Return ADC value (0-1023)
}


/*
 * PD Controller
 */
float Kp = 0.9f;
float Kd = 0.5f;

float prev_x_error = 0, prev_y_error = 0;

float compute_pd(float target, float actual, float* prev_error) {
    float error = target - actual;
    float d_error = (error - *prev_error) / (20 / 1000.0f);
    *prev_error = error;

    return Kp * error + Kd * d_error;
}

/*
 * Clamping and Mapping Functions
*/

#define SERVO_CENTER 1.5f
#define SERVO_MIN 0.7f
#define SERVO_MAX 2.2f
#define MAX_U 700.0f

float clamp(float val, float min, float max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

float map_pd_to_pwm(float u) {
    u = clamp(u, -MAX_U, MAX_U);
    float scale = (SERVO_MAX - SERVO_CENTER) / MAX_U;
    return clamp(SERVO_CENTER + scale * u, SERVO_MIN, SERVO_MAX);
}

/*
 * Update Servos
*/
float x_pwm = 0.00f;
float y_pwm = 0.00f;
void update_servos(float x_pwm, float y_pwm) {
    set_duty_cycle('X',x_pwm);  // tilt in X
    set_duty_cycle('Y',y_pwm);   // tilt in Y
}

/*
 * Butterworth Filter N=1, Cutoff 3 Hz, sampling @ 50 Hz
 */
#define ALPHA 0.1586f
void update_filter(uint16_t x_raw, uint16_t y_raw, bool is_x) {
    if (is_x)
        x_filtered = (ALPHA * x_raw + (1.0f - ALPHA) * x_filtered);
    else
        y_filtered = (ALPHA * y_raw + (1.0f - ALPHA) * y_filtered);
}



/*
 * main loop
 */
void main_loop()
{
    // print assignment information
    lcd_printf("Lab06: Amazing Ball");
    lcd_locate(0, 1);
    lcd_printf("Group: Group19");
    lcd_locate(0, 2);
    
    // Initialize servos and touchscreen
    servo_init('X');
    servo_init('Y');
    touchscreen_init();
    initialize_timer1();
        


    while (TRUE)
    {
        if (task_ready)  // Only do something if 10 ms have passed
        {
            switch (counter)
            {
                case 0:
                    // Set dimension to Y — don't read yet
                    touchscreen_dimension('X');
                    dimensionB = 'X';
                    x_raw = touchscreen_read();
                    touchscreen_dimension('Y');
                    break;

                case 1:
                    // Now 10ms have passed, read Y
                    dimensionB = 'Y';
                    y_raw = touchscreen_read();
                    // Set dimension to X for next read
                    touchscreen_dimension('X');
                    break;

                case 2:
                    
                    // Apply filters
                    update_filter(x_raw, y_raw, true);
                    update_filter(x_raw, y_raw, false);

                    // Compute target positions
                    float x_target = center_x + CIRCLE_RADIUS * cosf(CIRCLE_SPEED * t_seconds);
                    float y_target = center_y + CIRCLE_RADIUS * sinf(CIRCLE_SPEED * t_seconds);

                    // Compute PD
                    float pd_x = compute_pd(x_target, x_filtered, &prev_x_error);
                    float pd_y = compute_pd(y_target, y_filtered, &prev_y_error);

                    // Map to PWM
                    x_pwm = map_pd_to_pwm(pd_x);
                    y_pwm = map_pd_to_pwm(pd_y);

                    // Update servos
                    update_servos(x_pwm, y_pwm);
                    // Read touchscreen X again
                    dimensionB = 'X';
                    x_raw = touchscreen_read();

                    touchscreen_dimension('Y');

                    t_seconds += 0.02f;
                    break;
            }

            task_ready = false; // Don't run anything else until next interrupt
        }
    }


       
        
}
