//	Created in VS Code (from user Template) on:  2019-08-26
//	Created by: sunkm

// ** LIbrary for Controlling a SINGLE DS18B20 Temp Sensor using one.wire library
//  Similar to other libraries, but smaller footprint when used on ATTINY uC's  

#include <arduino.h>
#include <OneWire.h>
#include <SunkTemp.h>

// Constructor
SunkTemp::SunkTemp(OneWire owObject)
: _oneWire(owObject) 
{    
}

void SunkTemp::begin(byte setRes){

    // Set resolution ... should be correct by default, but just in case. :)
    setResolution(setRes);
}

void SunkTemp::setResolution(byte res){
    // Set resolution ... should be correct by default, but just in case. :)
    _oneWire.write(0xCC); // Bypass ROM command - since only 1 sensor used
    _oneWire.write(0x4E); // Write to scratchpad command
    _oneWire.write(0);    // Write Dummy value to alarm1 register
    _oneWire.write(100);  // Write Dummy value to alarm2 register
    switch(res){
        case 9:
        {
            _oneWire.write(0x1F); // set resolution to 9
            break;
        }
        case 10:
        {
            _oneWire.write(0x3F); // set resolution to 10
            break;
        }
        case 11:
        {
            _oneWire.write(0x5F); // set resolution to 11
            break;
        }
        case 12:
        {
            _oneWire.write(0x7F); // set resolution to 12
            break;
        }
        };

        delay(10);
        _oneWire.reset();

}

void SunkTemp::requestTemp(void){
    _oneWire.reset();
    _oneWire.write(0xCC);        // Bypass ROM command - since only 1 sensor used
    _oneWire.write(0x44);        // Request Temp Conversion
}

// Get the Raw Temperature readings from sensor
void SunkTemp::getRawData(void){
    _oneWire.reset();
    _oneWire.write(0xCC);           // Bypass ROM command - since only 1 sensor used
    _oneWire.write(0xBE);           // Request Read Scratchpad
    _rawData[0] = _oneWire.read();
    _rawData[1] = _oneWire.read();
    _oneWire.reset();               // Send Reset to stop sending or rest of scratchpad
}

// Get raw data and return to 2-byte array passed into function
void SunkTemp::returnRawTemp(byte (&_rawTemp)[2]){
    getRawData();
    _rawTemp[0] = _rawData[0];
    _rawTemp[1] = _rawData[1];
}

// Get & Return the Raw Temp as a single 16-bit integer (MSB, LSB)
int16_t SunkTemp::returnRawTempInteger(){
	getRawData();
	return ((_rawData[0] << 8) || (_rawData[1]));
}

// Get & Convert raw data to human readable value ±x.yyyy with 0.0625 max resolution
void SunkTemp::convertData(void){
    // Containers for math
    signed long _working1;
    signed long _working2;
    // Serial.begin(9600);

    getRawData();
	
    _working1 = ((int(_rawData[1]) << 8) | _rawData[0]); // combine 2 byte value to single int
    // Serial.print("Combined Raw Data: ");
    // Serial.println(_working1);
    
    // strip out temperature value of whole degrees
    // Last 4 bits are decimals     // working1 (initial): SSSS Sxxx xxxx xxxx
    _working2 = (_working1 >> 4);   // working2 (Shifted): 0000 SSSS Sxxx xxxx
    // Serial.print("Shifted Whole: ");
    // Serial.println(_working2);
    // // Make 'new' bits the same as the Sign bit, if negative
     // if(_working2 >= 0x0F00){        // 0000 SSSS Sxxx xxxx, ie IF the 'S's iare 1
		// _working2 |= 0xF000;		// Make the new value 1111 Sxxx xxxx 
     // }
    
    _working1 = (_working1 & 0x000F);   // Mask off to get just the decimal portion
    // Serial.print("Masked Decimal Raw: ");
    // Serial.println(_working1);
    _working1 *= 625;                   // Set _working1 to actual value in 0.xxxx degrees
    // Serial.print("Converted Decimal Data: ");
    // Serial.println(_working1);


    _temp = (_working2 * 10000);      // Whole degree portion, signed
    // Serial.print("converted Whole: ");
    // Serial.println(_temp);
    _temp += _working1;             // Add decimal portion

    // Serial.print("Combined Converted Data: ");
    // Serial.println(_temp);

}


int32_t SunkTemp::getTemp(uint8_t decimalPlaces){

    int32_t returnValue;
    uint16_t multipier = 1;

    convertData();

    switch (decimalPlaces)
    {
        case 3:
                multipier = 1;
                break;
        case 2:
                multipier = 10;
                break;
        case 1:
                multipier = 100;
                break;
        case 0:
                multipier = 1000;
                break;
    }
    returnValue = _temp/(10*multipier);
    // Round up/Down as needed
    if(_temp%(10*multipier) >= (5*multipier)){
        returnValue++;
    }
    else if(_temp%(10*multipier) <= (-5*multipier)){
        returnValue--;
    }

    return returnValue;

}

