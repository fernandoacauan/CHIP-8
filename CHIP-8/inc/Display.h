//-----------------------------------------------------------------------------
// File: Display.h
//
// Desc: Display definition.
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#pragma once
#include <stdint.h>

static constexpr size_t DISPLAY_WIDTH	= 64;
static constexpr size_t DISPLAY_HEIGHT	= 32;

//-----------------------------------------------------------------------------
// Name: class CDisplay
// Desc: Display interface.
//-----------------------------------------------------------------------------
class CDisplay
{
	uint8_t			m_display[ DISPLAY_HEIGHT * DISPLAY_WIDTH ];
public:
					CDisplay( void );

	uint8_t			Draw( const uint8_t x, const uint8_t y, const uint8_t * const sprite, const uint8_t n );
	void			Clear( void );

	const uint8_t *	GetDisplay( void ) const;
};