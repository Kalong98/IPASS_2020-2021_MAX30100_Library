/*
Arduino MAX30100 library
Copyright (C) 2021  Ka Long Yang <frankyyang1998@hotmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAX30100_HPP
#define MAX30100_HPP

#include "hwlib.hpp"
#include <array>

/// @file

/// \brief
/// sample ADT
/// \details
/// This is an ADT that implements a sample
/// The sample consist out of an uint16_t IR_data and and uint16_t red_data
struct sample{
	uint16_t IR_data;
	uint16_t red_data;
};

/// \brief
/// MAX30100 ADT
/// \details
/// This is an ADT that contains the variables and functions / method for I2C interacing with the MAX30100
class MAX30100{
private:
// I2C variables and methods
	uint_fast8_t & slave_address;
	hwlib::i2c_bus & chip_bus;
	
	/// \brief
	/// Write one byte to given address
	/// \details
	/// Writes one byte of data to given 8-bit register address
	void writeReg(uint8_t address, uint8_t data);
	
	/// \brief
	/// Read byte from given address
	/// \details
	/// Returns one byte of data from the given register address
	uint8_t readReg(uint8_t address);
	
// Calculated values
	std::array < sample, 15 > sample_buffer;
	uint8_t HR;
	uint8_t SpO2;
	float temp;
public:
	MAX30100(uint_fast8_t & slave_address, hwlib::i2c_bus & chip_bus);
// Init settings
	/// \brief
	/// Initialize default settings
	/// \details
	/// This function will clear the FIFO, set mode to SpO2, set pulse width to 1600 ms, set sampling rate to 100, 
	/// enable hi-res, set red LED current to 27.1 mA and IR LED current to 50 mA.
	/// Will return true upon succesfully doing so.
	bool init();
	
	/// \brief
	/// Clears the FIFO
	/// \details
	/// This function will clear the FIFO by writing 0x00 to the FIFO Write Pointer register, Over Flow Counter register and FIFO Read Pointer register
	void clearFIFO();
	
// Methods for configuration
	/// \brief
	/// Sets mode
	/// \details
	/// This method takes an uint8_t as argument and sets the mode of the MAX30100 according to given argument
	void setMode(uint8_t mode);
	
	/// \brief
	/// Sets pulse width
	/// \details
	/// This method takes an uint8_t as argument and sets the pulse width of the MAX30100 according to given argument
	void setPW(uint8_t pulse);
	
	/// \brief
	/// Sets sampling rate
	/// \details
	/// This method takes an uint8_t as argument and sets the sampling rate of the MAX30100 according to given argument
	void setSR(uint8_t sample_rate);
	
	/// \brief
	/// Enable / disable hi-res mode
	/// \details
	/// This method takes an boolean as argument.
	/// Enables hi-res when true, disables hi-res when false.
	void setHiRes(bool state);
	
	/// \brief
	/// Sets current for Red and IR LED
	/// \details
	/// This method takes two uint8_t as argument.
	/// Uses the four least significant bits of both argument to set the current.
	void setLedCur(uint8_t Red_Led_current, uint8_t IR_Led_Current);
	
// Methods for Sample collecting
	/// \brief
	/// Fills buffer with new samples
	/// \details
	/// This method must be called as fast as possible to prevent samples from overflowing in the FIFO
	/// it will fill the buffer with samples when the FIFO is full
	void update_buffer();
	
	/// \brief
	/// Reads one sample from FIFO
	/// \details
	/// This method reads the FIFO four times to get one sample and saves it to the given arguments.
	void readFIFO(uint16_t & red_data, uint16_t & IR_data);
// Methods for reading registers
	/// \brief
	/// Reads the part ID register
	/// \details
	/// This method reads and returns the one byte from the part ID register
	uint8_t getPartID();
	
	/// \brief
	/// Reads the FIFO Write Pointer register
	/// \details
	/// This method reads and returns the position of the FIFO Write Pointer
	uint8_t getFIFO_WR_PTR();
	
	/// \brief
	/// Reads the Overflow Counter register
	/// \details
	/// This method reads and returns the total overflowing samples from FIFO
	uint8_t getOverflowCount();
	
	/// \brief
	/// Reads the FIFO Read Pointer register
	/// \details
	/// This method reads and returns the position of the FIFO Read Pointer
	uint8_t getFIFO_RD_PTR();
	
	/// \brief
	/// Calculate samples to be read
	/// \details
	/// Calculate and return the total available samples to be read in the FIFO
	uint8_t NUM_AVAILABLE_SAMPLES();
	
// Variables and methods for data processing
	/// \brief
	/// Calculate onboard temperature
	/// \details
	/// Calculate and returns on board temperature from Temp_Integer and Temp_Fraction registers
	float getTemp();
};

#endif // MAX30100_HPP