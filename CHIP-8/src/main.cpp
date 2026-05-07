//-----------------------------------------------------------------------------
// File: main.cpp
//
// Desc: Project entry.
//
// Copyright (C) Fernando A. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <SDL.h>
#include <Interpreter.h>
#include <Arena.h>
#include <stdio.h>

static constexpr size_t CYCLES_PER_FRAME	= 5;

static constexpr SDL_Scancode s_keyMap[ 16 ] = {
	SDL_SCANCODE_X,
	SDL_SCANCODE_1,
	SDL_SCANCODE_2,
	SDL_SCANCODE_3,
	SDL_SCANCODE_Q,
	SDL_SCANCODE_W,
	SDL_SCANCODE_E,
	SDL_SCANCODE_A,
	SDL_SCANCODE_S,
	SDL_SCANCODE_D,
	SDL_SCANCODE_Z,
	SDL_SCANCODE_C,
	SDL_SCANCODE_4,
	SDL_SCANCODE_R,
	SDL_SCANCODE_F,
	SDL_SCANCODE_V,
};

//-----------------------------------------------------------------------------
// Name: UpdateTexture()
// Desc: Copies the display buffier into the SDL streaming texture.
//-----------------------------------------------------------------------------
static void UpdateTexture( SDL_Texture *texture, const CDisplay *display )
{
	void *			pixels;
	int				pitch;
	uint32_t *		dst;

	SDL_LockTexture( texture, nullptr, &pixels, &pitch );

	dst					= (uint32_t *)pixels;
	const uint8_t *src	= display->GetDisplay();

	for ( size_t idx = 0; idx < 64 * 32; idx++ ) {
		dst[ idx ] = src [ idx ] ? 0xFFFFFFFF : 0xFF000000;
	}

	SDL_UnlockTexture( texture );
}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Initializes SDL and all emulator subsystems.
//-----------------------------------------------------------------------------
int main( int argc, char **argv )
{
	if ( argc < 2 ) {
		printf( "usage: CHIP-8.exe game.format\n" );
		getchar();
		return 1;
	}

	SDL_Init( SDL_INIT_VIDEO );
	SDL_Window *	window		= SDL_CreateWindow( "CHIP-8", 100, 100, 640, 320, SDL_WINDOW_SHOWN );
	SDL_Renderer *	renderer	= SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	SDL_Texture *	texture		= SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 64, 32 );

	CMemory *		memory		= new ( CArena::Alloc(sizeof(CMemory), 0) ) CMemory( argv[1] );
	CDisplay *		display		= new ( CArena::Alloc(sizeof(CDisplay), 0) ) CDisplay();
	CInterpreter *	interpreter = new ( CArena::Alloc(sizeof(CInterpreter), 0) ) CInterpreter( memory, display );
	
	bool			quit		= false;
	SDL_Event		e;

	interpreter->Init();

	while ( !quit ) {
		uint32_t frameStart = SDL_GetTicks();
		while ( SDL_PollEvent(&e) ) {
			if ( e.type == SDL_QUIT ) {
				quit = true;
			}
		}

		const uint8_t *sdlKeys = SDL_GetKeyboardState( nullptr );

		for ( size_t key = 0; key < 16; key++ ) {
			interpreter->SetKey( (uint8_t)key, sdlKeys[s_keyMap[key]] ? 1 : 0 );
		}

		for ( size_t cycles = 0; cycles < CYCLES_PER_FRAME; cycles++ ) {
			interpreter->Execute();
		}

		interpreter->TickTimers();

		UpdateTexture( texture, display );
		
		SDL_RenderClear( renderer );
		SDL_RenderCopy( renderer, texture, nullptr, nullptr );
		SDL_RenderPresent( renderer );

	}

	SDL_DestroyTexture( texture );
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	SDL_Quit();

	CArena::Free( 0 );
	return 0;
}
