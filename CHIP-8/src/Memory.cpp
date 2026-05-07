//-----------------------------------------------------------------------------
// File: Memory.cpp
//
// Desc: Memory implementation.
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <Memory.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static constexpr uint8_t s_font[ 80 ] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,	// 0
	0x20, 0x60, 0x20, 0x20, 0x70,	// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,	// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,	// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,	// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,	// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,	// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,	// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,	// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,	// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,	// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,	// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,	// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,	// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,	// E
	0xF0, 0x80, 0xF0, 0x80, 0x80	// F
};

//-----------------------------------------------------------------------------
// Name: ReadFile()
// Desc: Loads file content into out. If file can't be opened, return false.
//-----------------------------------------------------------------------------
static bool ReadFile( const char *filename, uint8_t *out )
{
	FILE *	file;
	size_t	size;

	file = fopen( filename, "rb" );

	if ( !file ) {
		return false;
	}

	fseek( file, 0, SEEK_END );
	size = ftell( file );
	rewind( file );

	if ( size > (RAM_SIZE - 0x200) ) {
		size = RAM_SIZE - 0x200;
	}

	fread( out + 0x200, sizeof(uint8_t), size, file );
	fclose( file );
	return true;
}

//-----------------------------------------------------------------------------
// Name: CMemory()
// Desc: Copies the font sprites into the reserved 0x000 region, then loads
//		 the rom into ram starting at 0x200.
//-----------------------------------------------------------------------------
CMemory::CMemory( const char *filename )
{
	memcpy( m_ram, s_font, sizeof(s_font) );

	if ( !ReadFile(filename, m_ram) ) {
		exit( EXIT_FAILURE );
	}
}

//-----------------------------------------------------------------------------
// Name: Read()
// Desc: Returns the byte in the address. This branchless bound check computes
//		 the borrow bit of the subtraction (address - RAM_SIZE):
// 
//			borrow = {1, when address < RAM_SIZE (underflow)
//					 {0, when address >= RAM_SIZE
// 
//		 The borrow propagates into bit 15 of 'condition', so:
// 
//			mask = -(condition >> 15)
//			
//			if address in range => condition (bit 15) = 1 => mask = 0xFFFF
//			=> read m_ram[address]
// 
//			if address is out of range => condition = 0 => mask = 0x0000
//			=> read m_ram[0] & 0 = 0
// 
//		In resume:
//			Valid: m_ram[address] & 0xFF  (actual byte)
//			Invalid: m_ram[0] & 0x00 = 0;
//-----------------------------------------------------------------------------
uint8_t CMemory::Read( const uint16_t address ) const
{
	const uint16_t condition	= ( ~address & RAM_SIZE ) | ( (~address | RAM_SIZE) & (address - RAM_SIZE) );
	const uint16_t mask			= -( condition >> 15 );

	return m_ram[ address & mask ] & (uint8_t)mask;
}

//-----------------------------------------------------------------------------
// Name: Write()
// Desc: Writes the value to address. Uses the same bound check as Read().
//-----------------------------------------------------------------------------
void CMemory::Write( const uint16_t address, const uint8_t value )
{
	const uint16_t condition	= ( ~address & RAM_SIZE ) | ( (~address | RAM_SIZE) & (address - RAM_SIZE) );
	const uint16_t mask			= -( condition >> 15 );

	m_ram[ address & mask ] = ( value & (uint8_t)mask ) | ( m_ram[ address & mask ] & (uint8_t)~mask );
}

//-----------------------------------------------------------------------------
// Name: GetPointer()
// Desc: Returns a pointer to m_ram[address]
//-----------------------------------------------------------------------------
uint8_t *CMemory::GetPointer( const uint16_t address )
{
	if( address >= RAM_SIZE ) {
		return nullptr;
	}

	return &m_ram[ address ];
}