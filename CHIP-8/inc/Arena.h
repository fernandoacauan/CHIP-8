//-----------------------------------------------------------------------------
// File: Arena.h
//
// Desc: Arena Allocator interface.
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#pragma once

constexpr size_t MAX_ARENA	= 3;
constexpr size_t ARENA_SIZE	= ( 64 * 1024 );

typedef struct block_s		block_t;


//-----------------------------------------------------------------------------
// Name: class CArena
// Desc: Arena allocator class
//-----------------------------------------------------------------------------
class CArena
{
	static CArena			s_arenas[ MAX_ARENA ];
	block_t *				m_head;
	block_t *				m_current;

private:
							CArena( void );
							~CArena( void );

	static block_t *		NewBlock( size_t size );

public:
							CArena( const CArena& ) = delete;
	CArena &				operator=( CArena& )	= delete;

	static char *			Alloc( size_t size, size_t arena );
	static char *			Calloc( size_t size, size_t arena );

	static void				Free( size_t arena );
};


inline void *operator new( size_t, void *ptr )
{
	return ptr;
}