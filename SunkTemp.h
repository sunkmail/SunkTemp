
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
		
		// Reset the device - stops any current temp measurments
		void reset(void);

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
		
		void setTempHighAlarm(int8_t tempHighAlarm);
		void setTempLowAlarm(int8_t tempLowAlarm);
		int8_t getTempHighAlarm(void);
		int8_t getTempLowAlarm(void);
		
		int8_t showscratchpadUser1(void);
		int8_t showscratchpadUser2(void);
		
		void setUserByte1(int8_t user1);
		void setUserByte2(int8_t user2);
		void setUserBytes(int8_t user1, int8_t user2);
		void setUserDataInt(int16_t userInteger);		// Set user data as a single 16-bit integer
		
		int16_t getUserDataInt(void);
		int8_t	getUserByte1(void);
		int8_t	getUserByte2(void);

		void pullEEPROM(void);
		void pushEEPROM(void);
	
	protected:
	private:
		// Working Array for dealing with the Scratchpad
		uint8_t	scratchpad[9];
		
		// Get the entire Scratchpad from the device
		void pullScratchpad(uint8_t scratchpad[9]);
		
		// write Scratchpad variable onto device scratchpad 
		void pushScratchpad(uint8_t scratchpad[9]);		
		
		OneWire _oneWire;
		
		uint8_t _rawData[2];
		
		// Data converted to human readable value ±x.yyyy with 0.0625 max resolution
		int32_t _temp;

		// Get the Raw Temperature readings from sensor
		void getRawData(void);
		
		void convertData(void);


};

#endif	// SunkTemp_H
