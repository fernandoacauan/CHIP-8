//-----------------------------------------------------------------------------
// File: Display.cpp
//
// Desc: Display implementation.
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <Display.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Name: CDisplay()
// Desc: Initializes the display with all pixels off.
//-----------------------------------------------------------------------------
CDisplay::CDisplay( void )
{
	Clear();
}

//-----------------------------------------------------------------------------
// Name: Clear()
// Desc: Sets all the pixels to 0.
//-----------------------------------------------------------------------------
void CDisplay::Clear( void )
{
	memset( m_display, 0, sizeof(m_display) );
}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: Draws an n-byte sprite at screen position (x,y).
//-----------------------------------------------------------------------------
uint8_t CDisplay::Draw( const uint8_t x, const uint8_t y, const uint8_t * const sprite, const uint8_t n )
{
	uint8_t collision = 0;

	for ( size_t row = 0; row < n; row++ ) {
		uint8_t spriteByte = sprite[ row ];

		for ( size_t col = 0; col < 8; col++ ) {
			if ( spriteByte & (0x80 >> col) ) {
				size_t screenIdx = ( (y + row) % 32 ) * 64 + ( (x + col) % 64 );

				if ( m_display[screenIdx] == 0xFF ) {
					collision = 1;
				}

				m_display[ screenIdx ] ^= 0xFF;
			}
		}
	}

	return collision;
}

//-----------------------------------------------------------------------------
// Name: GetDisplay()
// Desc: Returns a read-only pointer to the pixel buffer.
//-----------------------------------------------------------------------------
const uint8_t *CDisplay::GetDisplay( void ) const
{
	return m_display;
}