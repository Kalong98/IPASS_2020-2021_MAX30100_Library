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

int main(){
	uint_fast8_t slave_address = 0x57;
	
	auto scl = hwlib::target::pin_oc(hwlib::target::pins::d21);
    auto sda = hwlib::target::pin_oc(hwlib::target::pins::d20);
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
	
	MAX30100 oximeter (slave_address, i2c_bus);
	oximeter.init();
	while(1){
	oximeter.update_buffer();
	}
}