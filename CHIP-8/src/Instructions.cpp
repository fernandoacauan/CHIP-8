//-----------------------------------------------------------------------------
// File: Instructions.cpp
//
// Desc:
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <Instructions.h>
#include <Interpreter.h>
#include <stdlib.h>

#define Vx		interpreter->m_registers->V[ (opcode >> 8) & 0xF ]
#define Kk		( opcode & 0xFF )
#define Vy		interpreter->m_registers->V[ (opcode >> 4) & 0xF ]
#define VF		interpreter->m_registers->V[ 0xF ]

void CInstructions::Dispatch0( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( (opcode & 0xFF) == 0xE0 ) {
		CLS( interpreter, opcode );
		return;
	}

	RET( interpreter, opcode );
}

void CInstructions::CLS( CInterpreter *interpreter, const uint16_t opcode )
{
	interpreter->m_display->Clear();
}

void CInstructions::RET( CInterpreter *interpreter, const uint16_t opcode )
{
	interpreter->m_registers->pc = interpreter->m_registers->stack[ --interpreter->m_registers->SP ];
}

void CInstructions::JP( CInterpreter *interpreter, const uint16_t opcode )
{
	interpreter->m_registers->pc = opcode & 0xFFF;
}

void CInstructions::CALL( CInterpreter *interpreter, const uint16_t opcode )
{
	interpreter->m_registers->stack[ interpreter->m_registers->SP++ ] = interpreter->m_registers->pc;
	
	interpreter->m_registers->pc = opcode & 0xFFF;
}

void CInstructions::SE( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( ((opcode >> 12) & 0xF) == 0x3 ) {
		if ( Vx == Kk ) {
			interpreter->m_registers->pc += 2;
		}
		return;
	}

	if ( Vx == Vy ) {
		interpreter->m_registers->pc += 2;
	}
}

void CInstructions::SNE( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( ((opcode >> 12) & 0xF) == 0x4 ) {
		if ( Vx != Kk ) {
			interpreter->m_registers->pc += 2;
		}
		return;
	}

	if ( Vx != Vy ) {
		interpreter->m_registers->pc += 2;
	}
}

void CInstructions::LD( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( ((opcode >> 12) & 0xF) == 0x6 ) {
		Vx = Kk;
		return;
	}

	if ( ((opcode >> 12) & 0xF) == 0x8 ) {
		Vx = Vy;
		return;
	}

	if ( ((opcode >> 12) & 0xF) == 0xA ) {
		interpreter->m_registers->I = opcode & 0xFFF;
		return;
	}
}

void CInstructions::ADD( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( ((opcode >> 12) & 0xF) == 0x7 ) {
		Vx += Kk;
		return;
	}

	VF = ( Vx + Vy ) > 255 ? 1 : 0;
	Vx += Vy;
}

void CInstructions::OR( CInterpreter *interpreter, const uint16_t opcode )
{
	Vx |= Vy;
}

void CInstructions::AND( CInterpreter *interpreter, const uint16_t opcode )
{
	Vx &= Vy;
}

void CInstructions::XOR( CInterpreter *interpreter, const uint16_t opcode )
{
	Vx ^= Vy;
}

void CInstructions::SUB( CInterpreter *interpreter, const uint16_t opcode )
{
	VF = ~( ( (~Vx & Vy) | (~(Vx ^ Vy) & (Vx - Vy)) ) >> 7 ) & 1;

	Vx = Vx - Vy;
}

void CInstructions::SHR( CInterpreter *interpreter, const uint16_t opcode )
{
	VF = Vx & 1;

	Vx >>= 1;
}

void CInstructions::SUBN( CInterpreter *interpreter, const uint16_t opcode )
{
	VF = ~( ( (~Vy & Vx) | (~(Vy ^ Vx) & (Vy - Vx)) ) >> 7 ) & 1;

	Vx = Vy - Vx;
}

void CInstructions::SHL( CInterpreter *interpreter, const uint16_t opcode )
{
	VF = ( Vx >> 7 ) & 1;

	Vx <<= 1;
}

void CInstructions::RND( CInterpreter *interpreter, const uint16_t opcode )
{
	Vx = (uint8_t)rand() & Kk;
}

void CInstructions::DRW( CInterpreter *interpreter, const uint16_t opcode )
{
	uint8_t *spriteData = interpreter->m_memory->GetPointer( interpreter->m_registers->I );
	VF					= interpreter->m_display->Draw( Vx, Vy, spriteData, opcode & 0xF );
}

void CInstructions::SKP( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( interpreter->m_keys[ Vx ] ) {
		interpreter->m_registers->pc += 2;
	}
}

void CInstructions::SKNP( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( !interpreter->m_keys[ Vx ] ) {
		interpreter->m_registers->pc += 2;
	}
}


static OpcodeHandler s_table8[ 16 ] = {
	CInstructions::LD,			CInstructions::OR,	CInstructions::AND, CInstructions::XOR,
	CInstructions::ADD,			CInstructions::SUB,	CInstructions::SHR, CInstructions::SUBN,
	nullptr,					nullptr,			nullptr,			nullptr,
	nullptr,					nullptr,			CInstructions::SHL, nullptr
};

void CInstructions::Dispatch8( CInterpreter *interpreter, const uint16_t opcode )
{
	s_table8[ opcode & 0xF ]( interpreter, opcode );
}

void CInstructions::DispatchE( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( (opcode & 0xFF) == 0x9E ) {
		SKP( interpreter, opcode );
		return;
	}

	SKNP( interpreter, opcode );
}

void CInstructions::DispatchF( CInterpreter *interpreter, const uint16_t opcode )
{
	switch ( opcode & 0xFF ) {
		case 0x07: Vx = interpreter->m_registers->DT;		break;

		case 0x0A: {
			bool keyPressed = false;

			for ( size_t idx = 0; idx < 16; idx++ ) {
				if ( interpreter->m_keys[idx] ) {
					Vx = (uint8_t)idx;
					keyPressed = true;
					break;
				}
			}

			if ( !keyPressed ) {
				interpreter->m_registers->pc -= 2;
			}
			break;
		}

		case 0x15: interpreter->m_registers->DT = Vx;		break;
		case 0x18: interpreter->m_registers->ST = Vx;		break;
		case 0x1E: interpreter->m_registers->I += Vx;		break;
		case 0x29: interpreter->m_registers->I = Vx * 5;	break;
		case 0x33: {
			uint8_t v	= Vx;
			uint8_t q1	= (uint8_t)( (v * 205u) >> 11 );
			uint8_t q2	= (uint8_t)( (q1 * 205u) >> 11 );

			interpreter->m_memory->Write( interpreter->m_registers->I, q2 );
			interpreter->m_memory->Write( interpreter->m_registers->I + 1, q1 - q2 * 10 );
			interpreter->m_memory->Write( interpreter->m_registers->I + 2, v - q1 * 10 );
			break;
		}
		case 0x55: {
			for ( size_t idx = 0; idx <= ((opcode >> 8) & 0xF); idx++ ) {
				interpreter->m_memory->Write( interpreter->m_registers->I + idx, interpreter->m_registers->V[ idx ] );
			}
			break;
		}
		case 0x65: {
			for ( size_t idx = 0; idx <= ((opcode >> 8) & 0xF); idx++ ) {
				interpreter->m_registers->V[ idx ] = interpreter->m_memory->Read( interpreter->m_registers->I + idx );
			}
			break;
		}
	}
}

OpcodeHandler s_mainTable[ 16 ] = {
	CInstructions::Dispatch0,	CInstructions::JP,	CInstructions::CALL,		CInstructions::SE,
	CInstructions::SNE,			CInstructions::SE,	CInstructions::LD,			CInstructions::ADD,
	CInstructions::Dispatch8,	CInstructions::SNE,	CInstructions::LD,			CInstructions::JP,
	CInstructions::RND,			CInstructions::DRW, CInstructions::DispatchE,	CInstructions::DispatchF
};