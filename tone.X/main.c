/*
 * File:  tone.X/main.c
 *      Attempt to create simple output tone demo based on
 *      top of Microchips "SASK WM8510 Record Play Demo for SST.zip"
 *      with latest MPLAB X IDE v6.15 and XC16 v2.10
 *
 * Created on September 30, 2023
 */

// example system.h - main include file
#include "ex_system.h"


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
