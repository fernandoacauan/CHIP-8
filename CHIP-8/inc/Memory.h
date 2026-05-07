//-----------------------------------------------------------------------------
// File: Memory.h
//
// Desc: Memory definition. Uses branchless bounds checked R/W operations.
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#pragma once
#include <stdint.h>

static constexpr size_t RAM_SIZE = 4 * 1024;

//-----------------------------------------------------------------------------
// Name: class CMemory
// Desc: Memory interface
//-----------------------------------------------------------------------------
class CMemory
{
	uint8_t			m_ram[ RAM_SIZE ];
public:
					CMemory( const char *filename );

	uint8_t			Read( const uint16_t address ) const;
	void			Write( const uint16_t address, const uint8_t value );

	uint8_t *		GetPointer( const uint16_t address );
};