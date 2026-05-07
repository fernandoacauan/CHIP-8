//-----------------------------------------------------------------------------
// File: Interpreter
//
// Desc: CPU interpreter implementation.
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <Interpreter.h>
#include <Arena.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Name: CInterpreter()
// Desc: Allocates the register file from the arena and stores references to
//		 memory and display.
//-----------------------------------------------------------------------------
CInterpreter::CInterpreter( CMemory *memory, CDisplay *display )
:
	m_memory( memory ),
	m_display( display )
{
	m_registers = (registers_t *)CArena::Alloc( sizeof(registers_t), 0 );
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Resets the register file to a known state. PC starts at 0x200.
//-----------------------------------------------------------------------------
void CInterpreter::Init( void )
{
	memset( m_registers, 0, sizeof(registers_t) );
	m_registers->pc = 0x200;
}

//-----------------------------------------------------------------------------
// Name: SetKey()
// Desc: Records the pressed (1) or released (0) state of key. The low nibble
//		 mask ensures the index never exceeds m_keys[15].
//-----------------------------------------------------------------------------
void CInterpreter::SetKey( const uint8_t key, const uint8_t state )
{
	m_keys[ key & 0xF ] = state;
}

//-----------------------------------------------------------------------------
// Name: TickTimers()
// Desc: Decrements the delay and sound timers by 1 each time it is called.
//-----------------------------------------------------------------------------
void CInterpreter::TickTimers( void )
{
	if ( m_registers->DT ) {
		--m_registers->DT;
	}

	if ( m_registers->ST ) {
		--m_registers->ST;
	}
}

//-----------------------------------------------------------------------------
// Name: Execute()
// Desc: Fetches one 16-bit big endian opcode from memory at PC, advances PC
//		 by 2, then dispatches to the appropriate handler through s_mainTable.
//-----------------------------------------------------------------------------
void CInterpreter::Execute( void )
{
	uint16_t opcode = ( m_memory->Read(m_registers->pc) << 8 ) | m_memory->Read( m_registers->pc + 1 );

	m_registers->pc += 2;
	
	s_mainTable[ (opcode >> 12) & 0xF ]( this, opcode );
}