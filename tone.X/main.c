/*
 * File:  tone.X/main.c
 * Function:
 *      - Plays Sine() or chainsaw pattern /|/|/| on Codec output at 250 Hz
 *      - Use switch S1 to mute/unmute audio (muted Audio signaled with
 *        Amber LED)
 *      - Use switch S2 to change pattern (default Sine alternative Chainsaw)
 *
 *      This demo was created from  Microchip example:
 *      "SASK WM8510 Record Play Demo for SST.zip"
 *      Code in this main.c is based on sask/src/main.c
 *      Using latest MPLAB X IDE v6.15 and XC16 v2.10 (original was MPLAB + C30)
 *
 * Created on September 30, 2023
 */

// example system.h - main include file
#include "ex_system.h"
#include <libq.h> // sinPI(Q15)
#include <stdbool.h> // bool, true, false

// for sample rate 8 000 Hz, we should produce 250 Hz chainsaw pattern /|/|/|
#define CHAINSAW_INCREMENT 2048
// 250 Hz for Sine
#define SINE_INCREMENT 2048
#define FRAME_SIZE 				128			/* Each audio frame will have these many samples	*/

/* Allocate memory for buffers and drivers	*/
int 		codecBuffer		[WM8510DRV_DRV_BUFFER_SIZE];
int 		samples			[FRAME_SIZE];
int 		decodedSamples	[FRAME_SIZE];

/* Instantiate the drivers 	*/
WM8510Handle codec;

/* Create the driver handles	*/
WM8510Handle * codecHandle = &codec;

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
    uint16_t i;
    int chainsaw=0;
    _Q15 qAnglePi=0; // angle times Pi
    bool bSineWaveform = true;
    
    CLOCK_Initialize();
    /* Initialize the board and the drivers	*/
	SASKInit();
	WM8510Init(codecHandle,codecBuffer);
	/* Start Audio input and output function	*/
	WM8510Start(codecHandle);
	/* Configure codec for 8K operation	*/
	WM8510SampleRate8KConfig(codecHandle);
    
    while(1){
			/* Obtaining the ADC samples	*/
			while(WM8510IsReadBusy(codecHandle));
			WM8510Read(codecHandle,samples,FRAME_SIZE);
            // we actually ignore these read "samples" in our project...

            qAnglePi = -32768; // = -1.0000 in Q1.15
            chainsaw = -32768; // minimum signed integer value
            for(i=0;i!=FRAME_SIZE;i++){
                if (bSineWaveform){
                    // sin() pattern using very fast Q1.15 version
                    decodedSamples[i] = _Q15sinPI(qAnglePi);
                    qAnglePi += SINE_INCREMENT;
                } else {
                    // fill trivial chainsaw pattern /|/|/|
                    decodedSamples[i] = chainsaw;
                    chainsaw += CHAINSAW_INCREMENT;
                }
            }
            
  			/* Wait till the codec is available for a new  frame	*/
			while(WM8510IsWriteBusy(codecHandle));	
		
			/* Write the frame to the output	*/
			WM8510Write (codecHandle,decodedSamples,FRAME_SIZE);

   			if(CheckSwitchS1() == 1)
			{
                int commandValue;
                
                YELLOW_LED ^= 1;
                // press S1 to mute/unmute audio
                if (YELLOW_LED){
                    commandValue = WM8510DRV_MAX_VOLUME; // max volume without clipping
                } else {
                    commandValue = 1; // mute
                }
				WM8510IOCtl(codecHandle,WM8510_DAC_VOLUME,	 (void *) &commandValue);
            }
            if (CheckSwitchS2()){
                bSineWaveform = !bSineWaveform;
            }
    }
    return 0;
}
