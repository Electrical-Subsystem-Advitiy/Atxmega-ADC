/*
 * ADC FROM AVR1505.cpp
 *
 * Created: 14-06-2018 23:16:17
 * Author : Neilabh
 */ 


/*
* --- NOTES ---
* This is operating on ATXMEGA128A3U with Voltage 3.3 Volts.

* This code converts analogue to 12 bit digital.
* But only 11 bits of these are actually available.
* The last bit is for sign.
* This is because here, we have chosen signed mode.
* Unsigned mode can be chosen by changing the 2nd entry of the ADC_ConvMode_and_Resolution_Config function in main.c to false.
* Then, the program measures only positive values, but with double the resolution.
* So, max Value is 2047.
* We have divided this by 4 to get a 8 but reading onto the USART as well as LED.
* Note that the precision to be taken depends on the fluctuation of voltage.
* In this case, the voltage source used had a fluctuation of 2 bits.
* (scientiFic Dual Tracking Power Supply PSD3204 - 30 V / 3 A)

* Note the _delay_ms(100) in the code can be used if required to make the data more manageable.
* As of now, it has been commented out.

* This code shows max reading, that is 1111 1111 or FF at 2.02 Volts.
* This code shows min reading, that is 0000 0001 or 01 at 0.0015 Volts.
** (The Precision Difference is due to values being measured across different modes on multimeter)

* Negative terminal is connected to Ground.
* Positive terminal is connected to PORT B PIN 1.
** This is because we have used single ended ext input.
** Otherwise, Negative Terminal is connected to PIN0 as default.
* Instead of this, this can be shifted to PORT A PIN 1.
** (And corresponding PORT A PIN 0)
* For this, change ADCB to ADCA in all instances in main.c
* To change this, change MUXNEG & MUXPOS in ADCx_ChN_MUXCTRL register.
** x is PORT, A or B; N is Ch number, 0, 1, 2, or 3.

* Channel 0 or Ch0 has been used in this code.
* In case more channels are needed, 4 channels are available.
* For that, change Ch0 in main.c to Ch1, Ch2, OR Ch3 as required.

* 4 I/P modes available:
** Single Ended External I/P - used here.
** Internal I/P.
** Differential I/P w/o gain.
** Differential I/P with gain
* These can be assigned by assigning ADC_CH_INPUTMODE_<specify input mode>_gc in main.c.


* For USART, PORT C PIN 2 is RX.
* For USART, PORT C PIN 3 IS TX.

* ADC clock here has been specified here to be Peripheral Clock / 8. This gives the max. sample rate.
* To change the ADC clock, change the function ADC_Prescaler_Config in main.c accordingly.
* ADC clock determines the conversion rate.

* 5 options are available for selecting the reference voltage:
** Fixed internal voltage of 1V - ADC_REFSEL_INT1V_gc
*** Not recommended as too low to give a good resolution.
** Internal Voltage Reference of VCC / 1.6 - ADC_REFSEL_INTVCC_gc
** Internal Voltage Reference of VCC / 2 - ADC_REFSEL_INTVCC2_gc
** External Reference on PORT A - ADC_REFSEL_AREFA_gc
** External Reference on PORT B - ADC_REFSEL_AREFB_gc

* Here, ADC_REFSEL_INTVCC_gc has been used, which is 3.3 / 1.6 = 2.09 Volts.
* This is the maximum range possible without using external reference.

* Resolution can be changed between 8 bit and 12 bit via 3rd entry in ADC_ConvMode_and_Resolution_Config function in main.c.
* Here, Resolution used is 12 bit. After taking out one bit for sign, the reamining 11 bits have bben scald down to give a 8 bit answer here, as mentioned above.



* This code has been taken from AVR1505 documentation, Task 1.
* AVR 1505 is available at http://www.microchip.com//wwwAppNotes/AppNotes.aspx?appnote=en591328
* PDF to AVR 1505 is available at http://ww1.microchip.com/downloads/en/AppNotes/doc8320.pdf
* This is available at http://ww1.microchip.com/downloads/en/AppNotes/AVR1505.zip
* The solution to tasks 2, 3 and 4 are available at https://github.com/fffaraz/Introduction-to-Microprocessors/tree/master/material/AVR/AVR1505
* These are also available at https://github.com/fffaraz/Introduction-to-Microprocessors/tree/master/material/AVR/AVR1505

*/

/*
* Resolution Check:
* 0111 1111 at 0.994 Volts
* 1000 0000 at 1.001 Volts
* 1111 1100 at 1.983 Volts
* 1111 1101 at 1.990 Volts
* 1111 1110 at 1.998 Volts
* 1111 1111 at 2.01 Volts (Precision loss due to change in multimeter readings at 2 Volts.)
*/

#include <avr/io.h>
#include "../adc_driver.h"

#define LEDPORT PORTF

volatile uint16_t ADC_result;

void USARTXInit(void)
{
	PORTC_DIRSET=PIN3_bm;                                           //Setting TX pin of USARTC0 as output (PC3)
	PORTC_DIRCLR=PIN2_bm;                                           //Setting RX pin of USARTC0 as input (PC2)
	USARTC0_BAUDCTRLA=0x0C;                                         //BSEL should be 12 in decimal for selection of baud rate 9600 with given freq
	USARTC0_BAUDCTRLB=0;                                            //BSCALE = 0
	USARTC0_CTRLC=(USART_CHSIZE_8BIT_gc)|(USART_PMODE_DISABLED_gc); //8 bit transmission in one data frame, no parity and one stop bit
	USARTC0_CTRLB=(USART_RXEN_bm)|(USART_TXEN_bm);                  //Enabling the transmitter and receiver channels of USARTC0
}

void USARTXTrans(uint8_t dat)
{
	while(!(USARTC0_STATUS & USART_DREIF_bm));                      //While data register empty interrupt flag is not high, wait
	USARTC0_DATA=dat;                                               //Writing the desired value into the transmit buffer register
}


int main( void )
{
	// Variable for use when we read the result from an ADC channel
	int8_t offset;

	LEDPORT.DIR = 0xFF; //Set as ouput, the 8 MSB of the result is output here
	LEDPORT.OUT = 0x00; //Default off for LED

	//PORTQ.PIN2CTRL = (PORTQ.PIN2CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLDOWN_gc;	// This pin must be grounded to "enable" NTC-resistor

	/* Move stored calibration values to ADC B */
	ADC_CalibrationValues_Load(&ADCB);

	/* Set up ADC B to have signed conversion mode and 12 bit resolution. */
	ADC_ConvMode_and_Resolution_Config(&ADCB, true, ADC_RESOLUTION_12BIT_gc);

	// The ADC has different voltage reference options, controlled by the REFSEL bits in the
	// REFCTRL register. Here the internal reference is selected
	ADC_Reference_Config(&ADCB, ADC_REFSEL_INTVCC_gc);

	// The clock into the ADC decide the maximum sample rate and the conversion time, and
	// this is controlled by the PRESCALER bits in the PRESCALER register. Here, the
	// Peripheral Clock is divided by 8 ( gives 250 KSPS with 2Mhz clock )
	ADC_Prescaler_Config(&ADCB, ADC_PRESCALER_DIV8_gc);

	// The used Virtual Channel (CH0) must be set in the correct mode
	// In this task we will use single ended input, so this mode is selected

	/* Setup channel 0 to have single ended input. */
	
	ADC_Ch_InputMode_and_Gain_Config(&ADCB.CH0,	ADC_CH_INPUTMODE_SINGLEENDED_gc, ADC_CH_GAIN_1X_gc); 
	// When changing gain, if required, when using differential, change here, as well as in adc_driver.h, labeled as "In case of changing gain, change the gain value here.".


	// Setting up the which pins to convert.
	// Note that the negative pin is internally connected to ground
	ADC_Ch_InputMux_Config(&ADCB.CH0, ADC_CH_MUXPOS_PIN1_gc, ADC_CH_MUXNEG_PIN0_gc);


	// Before the ADC can be used it must be enabled
	ADC_Enable(&ADCB);


	// Wait until the ADC is ready
	ADC_Wait_8MHz(&ADCB);

	// In the while(1) loop, a conversion is started on CH0 and the 8 MSB of the result is
	// output on the LEDPORT when the conversion is done

	/* Get offset value for ADC B.  */
	offset = ADC_Offset_Get_Signed(&ADCB, &(ADCB.CH0), true);
	
	USARTXInit();
	
	while (1) {
		// Start a single conversion
		ADC_Ch_Conversion_Start(&ADCB.CH0);

		while(!ADC_Ch_Conversion_Complete(&ADCB.CH0));

		//ADCB.INTFLAGS = ADC_CH0IF_bm;     // Clear CH0IF by writing a one to it
		ADC_result =   ADC_ResultCh_GetWord_Signed(&ADCB.CH0, offset);
		// Max result is 2047, divide by 4 to get a byte value;
		ADC_result = ((ADC_result - 1) / 8);
		// invert as the LED is on when pin is low
		LEDPORT.OUT = ADC_result;
		
		USARTXTrans(ADC_result);
		//_delay_ms(100);
	}
}



