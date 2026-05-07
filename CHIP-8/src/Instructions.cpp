//-----------------------------------------------------------------------------
// File: Instructions.cpp
//
// Desc: Instructions implementation.
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

//-----------------------------------------------------------------------------
// Name: Dispatch0()
// Desc: Routes 0x00XX opcodes.
//-----------------------------------------------------------------------------
void CInstructions::Dispatch0( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( (opcode & 0xFF) == 0xE0 ) {
		CLS( interpreter, opcode );
		return;
	}

	RET( interpreter, opcode );
}

//-----------------------------------------------------------------------------
// Name: CLS()
// Desc: Clean screen.
//-----------------------------------------------------------------------------
void CInstructions::CLS( CInterpreter *interpreter, const uint16_t opcode )
{
	interpreter->m_display->Clear();
}

//-----------------------------------------------------------------------------
// Name: RET()
// Desc: Returns from subroutine.
//-----------------------------------------------------------------------------
void CInstructions::RET( CInterpreter *interpreter, const uint16_t opcode )
{
	interpreter->m_registers->pc = interpreter->m_registers->stack[ --interpreter->m_registers->SP ];
}

//-----------------------------------------------------------------------------
// Name: JP()
// Desc: Jumps to the specified address.
//-----------------------------------------------------------------------------
void CInstructions::JP( CInterpreter *interpreter, const uint16_t opcode )
{
	interpreter->m_registers->pc = opcode & 0xFFF;
}

//-----------------------------------------------------------------------------
// Name: CALL()
// Desc: Call subroutine at the specified address, and jump to the address.
//-----------------------------------------------------------------------------
void CInstructions::CALL( CInterpreter *interpreter, const uint16_t opcode )
{
	interpreter->m_registers->stack[ interpreter->m_registers->SP++ ] = interpreter->m_registers->pc;
	
	interpreter->m_registers->pc = opcode & 0xFFF;
}

//-----------------------------------------------------------------------------
// Name: SE()
// Desc: Skip next instruction if Vx == Kk or Vx == Vy.
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Name: SNE()
// Desc: Skip next instruction if Vx != Kk or Vx != Vy.
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Name: LD()
// Desc: Handles all load variants. 
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Name: ADD()
// Desc: Add Vx + Kk or Vx + Vy.
//-----------------------------------------------------------------------------
void CInstructions::ADD( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( ((opcode >> 12) & 0xF) == 0x7 ) {
		Vx += Kk;
		return;
	}

	VF = ( Vx + Vy ) > 255 ? 1 : 0;
	Vx += Vy;
}

//-----------------------------------------------------------------------------
// Name: OR()
// Desc: Calculates the OR operation between Vx and Vy.
//-----------------------------------------------------------------------------
void CInstructions::OR( CInterpreter *interpreter, const uint16_t opcode )
{
	Vx |= Vy;
}

//-----------------------------------------------------------------------------
// Name: AND()
// Desc: Calculates the AND operation between Vx and Vy.
//-----------------------------------------------------------------------------
void CInstructions::AND( CInterpreter *interpreter, const uint16_t opcode )
{
	Vx &= Vy;
}

//-----------------------------------------------------------------------------
// Name: XOR()
// Desc: Calculates the XOR operation between Vx and Vy.
//-----------------------------------------------------------------------------
void CInstructions::XOR( CInterpreter *interpreter, const uint16_t opcode )
{
	Vx ^= Vy;
}

//-----------------------------------------------------------------------------
// Name: SUB()
// Desc: Subtracts Vy from Vx. Uses branchless operation:
//			(~Vx & Vy): A bit where Vx = 0 and Vy = 1 guarantees a borrow at
//						that position.
//
//			(~(Vx ^ Vy) & (Vx - Vy)): Where Vx and Vy are equal, a borrow 
//									  propagates
//
//			>> 7: Extracts MSB, which holds the final borrow.
//
//		 The whole expression is then inverted (~(...) & 1) beucase VF is NOT
//		 borrow.
//		
//-----------------------------------------------------------------------------
void CInstructions::SUB( CInterpreter *interpreter, const uint16_t opcode )
{
	VF = ~( ( (~Vx & Vy) | (~(Vx ^ Vy) & (Vx - Vy)) ) >> 7 ) & 1;

	Vx = Vx - Vy;
}

//-----------------------------------------------------------------------------
// Name: SHR()
// Desc: Shift Vx one bit to the right.
//-----------------------------------------------------------------------------
void CInstructions::SHR( CInterpreter *interpreter, const uint16_t opcode )
{
	VF = Vx & 1;

	Vx >>= 1;
}

//-----------------------------------------------------------------------------
// Name: SUBN()
// Desc: Calculates Vy - Vx and stores in Vx. See SUB() for a detailed explana
//		 tion of the bit trick.
//-----------------------------------------------------------------------------
void CInstructions::SUBN( CInterpreter *interpreter, const uint16_t opcode )
{
	VF = ~( ( (~Vy & Vx) | (~(Vy ^ Vx) & (Vy - Vx)) ) >> 7 ) & 1;

	Vx = Vy - Vx;
}

//-----------------------------------------------------------------------------
// Name: SHL()
// Desc: Shift Vx one bit to the left.
//-----------------------------------------------------------------------------
void CInstructions::SHL( CInterpreter *interpreter, const uint16_t opcode )
{
	VF = ( Vx >> 7 ) & 1;

	Vx <<= 1;
}

//-----------------------------------------------------------------------------
// Name: RND()
// Desc: Generates a random byte and ANDs it with the immediate mask Kk.
//-----------------------------------------------------------------------------
void CInstructions::RND( CInterpreter *interpreter, const uint16_t opcode )
{
	Vx = (uint8_t)rand() & Kk;
}

//-----------------------------------------------------------------------------
// Name: DRW()
// Desc: Draw an n-byte sprite starting at memory address I at (Vx,Vy).
//-----------------------------------------------------------------------------
void CInstructions::DRW( CInterpreter *interpreter, const uint16_t opcode )
{
	uint8_t *spriteData = interpreter->m_memory->GetPointer( interpreter->m_registers->I );
	VF					= interpreter->m_display->Draw( Vx, Vy, spriteData, opcode & 0xF );
}

//-----------------------------------------------------------------------------
// Name: SKP()
// Desc: Skip the next instruction if the key with value Vx is pressed.
//-----------------------------------------------------------------------------
void CInstructions::SKP( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( interpreter->m_keys[ Vx ] ) {
		interpreter->m_registers->pc += 2;
	}
}

//-----------------------------------------------------------------------------
// Name: SKNP()
// Desc: Skip the next instruction if the key with the value Vx is NOT pressed.
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Name: Dispatch8()
// Desc: Routes 0x8xyn opcodes.
//-----------------------------------------------------------------------------
void CInstructions::Dispatch8( CInterpreter *interpreter, const uint16_t opcode )
{
	s_table8[ opcode & 0xF ]( interpreter, opcode );
}

//-----------------------------------------------------------------------------
// Name: DispatchE()
// Desc: Routes 0xExxx opcodes.
//-----------------------------------------------------------------------------
void CInstructions::DispatchE( CInterpreter *interpreter, const uint16_t opcode )
{
	if ( (opcode & 0xFF) == 0x9E ) {
		SKP( interpreter, opcode );
		return;
	}

	SKNP( interpreter, opcode );
}

//-----------------------------------------------------------------------------
// Name: DispatchF()
// Desc: Routes 0xFxxx opcodes.
//-----------------------------------------------------------------------------
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
