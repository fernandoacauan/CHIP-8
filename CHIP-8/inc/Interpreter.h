//-----------------------------------------------------------------------------
// File: Interpreter.h
//
// Desc: CPU interpreter interface.
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#pragma once
#include <Display.h>
#include <Memory.h>
#include <stdint.h>

class CInterpreter;

typedef struct registers_s
{
	uint8_t				SP;
	uint8_t				DT;
	uint8_t				ST;
	uint8_t				V[ 16 ];
	uint16_t			I;
	uint16_t			pc;
	uint16_t			stack[ 16 ];
} registers_t;

typedef void			( *OpcodeHandler )( CInterpreter *, uint16_t );
extern OpcodeHandler	s_mainTable[ 16 ];

//-----------------------------------------------------------------------------
// Name: class CInterpreter
// Desc: Interpreter interface
//-----------------------------------------------------------------------------
class CInterpreter
{
	registers_t *		m_registers;
	CMemory *			m_memory;
	CDisplay *			m_display;
	uint8_t				m_keys[ 16 ];
	friend class		CInstructions;
public:
						CInterpreter( CMemory *memory, CDisplay *display );

	void				Init( void );
	void				Execute( void );

	void				SetKey( const uint8_t key, const uint8_t state );
	void				TickTimers( void );
};
