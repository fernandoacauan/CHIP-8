//-----------------------------------------------------------------------------
// File: Instructions.h
//
// Desc: Instructions definition.
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#pragma once
#include <stdint.h>

class CInterpreter;

class CInstructions
{
public:
				CInstructions( void ) = delete;

	static void CLS( CInterpreter *interpreter, const uint16_t opcode );
	static void RET( CInterpreter *interpreter, const uint16_t opcode );
	static void JP( CInterpreter *interpreter, const uint16_t opcode );
	static void CALL( CInterpreter *interpreter, const uint16_t opcode );
	static void SE( CInterpreter *interpreter, const uint16_t opcode );
	static void SNE( CInterpreter *interpreter, const uint16_t opcode );

	static void LD( CInterpreter *interpreter, const uint16_t opcode );
	static void ADD( CInterpreter *interpreter, const uint16_t opcode );
	static void OR( CInterpreter *interpreter, const uint16_t opcode );
	static void AND( CInterpreter *interpreter, const uint16_t opcode );
	static void XOR( CInterpreter *interpreter, const uint16_t opcode );
	
	static void SUB( CInterpreter *interpreter, const uint16_t opcode );
	static void SHR( CInterpreter *interpreter, const uint16_t opcode );
	static void SUBN( CInterpreter *interpreter, const uint16_t opcode );
	static void SHL( CInterpreter *interpreter, const uint16_t opcode );

	static void RND( CInterpreter *interpreter, const uint16_t opcode );

	static void DRW( CInterpreter *interpreter, const uint16_t opcode );

	static void SKP( CInterpreter *interpreter, const uint16_t opcode );
	static void SKNP( CInterpreter *interpreter, const uint16_t opcode );

	static void Dispatch0( CInterpreter *interpreter, const uint16_t opcode );
	static void Dispatch8( CInterpreter *interpreter, const uint16_t opcode );
	static void DispatchE( CInterpreter *interpreter, const uint16_t opcode );
	static void DispatchF( CInterpreter *interpreter, const uint16_t opcode );
};
