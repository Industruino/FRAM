//  Basic read/write functions for the MB85RS64A SPI FRAM chip
//  Copyright (C) 2017  Industruino <connect@industruino.com>
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//  Developed by Claudio Indellicati <bitron.it@gmail.com>

#include "Fram.h"


#define FRAM_CMD_WREN  0x06
#define FRAM_CMD_WRDI  0x04
#define FRAM_CMD_READ  0x03
#define FRAM_CMD_WRITE 0x02

////////////////////////////////////////////////////////////////////////////////

FramClass::FramClass()
{
  csPin = FRAM_DEFAULT_CS_PIN;
  spiSettings = FRAM_DEFAULT_SPI_SETTINGS;
}

////////////////////////////////////////////////////////////////////////////////

void FramClass::begin (uint8_t cp, SPISettings ss)
{
  csPin = cp;
  spiSettings = ss;

  // Set CS pin HIGH and configure it as an output
  deassertCS();
  pinMode(csPin, OUTPUT);

  SPI.begin();
}

////////////////////////////////////////////////////////////////////////////////

uint8_t FramClass::write (uint16_t addr, uint8_t *data, uint16_t count)
{
  if (addr + count > FRAM_SIZE)
    return 1U;

  if (count == 0U)
    return 0U;

  assertCS();
  SPI.transfer(FRAM_CMD_WREN);
  deassertCS();

  assertCS();

  SPI.transfer(FRAM_CMD_WRITE);
  SPI.transfer16(addr);

  for (uint16_t i = 0; i < count; ++i)
    SPI.transfer(data[i]);

  deassertCS();

  assertCS();
  SPI.transfer(FRAM_CMD_WRDI);
  deassertCS();

  return 0U;
}

////////////////////////////////////////////////////////////////////////////////

uint8_t FramClass::read (uint16_t addr, uint8_t *dataBuffer, uint16_t count)
{
  if (addr + count > FRAM_SIZE)
    return 1U;

  if (count == 0U)
    return 0U;

  assertCS();

  SPI.transfer(FRAM_CMD_READ);
  SPI.transfer16(addr);

  for (uint16_t i=0; i < count; ++i)
    dataBuffer[i] = SPI.transfer(0x00);

  deassertCS();

  return 0U;
}

////////////////////////////////////////////////////////////////////////////////

FramClass Fram;

