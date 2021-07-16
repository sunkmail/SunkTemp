
// ** LIbrary for Controlling a SINGLE DS18B20 Temp Sensor using one.wire library
//  Similar to other libraries, but smaller memory footprint - originally designed to use with ATTINY85

#include <arduino.h>
#include <OneWire.h>
#include <SunkTemp.h>

const uint8_t tempLSB = 0;
const uint8_t tempMSB = 1;
const uint8_t ThUser1 = 2;
const uint8_t TlUser2 = 3;
const uint8_t config = 4;
const uint8_t res_FFh = 5;
const uint8_t res = 6;
const uint8_t res_10h = 7;
const uint8_t crc = 8;

// uint8_t	scratchpad[9];


// Constructor
SunkTemp::SunkTemp(OneWire owObject)
: _oneWire(owObject) 
{    
}

void SunkTemp::begin(uint8_t setRes){

    // Set resolution ... should be correct by default, but just in case. :)
    setResolution(setRes);
}

void SunkTemp::reset(void){				// Reset the device - stops any current temp measurments
	_oneWire.reset();
}

void SunkTemp::setResolution(uint8_t res){
    // Set resolution ... should be correct by default, but just in case. :)
	pullScratchpad(scratchpad);	// Get existing scratchpad data
    switch(res){
        case 9:
        {
            scratchpad[config] = 0x1F; // set resolution to 9
            break;
        }
        case 10:
        {
            scratchpad[config] = 0x3F; // set resolution to 10
            break;
        }
        case 11:
        {
            scratchpad[config] = 0x5F; // set resolution to 11
            break;
        }
        case 12:
        {
            scratchpad[config] = 0x7F; // set resolution to 12
            break;
        }
    };
	pushScratchpad(scratchpad);			// Load new Data onto Device

}

void SunkTemp::requestTemp(void){
    _oneWire.reset();
    _oneWire.write(0xCC);        // Bypass ROM command - since only 1 sensor used
    _oneWire.write(0x44);        // Request Temp Conversion
}

void SunkTemp::setTempHighAlarm(int8_t tempHighAlarm){
    // Set the High Temperature alarm value
	pullScratchpad(scratchpad);	// Get existing scratchpad data
	scratchpad[ThUser1] = tempHighAlarm;
	pushScratchpad(scratchpad);			// Load new Data onto Device
}

void SunkTemp::setTempLowAlarm(int8_t tempLowAlarm){
    // Set the Low Temperature alarm value
	pullScratchpad(scratchpad);			// Get existing scratchpad data
	scratchpad[TlUser2] = tempLowAlarm;
	pushScratchpad(scratchpad);			// Load new Data onto Device
}

int8_t SunkTemp::getTempHighAlarm(void){
	pullScratchpad(scratchpad);	// Get existing scratchpad data
	return scratchpad[ThUser1];
}

int8_t SunkTemp::getTempLowAlarm(void){
	pullScratchpad(scratchpad);	// Get existing scratchpad data
	return scratchpad[TlUser2];
}

int8_t	SunkTemp::getUserByte1(void){
	return getTempHighAlarm();
}

int8_t	SunkTemp::getUserByte2(void){
	return getTempLowAlarm();	
}

int16_t	SunkTemp::getUserDataInt(void){
	pullScratchpad(scratchpad);	// Get existing scratchpad data
	int16_t returnValue;
	// returnValue = int((scratchpad[ThUser1] << 8) || (scratchpad[TlUser2]));
	returnValue = ((scratchpad[ThUser1] << 8) + scratchpad[TlUser2]);
	return returnValue;	
}


void SunkTemp::setUserDataInt(int16_t userData){
	pullScratchpad(scratchpad);					// Get existing scratchpad data
	scratchpad[TlUser2] = userData;				// Update user data bytes with new data
	scratchpad[ThUser1] = (userData >> 8);
	pushScratchpad(scratchpad);					// Load new Data onto Device
	
}

void SunkTemp::setUserByte1(int8_t user1){
	setTempHighAlarm(user1);					// User byte 1 IS the same byte as High Temp Alarm
}

void SunkTemp::setUserByte2(int8_t user2){
	setTempLowAlarm(user2);						// User byte 2 IS the same byte as Low Temp Alarm
}

void SunkTemp::setUserBytes(int8_t user1, int8_t user2){
	pullScratchpad(scratchpad);		// Get existing scratchpad data
	scratchpad[ThUser1] = user1;	// Update Variable
	scratchpad[TlUser2] = user2;	// Update Variable
	pushScratchpad(scratchpad);		// Load new Data onto Device
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

// Pull the entire scratchpad value
void SunkTemp::pullScratchpad(uint8_t scratchpad[9]){
	_oneWire.reset();
    _oneWire.write(0xCC);           // Bypass ROM command - since only 1 sensor used
    _oneWire.write(0xBE);           // Request Read Scratchpad
	for(uint8_t i = 0; i < 9; i++){
		scratchpad[i] = _oneWire.read();
	}
	_oneWire.reset();               // Send Reset at end of scratchpad
}

// Push the writable scratchpad values onto Device
void SunkTemp::pushScratchpad(uint8_t scratchpad[9]){
	_oneWire.write(0xCC); // Bypass ROM command - since only 1 sensor used
    _oneWire.write(0x4E); // Write to scratchpad command
    _oneWire.write(scratchpad[ThUser1]);   	// Write Scratchpad Temp High Alarm / User Data 1
    _oneWire.write(scratchpad[TlUser2]);  	// Write Scratchpad Temp Low Alarm / User Data 2
    _oneWire.write(scratchpad[config]);		// Write Scratchpad Config value
	
	delay(10);
    _oneWire.reset();
}

// Pull the EEPROM to the scratchpad
void SunkTemp::pullEEPROM(void){	// Done automatically during boot-up
	_oneWire.write(0xCC); 			// Bypass ROM command - since only 1 sensor used
	_oneWire.write(0xB8);           // Pull EEPROM Values and put on scratchpad
}

// Push the scratchpad to the EEPROM
void SunkTemp::pushEEPROM(void){
	_oneWire.write(0xCC); 			// Bypass ROM command - since only 1 sensor used
	_oneWire.write(0x48);           // Copy scratchpad values to EEPROM
}

// Get raw data and return to 2-byte array passed into function
void SunkTemp::returnRawTemp(uint8_t (&_rawTemp)[2]){
    getRawData();
    _rawTemp[0] = _rawData[0];
    _rawTemp[1] = _rawData[1];
}

// Get & Return the Raw Temp as a single 16-bit integer (MSB, LSB)
int16_t SunkTemp::returnRawTempInteger(){
	getRawData();
	int16_t returnValue;
	returnValue = ((_rawData[0] << 8) || (_rawData[1]));
	return returnValue;
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

