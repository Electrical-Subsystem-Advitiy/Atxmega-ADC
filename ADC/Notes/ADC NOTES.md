## NOTES 
* This is operating on ATXMEGA128A3U with Voltage 3.3 Volts.

* For more details, refer to https://github.com/Electrical-Subsystem-Advitiy/Atxmega-ADC/blob/master/ADC/ADC%20SELECTION%20GUIDE%20-%20doc8378.pdf

#### Conversion Mode And Resolution:
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

#### Delay:
* Note the _delay_ms(100) in the code can be used if required to make the data more manageable.
* As of now, it has been commented out.

#### Max and Min Readings:
* This code shows max reading, that is 1111 1111 or FF at 2.02 Volts.
* This code shows min reading, that is 0000 0001 or 01 at 0.0015 Volts. \
  o (The Precision Difference is due to values being measured across different modes on multimeter)

#### Connections:
* Negative terminal is connected to Ground.
* Positive terminal is connected to PORT B PIN 1. \
  o This is because we have used single ended ext input. \
  o Otherwise, Negative Terminal is connected to PIN0 as default.
* Instead of this, this can be shifted to PORT A PIN 1. \
  o  (And corresponding PORT A PIN 0)
* For this, change ADCB to ADCA in all instances in main.c
* To change this, change MUXNEG & MUXPOS in ADCx_ChN_MUXCTRL register. \
  o x is PORT, A or B; N is Ch number, 0, 1, 2, or 3.

#### Virtual Channels:
* Channel 0 or Ch0 has been used in this code.
* In case more channels are needed, 4 channels are available.
* For that, change Ch0 in main.c to Ch1, Ch2, OR Ch3 as required.

#### Input Modes:
* 4 I/P modes available: \
  o Single Ended External I/P - used here. \
  o Internal I/P. \
  o Differential I/P w/o gain. \
  o Differential I/P with gain
* These can be assigned by assigning ADC_CH_INPUTMODE_<specify input mode>_gc in main.c.

#### USART Connections:
* For USART, PORT C PIN 2 is RX.
* For USART, PORT C PIN 3 IS TX.

#### ADC Clock:
* ADC clock here has been specified here to be Peripheral Clock / 8. This gives the max. sample rate.
* To change the ADC clock, change the function ADC_Prescaler_Config in main.c accordingly.
* ADC clock determines the conversion rate.

#### Reference Voltage:
* 5 options are available for selecting the reference voltage: \
  o Fixed internal voltage of 1V - ADC_REFSEL_INT1V_gc \
  o Not recommended as too low to give a good resolution. \
  o Internal Voltage Reference of VCC / 1.6 - ADC_REFSEL_INTVCC_gc \
  o Internal Voltage Reference of VCC / 2 - ADC_REFSEL_INTVCC2_gc \
  o External Reference on PORT A - ADC_REFSEL_AREFA_gc \
  o External Reference on PORT B - ADC_REFSEL_AREFB_gc

* Here, ADC_REFSEL_INTVCC_gc has been used, which is 3.3 / 1.6 = 2.09 Volts.
* This is the maximum range possible without using external reference.

#### Resolution:
* Resolution can be changed between 8 bit and 12 bit via 3rd entry in ADC_ConvMode_and_Resolution_Config function in main.c.
* Here, Resolution used is 12 bit. After taking out one bit for sign, the reamining 11 bits have bben scald down to give a 8 bit answer here, as mentioned above.


#### References:
* This code has been taken from AVR1505 documentation, Task 1.
* AVR 1505 is available at http://www.microchip.com//wwwAppNotes/AppNotes.aspx?appnote=en591328
* PDF to AVR 1505 is available at http://ww1.microchip.com/downloads/en/AppNotes/doc8320.pdf
* This is available at http://ww1.microchip.com/downloads/en/AppNotes/AVR1505.zip
* The solution to tasks 2, 3 and 4 are available at https://github.com/fffaraz/Introduction-to-Microprocessors/tree/master/material/AVR/AVR1505
* These are also available at https://github.com/fffaraz/Introduction-to-Microprocessors/tree/master/material/AVR/AVR1505


#### Resolution Check:
* 0111 1111 at 0.994 Volts
* 1000 0000 at 1.001 Volts
* 1111 1100 at 1.983 Volts
* 1111 1101 at 1.990 Volts
* 1111 1110 at 1.998 Volts
* 1111 1111 at 2.01 Volts (Precision loss due to change in multimeter readings at 2 Volts.)

- (This is for PtX-128 at Satellite Lab, IIT Bombay)

- By Neilabh

