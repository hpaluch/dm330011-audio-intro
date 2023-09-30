/*
 * File:   swleds.X/main.c
 *
 *         Basic Switches and LED program to test DM330011 board
 *         with latest MPLAB X IDE v6.15 and XC16 v2.10
 * Created on September 30, 2023, 11:01 AM
 */

// Chip configuration - must be before includes
/* 
 From SASK-WM8510-Record-Play-Demo-for-SST.zip 
 _FGS(GWRP_OFF & GCP_OFF);
_FOSCSEL(FNOSC_FRC);
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);
_FWDT(FWDTEN_OFF);
*/

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
#pragma config RBS = NO_RAM             // Boot Segment RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WRPROTECT_OFF     // Secure Segment Program Write Protect (Secure Segment may be written)
#pragma config SSS = NO_FLASH           // Secure Segment Program Flash Code Protection (No Secure Segment)
#pragma config RSS = NO_RAM             // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Mode (Internal Fast RC (FRC))
#pragma config IESO = OFF               // Two-speed Oscillator Start-Up Enable (Start up with user-selected oscillator)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Source (Primary Oscillator Disabled)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function (OSC2 pin has digital I/O function)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define FCY 40000000UL
#include "xc.h"
#include <libpic30.h>  // __delay_us()) and __delay_ms())

// modified from sask.h

/* The LEDS and their ports	*/
#define YELLOW_LED_TRIS		_TRISC13
#define GREEN_LED_TRIS		_TRISC14
#define RED_LED_TRIS		_TRISC15

#define YELLOW_LED		_LATC13
#define GREEN_LED			_LATC14
#define RED_LED		_LATC15

/* The Switches and their ports */

#define SWITCH_S1_TRIS	_TRISD8
#define SWITCH_S2_TRIS	_TRISD9

#define SWITCH_S1	_RD8
#define SWITCH_S2	_RD9

/* Volume control pins	*/
#define VOLUME_UPDN_TRIS	_TRISG0
#define VOLUME_CLK_TRIS		_TRISG1

#define VOLUME_UPDN	_LATG0
#define VOLUME_CLK	_LATG1

#define SWITCH_DEBOUNCE			12	/* Wait for this many times before switch press is valid		
									 * For this application, each increment is 
									 * (FRAME_SIZE/ADC_FSAMP) seconds */
#define SASK_LED_ON 	0
#define SASK_LED_OFF	1


// modified from SASK-WM8510-Record-Play-Demo-for-SST.zip demo code
void SASKInit(void)
{
 	/* Intialize the board LED and swicth ports	
	 * and turn all LEDS off 	*/
	 
	YELLOW_LED_TRIS	= 0;
	RED_LED_TRIS = 0;		
 	GREEN_LED_TRIS = 0;	

 	YELLOW_LED = SASK_LED_OFF;	
	RED_LED	= SASK_LED_OFF;		
 	GREEN_LED = SASK_LED_OFF;		

 	SWITCH_S1_TRIS = 1;	
 	SWITCH_S2_TRIS	= 1;
    
  	VOLUME_UPDN_TRIS = 0;	
	VOLUME_CLK_TRIS	= 0;
	
	VOLUME_UPDN = 0;
	VOLUME_CLK = 0;	

}

// initializes Fosc to 80 Mhz => Fcy = 40 Mhz/MIPS
// grabbed from SASK-WM8510-Record-Play-Demo-for-SST.zip demo code
void CLOCK_Initialize(void)
{
  	/* Configure Oscillator to operate the device at 40MHz.
	 * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	 * Fosc= 7.37M*40/(2*2)=80Mhz for 7.37M input clock */	 
	PLLFBD=41;				/* M=39	*/
	CLKDIVbits.PLLPOST=0;		/* N1=2	*/
	CLKDIVbits.PLLPRE=0;		/* N2=2	*/
	OSCTUN=0;			
	
	__builtin_write_OSCCONH(0x01);		/*	Initiate Clock Switch to FRC with PLL*/
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0b01);	/*	Wait for Clock switch to occur	*/
	while(!OSCCONbits.LOCK);

}

uint16_t gCounter=0;

int main(void) {
    CLOCK_Initialize();
    SASKInit();
    while(1){
        gCounter++;
        __delay_ms(10);
        if ( (gCounter&31) == 31){
            RED_LED ^= 1;
        }
        // both switches and LEDs use inverted logic, so we just
        // copy switch state to LED
        YELLOW_LED = SWITCH_S1;
        GREEN_LED = SWITCH_S2;
    }
    return 0;
}
