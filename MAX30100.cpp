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

#include "hwlib.hpp"
#include "MAX30100.hpp"

	void MAX30100::writeReg(uint8_t address, uint8_t data){
		auto transactie = chip_bus.write(slave_address);
		transactie.write(address);
		transactie.write(data);
	}

	uint8_t MAX30100::readReg(uint8_t address){
		chip_bus.write(slave_address).write(address);
		return chip_bus.read(slave_address).read_byte();
		}

	MAX30100::MAX30100(uint_fast8_t & slave_address, hwlib::i2c_bus & chip_bus):
	slave_address (slave_address),
	chip_bus (chip_bus)
	{}

	bool MAX30100::init(){
		if (getPartID() != 0x11){
			return false;
		}
		clearFIFO();
		setMode(0x03);
		setPW(0x03);
		setSR(0x01);
		setHiRes(true);
		setLedCur(0x08, 0x0f);
		return true;
	}
	
	void MAX30100::clearFIFO(){
		writeReg(0x02, 0x00);
		writeReg(0x03, 0x00);
		writeReg(0x04, 0x00);
		}

	void MAX30100::setMode(uint8_t mode){
		writeReg(0x06, mode);
	}
		
	void MAX30100::setPW(uint8_t pulse){
		uint8_t state_now = readReg(0x07);
		writeReg(0x07, (state_now & 0xfc) | pulse);
	}
		
	void MAX30100::setSR(uint8_t sample_rate){
		uint8_t state_now = readReg(0x07);
		writeReg(0x07, (state_now & 0xe3) | sample_rate << 2);
	}
		
	void MAX30100::setHiRes(bool state){
		uint8_t state_now = readReg(0x07);
		if(state){
			writeReg(0x07, (state_now & 0xbf) | 1 << 6);
		}
		else{
			writeReg(0x07, (state_now & 0xbf) & ~(1 << 6));
		}
	}
		
	void MAX30100::setLedCur(uint8_t Red_Led_current, uint8_t IR_Led_Current){
		writeReg(0x09, Red_Led_current << 4 | IR_Led_Current);
	}

	void MAX30100::update_buffer(){
		uint8_t available_samples = NUM_AVAILABLE_SAMPLES();
		uint16_t IR_data = 0;
		uint16_t red_data = 0;
		if (available_samples == 15){
			for (unsigned int i = 0; i < available_samples; i++){
				readFIFO(IR_data, red_data);
				sample raw_values = {IR_data, red_data};
				hwlib::cout << "IR_data: " << IR_data << "\n";
				hwlib::cout << "red_data: " << red_data << "\n";
				sample_buffer[i] = raw_values;
			}
		}
	}
	
	void MAX30100::readFIFO(uint16_t & IR_data, uint16_t & red_data){
		chip_bus.write(slave_address).write(0x05); // Register
		auto address_to_read = chip_bus.read(slave_address);
		IR_data = address_to_read.read_byte() << 8; // data
		IR_data |= address_to_read.read_byte();
		red_data = address_to_read.read_byte() << 8;
		red_data |= address_to_read.read_byte();
	}

	uint8_t MAX30100::getPartID(){
		return readReg(0xFF);
	}
		
	uint8_t MAX30100::getFIFO_WR_PTR(){
		return readReg(0x02);
	}
	
	uint8_t MAX30100::getOverflowCount(){
		return readReg(0x03);
	}
		
	uint8_t MAX30100::getFIFO_RD_PTR(){
		return readReg(0x04);
	}
		
	uint8_t MAX30100::NUM_AVAILABLE_SAMPLES(){
		return (readReg(0x02) - readReg(0x04)) & (0x10-1);
	}

	float MAX30100::getTemp(){
		uint8_t state_now = readReg(0x06);
		writeReg(0x06, (state_now & 0xf7) | 1 << 3);
		temp = readReg(0x16) + (readReg(0x17) * 0.0625);
		return temp;
	}