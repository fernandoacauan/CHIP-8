//-----------------------------------------------------------------------------
// File: Arena.cpp
//
// Desc: This allocator manages memory in large contiguous blocks ( arenas ).
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <Arena.h>


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

extern "C" void* __cdecl memset( void*, int, size_t );

struct block_s
{
	char *					base;
	char *					available;
	char *					limit;
	struct block_s *		next;
};

union align 
{
	char *					p;
	long					i;
	double					d;
	int						( *f )(void);
};

static inline int RoundUp( int x, int y )
{
	return ( x + (y - 1) ) & ~( y - 1 );
}

CArena CArena::s_arenas[ MAX_ARENA ];



//-----------------------------------------------------------------------------
// Name: CArena()
// Desc: Constructor
//-----------------------------------------------------------------------------
CArena::CArena( void )
:
	m_head( nullptr ),
	m_current( nullptr )
{
}

//-----------------------------------------------------------------------------
// Name: ~CArena()
// Desc: Destructor. Iterates through the linked list of blocks and frees both
//		 the raw memory chunk and the block metadata struct itself.
//-----------------------------------------------------------------------------
CArena::~CArena( void )
{
	block_t *current = m_head;

	while ( current ) {
		block_t *next = current->next;
		delete[] current->base;
		delete current;
		current = next;
	}

	m_head = m_current = nullptr;
}

//-----------------------------------------------------------------------------
// Name: Free()
// Desc: Clears all memory blocks from a specific arena.
//-----------------------------------------------------------------------------
void CArena::Free( size_t arena )
{
	if ( arena >= MAX_ARENA ) {
		return;
	}

	block_t *current = s_arenas[ arena ].m_head;

	while ( current ) {
		block_t *next = current->next;
		delete[] current->base;
		delete current;
		current = next;
	}

	s_arenas[ arena ].m_head	= nullptr;
	s_arenas[ arena ].m_current = nullptr;
}


//-----------------------------------------------------------------------------
// Name: NewBlock()
// Desc: Allocates a new block and its corresponding raw memory buffer.
//-----------------------------------------------------------------------------
block_t *CArena::NewBlock( size_t size )
{
	block_t *block		= new block_t;

	block->available	= ( block->base = new char[size] );
	block->limit		= block->base + size;
	block->next			= nullptr;

	return block;
}

//-----------------------------------------------------------------------------
// Name: Alloc()
// Desc: Reserves size bytes from the specified arena. If the current block
//		 doesn't have enough space, it creates a new block and updates the
//		 chain. Returns a pointer to the allocated memory.
//-----------------------------------------------------------------------------
char *CArena::Alloc( size_t size, size_t arena )
{
	size_t		round = RoundUp( size, sizeof(align) );
	char *		ptr;

	if ( arena >= MAX_ARENA ) {
		return nullptr;
	}

	CArena &	target = s_arenas[ arena ];

	if ( !target.m_current || (target.m_current->available + round) > target.m_current->limit ) {
		size_t		blockSize	= ( round > ARENA_SIZE ) ? round : ARENA_SIZE;
		block_t *	block		= NewBlock( blockSize );

		block->next				= target.m_head;
		target.m_head			= block;
		target.m_current		= block;
	}

	ptr = target.m_current->available;
	target.m_current->available += round;

	return ptr;
}

//-----------------------------------------------------------------------------
// Name: Calloc()
// Desc: Same behavior as alloc, but guarantees that the returned memory is
//		 init to zero before it is handed over.
//-----------------------------------------------------------------------------
char* CArena::Calloc( size_t size, size_t arena )
{
	char *ptr	= Alloc( size, arena );

	if ( !ptr ) {
		return nullptr;
	}

	memset( ptr, 0, size );

	return ptr;
}
