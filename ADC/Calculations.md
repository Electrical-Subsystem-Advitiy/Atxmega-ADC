## I have the following calculations:

#### While checking the resolution, I noticed the following trend.
#### The Voltage readings are from a multimeter.

#### Error Check:
* 0111 1111 at 0.994 Volts (127/255 == 1.027 Volts) (Error - 3.22% or 0.031 Volts)
* 1000 0000 at 1.001 Volts (128/255 == 1.034 Volts) (Error - 3.21% or 0.33 Volts)
* 1111 1100 at 1.983 Volts (252/255 == 2.036 Volts) (Error - 2.60% or 0.53 Volts)
* 1111 1101 at 1.990 Volts (253/255 == 2.044 Volts) (Error - 2.64% or 0.54 Volts)
* 1111 1110 at 1.998 Volts (254/255 == 2.052 Volts) (Error - 2.63% or 0.54 Volts)
* 1111 1111 at 2.01 Volts  (255/255 == 2.06 Volts) (Error - 2.43% or 0.5 Volts)
  - (Precision loss due to change in multimeter readings at 2 Volts.) 

##### As can be seen, the readings have an error of 2.4% to 3.2% in the readings from theoretical readings. These need to be kept in mind while using this code.
