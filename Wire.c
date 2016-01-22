/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

	#include "Wire.h"
	#include <stdlib.h>
	#include <string.h>
	#include <inttypes.h>
	#include "twi.h"
	

// Initialize Class Variables //////////////////////////////////////////////////

uint8_t TwoWire_rxBuffer[BUFFER_LENGTH];
volatile uint8_t TwoWire_rxBufferIndex = 0;
volatile uint8_t TwoWire_rxBufferLength = 0;

volatile uint8_t TwoWire_txAddress = 0;
uint8_t TwoWire_txBuffer[BUFFER_LENGTH];
volatile uint8_t TwoWire_txBufferIndex = 0;
volatile uint8_t TwoWire_txBufferLength = 0;

uint8_t TwoWire_transmitting = 0;
void (*TwoWire_user_onRequest)(void);
void (*TwoWire_user_onReceive)(int);

// Public Methods //////////////////////////////////////////////////////////////

void TwoWire_begin(void)
{
  TwoWire_rxBufferIndex = 0;
  TwoWire_rxBufferLength = 0;

  TwoWire_txBufferIndex = 0;
  TwoWire_txBufferLength = 0;

  twi_init();
}

void TwoWire_begin_slave(uint8_t address)
{
  twi_setAddress(address);
  twi_attachSlaveTxEvent(TwoWire_onRequestService);
  twi_attachSlaveRxEvent(TwoWire_onReceiveService);
  TwoWire_begin();
}


uint8_t TwoWire_requestFrom(uint8_t address, uint8_t quantity)
{
  // clamp to buffer length
  if(quantity > BUFFER_LENGTH){
    quantity = BUFFER_LENGTH;
  }
  // perform blocking read into buffer
  uint8_t read = twi_readFrom(address, TwoWire_rxBuffer, quantity);
  // set rx buffer iterator vars
  TwoWire_rxBufferIndex = 0;
  TwoWire_rxBufferLength = read;

  return read;
}



void TwoWire_beginTransmission(uint8_t address)
{
  // indicate that we are transmitting
  TwoWire_transmitting = 1;
  // set address of targeted slave
  TwoWire_txAddress = address;
  // reset tx buffer iterator vars
  TwoWire_txBufferIndex = 0;
  TwoWire_txBufferLength = 0;
}


uint8_t TwoWire_endTransmission(void)
{
  // transmit buffer (blocking)
  int8_t ret = twi_writeTo(TwoWire_txAddress, TwoWire_txBuffer, TwoWire_txBufferLength, 1);
  // reset tx buffer iterator vars
  TwoWire_txBufferIndex = 0;
  TwoWire_txBufferLength = 0;
  // indicate that we are done transmitting
  TwoWire_transmitting = 0;
  return ret;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
void TwoWire_send(uint8_t data)
{
  if(TwoWire_transmitting){
  // in master transmitter mode
    // don't bother if buffer is full
    if(TwoWire_txBufferLength >= BUFFER_LENGTH){
      return;
    }
    // put byte in tx buffer
    TwoWire_txBuffer[TwoWire_txBufferIndex] = data;
    ++TwoWire_txBufferIndex;
    // update amount in buffer   
    TwoWire_txBufferLength = TwoWire_txBufferIndex;
  }else{
  // in slave send mode
    // reply to master
    twi_transmit(&data, 1);
  }
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
void TwoWire_send_quantity(uint8_t* data, uint8_t quantity)
{
  if(TwoWire_transmitting){
  // in master transmitter mode
    for(uint8_t i = 0; i < quantity; ++i){
      TwoWire_send(data[i]);
    }
  }else{
  // in slave send mode
    // reply to master
    twi_transmit(data, quantity);
  }
}


// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
uint8_t TwoWire_available(void)
{
  return TwoWire_rxBufferLength - TwoWire_rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
uint8_t TwoWire_receive(void)
{
  // default to returning null char
  // for people using with char strings
  uint8_t value = '\0';
  
  // get each successive byte on each call
  if(TwoWire_rxBufferIndex < TwoWire_rxBufferLength){
    value = TwoWire_rxBuffer[TwoWire_rxBufferIndex];
    ++TwoWire_rxBufferIndex;
  }

  return value;
}

// behind the scenes function that is called when data is received
void TwoWire_onReceiveService(uint8_t* inBytes, int numBytes)
{
  // don't bother if user hasn't registered a callback
  if(!TwoWire_user_onReceive){
    return;
  }
  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
  if(TwoWire_rxBufferIndex < TwoWire_rxBufferLength){
    return;
  }
  // copy twi rx buffer into local read buffer
  // this enables new reads to happen in parallel
  for(uint8_t i = 0; i < numBytes; ++i){
    TwoWire_rxBuffer[i] = inBytes[i];    
  }
  // set rx iterator vars
  TwoWire_rxBufferIndex = 0;
  TwoWire_rxBufferLength = numBytes;
  // alert user program
  TwoWire_user_onReceive(numBytes);
}

// behind the scenes function that is called when data is requested
void TwoWire_onRequestService(void)
{
  // don't bother if user hasn't registered a callback
  if(!TwoWire_user_onRequest){
    return;
  }
  // reset tx buffer iterator vars
  // !!! this will kill any pending pre-master sendTo() activity
  TwoWire_txBufferIndex = 0;
  TwoWire_txBufferLength = 0;
  // alert user program
  TwoWire_user_onRequest();
}

// sets function called on slave write
void TwoWire_onReceive( void (*function)(int) )
{
  TwoWire_user_onReceive = function;
}

// sets function called on slave read
void TwoWire_onRequest( void (*function)(void) )
{
  TwoWire_user_onRequest = function;
}


