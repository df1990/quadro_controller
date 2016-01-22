// I2Cdev library collection - Main I2C device class
// Abstracts bit and byte I2C R/W functions into a convenient class
// 6/9/2012 by Jeff Rowberg <jeff@rowberg.net>
//
// Changelog:
//		2015-08-01 - Library code changed from cpp to c by Dariusz Fertyk
//      2013-05-06 - add Francesco Ferrara's Fastwire v0.24 implementation with small modifications
//      2013-05-05 - fix issue with writing bit values to words (Sasquatch/Farzanegan)
//      2012-06-09 - fix major issue with reading > 32 bytes at a time with Arduino Wire
//                 - add compiler warnings when using outdated or IDE or limited I2Cdev implementation
//      2011-11-01 - fix write*Bits mask calculation (thanks sasquatch @ Arduino forums)
//      2011-10-03 - added automatic Arduino version detection for ease of use
//      2011-10-02 - added Gene Knight's NBWire TwoWire class implementation with small modifications
//      2011-08-31 - added support for Arduino 1.0 Wire library (methods are different from 0.x)
//      2011-08-03 - added optional timeout parameter to read* methods to easily change from default
//      2011-08-02 - added support for 16-bit registers
//                 - fixed incorrect Doxygen comments on some methods
//                 - added timeout value for read operations (thanks mem @ Arduino forums)
//      2011-07-30 - changed read/write function structures to return success or byte counts
//                 - made all methods static for multi-device memory savings
//      2011-07-28 - initial release



#include "I2Cdev.h"
#include "Wire.h"
//#include "UART.h"



uint8_t min(uint8_t l, uint8_t r)
{
	return ((l < r) ? l : r);
}

void I2C_dev_init(void)
{
	TwoWire_begin();
}

/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
uint8_t I2Cdev_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) 
{
    uint8_t b;
    uint8_t count = I2Cdev_readByte(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read a single bit from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-15)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
uint8_t I2Cdev_readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data) 
{
    uint16_t b;
    uint8_t count = I2Cdev_readWord(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
uint8_t I2Cdev_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) 
{
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count;
	uint8_t b;
    if ((count = I2Cdev_readByte(devAddr, regAddr, &b)) != 0) 
	{
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return count;
}

/** Read multiple bits from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-15)
 * @param length Number of bits to read (not more than 16)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (1 = success, 0 = failure, -1 = timeout)
 */
uint8_t I2Cdev_readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data) 
{
    // 1101011001101001 read byte
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    //    010           masked
    //           -> 010 shifted
    uint8_t count;
    uint16_t w;
    if ((count = I2Cdev_readWord(devAddr, regAddr, &w)) != 0) 
	{
        uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        w &= mask;
        w >>= (bitStart - length + 1);
        *data = w;
    }
    return count;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
uint8_t I2Cdev_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data) 
{
    return I2Cdev_readBytes(devAddr, regAddr, 1, data);
}

/** Read single word from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for word value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
uint8_t I2Cdev_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data) 
{
    return I2Cdev_readWords(devAddr, regAddr, 1, data);
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (-1 indicates failure)
 */
uint8_t I2Cdev_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) 
{
    /*#ifdef I2CDEV_SERIAL_DEBUG
        UART_print_s("I2C (0x");
        UART_print_d8(devAddr, HEX);
        UART_print_s(") reading ");
        UART_print_d8(length, DEC);
        UART_print_s(" bytes from 0x");
        UART_print_d8(regAddr, HEX);
        UART_print_s("...");
    #endif*/

    uint8_t count = 0;
    //uint32_t t1 = millis();


		// Arduino v00xx (before v1.0), Wire library

		// I2C/TWI subsystem uses internal buffer that breaks with large data requests
		// so if user requests more than BUFFER_LENGTH bytes, we have to do it in
		// smaller chunks instead of all at once
		for (uint8_t k = 0; k < length; k += min(length, BUFFER_LENGTH)) 
		{
			TwoWire_beginTransmission(devAddr);
			TwoWire_send(regAddr);
			TwoWire_endTransmission();
			TwoWire_beginTransmission(devAddr);
			TwoWire_requestFrom(devAddr, (uint8_t)min(length - k, BUFFER_LENGTH));

			for (; TwoWire_available(); count++) 
			{
				data[count] = TwoWire_receive();
				#ifdef I2CDEV_SERIAL_DEBUG
					UART_print_d8(data[count], HEX);
					if (count + 1 < length) UART_print_s(" ");
				#endif
			}

			TwoWire_endTransmission();
		}
	#if 0
		// Arduino v1.0.0, Wire library
		// Adds standardized write() and read() stream methods instead of send() and receive()

		// I2C/TWI subsystem uses internal buffer that breaks with large data requests
		// so if user requests more than BUFFER_LENGTH bytes, we have to do it in
		// smaller chunks instead of all at once
		for (uint8_t k = 0; k < length; k += min(length, BUFFER_LENGTH)) {
			Wire.beginTransmission(devAddr);
			Wire.write(regAddr);
			Wire.endTransmission();
			Wire.beginTransmission(devAddr);
			Wire.requestFrom(devAddr, (uint8_t)min(length - k, BUFFER_LENGTH));
	
			for (; Wire.available() && (timeout == 0 || millis() - t1 < timeout); count++) {
				data[count] = Wire.read();
				#ifdef I2CDEV_SERIAL_DEBUG
					UART_print_d8(data[count], HEX);
					if (count + 1 < length) Serial.print(" ");
				#endif
			}
	
			Wire.endTransmission();
		}
	#elif 0
		// Arduino v1.0.1+, Wire library
		// Adds official support for repeated start condition, yay!

		// I2C/TWI subsystem uses internal buffer that breaks with large data requests
		// so if user requests more than BUFFER_LENGTH bytes, we have to do it in
		// smaller chunks instead of all at once
		for (uint8_t k = 0; k < length; k += min(length, BUFFER_LENGTH)) {
			Wire.beginTransmission(devAddr);
			Wire.write(regAddr);
			Wire.endTransmission();
			Wire.beginTransmission(devAddr);
			Wire.requestFrom(devAddr, (uint8_t)min(length - k, BUFFER_LENGTH));
	
			for (; Wire.available() && (timeout == 0 || millis() - t1 < timeout); count++) {
				data[count] = Wire.read();
				#ifdef I2CDEV_SERIAL_DEBUG
					UART_print_d8(data[count], HEX);
					if (count + 1 < length) UART_print_s(" ");
				#endif
			}
		}
	#endif

    

    // check for timeout
    //if (timeout > 0 && millis() - t1 >= timeout && count < length) count = -1; // timeout

    #ifdef I2CDEV_SERIAL_DEBUG
        UART_print_s(". Done (");
        UART_print_d8(count, DEC);
        UART_print_s(" read).");
		UART_println();
    #endif

    return count;
}

/** Read multiple words from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of words read (-1 indicates failure)
 */
uint8_t I2Cdev_readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data) 
{
    #ifdef I2CDEV_SERIAL_DEBUG
        UART_print_s("I2C (0x");
        UART_print_d8(devAddr, HEX);
        UART_print_s(") reading ");
        UART_print_d8(length, DEC);
        UART_print_s(" words from 0x");
        UART_print_d8(regAddr, HEX);
        UART_print_s("...");
    #endif

    uint8_t count = 0;
    //uint32_t t1 = millis();

    

        #if 1
            // Arduino v00xx (before v1.0), Wire library

            // I2C/TWI subsystem uses internal buffer that breaks with large data requests
            // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
            // smaller chunks instead of all at once
            for (uint8_t k = 0; k < length * 2; k += min(length * 2, BUFFER_LENGTH)) 
			{
                TwoWire_beginTransmission(devAddr);
                TwoWire_send(regAddr);
                TwoWire_endTransmission();
                TwoWire_beginTransmission(devAddr);
                TwoWire_requestFrom(devAddr, (uint8_t)(length * 2)); // length=words, this wants bytes
    
                uint8_t msb;
				msb	= 1; // starts with MSB, then LSB
                for (; TwoWire_available() && count < length ;) 
				{
                    if (msb) 
					{
                        // first byte is bits 15-8 (MSb=15)
                        data[count] = TwoWire_receive() << 8;
                    } else 
					{
                        // second byte is bits 7-0 (LSb=0)
                        data[count] |= TwoWire_receive();
                        #ifdef I2CDEV_SERIAL_DEBUG
                            UART_print_d8((uint8_t)(data[count] >> 8), HEX);
							UART_print_d8((uint8_t)(data[count]), HEX);
                            if (count + 1 < length) UART_print_s(" ");
                        #endif
                        count++;
                    }
                    msb = !msb;
                }

                TwoWire_endTransmission();
            }
        #elif 0
            // Arduino v1.0.0, Wire library
            // Adds standardized write() and read() stream methods instead of send() and receive()
    
            // I2C/TWI subsystem uses internal buffer that breaks with large data requests
            // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
            // smaller chunks instead of all at once
            for (uint8_t k = 0; k < length * 2; k += min(length * 2, BUFFER_LENGTH)) {
                Wire.beginTransmission(devAddr);
                Wire.write(regAddr);
                Wire.endTransmission();
                Wire.beginTransmission(devAddr);
                Wire.requestFrom(devAddr, (uint8_t)(length * 2)); // length=words, this wants bytes
    
                bool msb = true; // starts with MSB, then LSB
                for (; Wire.available() && count < length && (timeout == 0 || millis() - t1 < timeout);) {
                    if (msb) {
                        // first byte is bits 15-8 (MSb=15)
                        data[count] = Wire.read() << 8;
                    } else {
                        // second byte is bits 7-0 (LSb=0)
                        data[count] |= Wire.read();
                        #ifdef I2CDEV_SERIAL_DEBUG
                            Serial.print(data[count], HEX);
                            if (count + 1 < length) Serial.print(" ");
                        #endif
                        count++;
                    }
                    msb = !msb;
                }
        
                Wire.endTransmission();
            }
        #elif 0
            // Arduino v1.0.1+, Wire library
            // Adds official support for repeated start condition, yay!

            // I2C/TWI subsystem uses internal buffer that breaks with large data requests
            // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
            // smaller chunks instead of all at once
            for (uint8_t k = 0; k < length * 2; k += min(length * 2, BUFFER_LENGTH)) {
                Wire.beginTransmission(devAddr);
                Wire.write(regAddr);
                Wire.endTransmission();
                Wire.beginTransmission(devAddr);
                Wire.requestFrom(devAddr, (uint8_t)(length * 2)); // length=words, this wants bytes
        
                bool msb = true; // starts with MSB, then LSB
                for (; Wire.available() && count < length && (timeout == 0 || millis() - t1 < timeout);) {
                    if (msb) {
                        // first byte is bits 15-8 (MSb=15)
                        data[count] = Wire.read() << 8;
                    } else {
                        // second byte is bits 7-0 (LSb=0)
                        data[count] |= Wire.read();
                        #ifdef I2CDEV_SERIAL_DEBUG
                            Serial.print(data[count], HEX);
                            if (count + 1 < length) Serial.print(" ");
                        #endif
                        count++;
                    }
                    msb = !msb;
                }
        
                Wire.endTransmission();
            }
        #endif

    //if (timeout > 0 && millis() - t1 >= timeout && count < length) count = -1; // timeout

    #ifdef I2CDEV_SERIAL_DEBUG
        UART_print_s(". Done (");
        UART_print_d8(count, DEC);
        UART_print_s(" read).");
		UART_println();
    #endif
    
    return count;
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
uint8_t I2Cdev_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) 
{
    uint8_t b;
    I2Cdev_readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return I2Cdev_writeByte(devAddr, regAddr, b);
}

/** write a single bit in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-15)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
uint8_t I2Cdev_writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data) 
{
    uint16_t w;
    I2Cdev_readWord(devAddr, regAddr, &w);
    w = (data != 0) ? (w | (1 << bitNum)) : (w & ~(1 << bitNum));
    return I2Cdev_writeWord(devAddr, regAddr, w);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
uint8_t I2Cdev_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) 
{
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if (I2Cdev_readByte(devAddr, regAddr, &b) != 0) 
	{
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        return I2Cdev_writeByte(devAddr, regAddr, b);
    } 
	else 
	{
        return 0;
    }
}

/** Write multiple bits in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-15)
 * @param length Number of bits to write (not more than 16)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
uint8_t I2Cdev_writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data) 
{
    //              010 value to write
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    // 0001110000000000 mask word
    // 1010111110010110 original value (sample)
    // 1010001110010110 original & ~mask
    // 1010101110010110 masked | value
    uint16_t w;
    if (I2Cdev_readWord(devAddr, regAddr, &w) != 0) 
	{
        uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        w &= ~(mask); // zero all important bits in existing word
        w |= data; // combine data with existing word
        return I2Cdev_writeWord(devAddr, regAddr, w);
    } 
	else 
	{
        return 0;
    }
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
uint8_t I2Cdev_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) 
{
    return I2Cdev_writeBytes(devAddr, regAddr, 1, &data);
}

/** Write single word to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New word value to write
 * @return Status of operation (true = success)
 */
uint8_t I2Cdev_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) 
{
    return I2Cdev_writeWords(devAddr, regAddr, 1, &data);
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
uint8_t I2Cdev_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) 
{
    #ifdef I2CDEV_SERIAL_DEBUG
        UART_print_s("I2C (0x");
        UART_print_d8(devAddr, HEX);
        UART_print_s(") writing ");
        UART_print_d8(length, DEC);
        UART_print_s(" bytes to 0x");
        UART_print_d8(regAddr, HEX);
        UART_print_s("...");
    #endif
    uint8_t status = 0;
    #if 1
        TwoWire_beginTransmission(devAddr);
        TwoWire_send(regAddr); // send address
    #elif 0
        Wire.beginTransmission(devAddr);
        Wire.write((uint8_t) regAddr); // send address
    #elif 0
        Fastwire::beginTransmission(devAddr);
        Fastwire::write(regAddr);
    #endif
	
    for (uint8_t i = 0; i < length; i++) 
	{
        #ifdef I2CDEV_SERIAL_DEBUG
            UART_print_d8(data[i], HEX);
            if (i + 1 < length) UART_print_s(" ");
        #endif
        #if 1
            TwoWire_send(data[i]);
        #elif 0
            Wire.write((uint8_t) data[i]);
        #elif 0
            Fastwire::write((uint8_t) data[i]);
        #endif
    }
    #if 1
        status = TwoWire_endTransmission();
    #elif 0
        status = Wire.endTransmission();
    #elif 0
        Fastwire::stop();
        //status = Fastwire::endTransmission();
    #endif
    #ifdef I2CDEV_SERIAL_DEBUG
        UART_print_s(". Done.");
		UART_println();
    #endif
    if(0 == status)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
uint8_t I2Cdev_writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t* data) 
{
    #ifdef I2CDEV_SERIAL_DEBUG
        UART_print_s("I2C (0x");
        UART_print_d8(devAddr, HEX);
        UART_print_s(") writing ");
        UART_print_d8(length, DEC);
        UART_print_s(" words to 0x");
        UART_print_d8(regAddr, HEX);
        UART_print_s("...");
    #endif
    uint8_t status = 0;
    #if 1
        TwoWire_beginTransmission(devAddr);
        TwoWire_send(regAddr); // send address
    #elif 0
        Wire.beginTransmission(devAddr);
        Wire.write(regAddr); // send address
    #elif 0
        Fastwire::beginTransmission(devAddr);
        Fastwire::write(regAddr);
    #endif
	
    for (uint8_t i = 0; i < length * 2; i++) 
	{
        #ifdef I2CDEV_SERIAL_DEBUG
            UART_print_d8((uint8_t)(data[i] >> 8), HEX);
			UART_print_d8((uint8_t)(data[i]), HEX);
            if (i + 1 < length) UART_print_s(" ");
        #endif
        #if 1
            TwoWire_send((uint8_t)(data[i] >> 8));     // send MSB
            TwoWire_send((uint8_t)data[i++]);          // send LSB
        #elif 0
            Wire.write((uint8_t)(data[i] >> 8));    // send MSB
            Wire.write((uint8_t)data[i++]);         // send LSB
        #elif 0
            Fastwire::write((uint8_t)(data[i] >> 8));       // send MSB
            status = Fastwire::write((uint8_t)data[i++]);   // send LSB
            if (status != 0) break;
        #endif
    }
    #if 1
        status = TwoWire_endTransmission();
    #elif 0
        status = Wire.endTransmission();
    #elif 0
        Fastwire::stop();
        //status = Fastwire::endTransmission();
    #endif
    #ifdef I2CDEV_SERIAL_DEBUG
        UART_print_s(". Done.");
		UART_println();
    #endif
	if(0 == status)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

