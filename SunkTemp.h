
//  Currently Library does NOT check CRC

#include <arduino.h>
#include <OneWire.h>

#ifndef SunkTemp_H
#define SunkTemp_H


class SunkTemp
{
	public:
		// Constructor - Pass HW Pin for 1-Wire bus
		SunkTemp(OneWire owObject);

		// begin - Set resolution if variable passed, Default: 12
		void begin(uint8_t res = 12);

		//  Set Resolution bit depth on Sensor. Default: 12 (Valid Range: 9 - 12)
		void setResolution(uint8_t setRes = 12);

		//  Send commands to start Temp conversion
		void requestTemp(void);

		// Get & Return the Raw Temp to individual Bytes in user defined array
		void returnRawTemp(uint8_t (&_rawTemp)[2]);
		
		// Get & Return the Raw Temp as a single 16-bit integer (MSB, LSB)
		int16_t returnRawTempInteger(void);

		// Get & Convert new Raw data and return human-readable integer with specified # of decimal places 
		int32_t getTemp(uint8_t decimalPlaces = 3);

	
	protected:
	private:
		OneWire _oneWire;
		byte _rawData[2];
		// Data converted to human readable value ±x.yyyy with 0.0625 max resolution
		signed long _temp;

		// Get the Raw Temperature readings from sensor
		void getRawData(void);
		
		void convertData(void);
};

#endif	// SunkTemp_H