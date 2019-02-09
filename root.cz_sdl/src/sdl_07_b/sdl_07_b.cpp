/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program pomoci primeho pristupu do pameti pixelu surface              *
 *   generuje ohen                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


// Algoritmu na ohen je spousta. Zde je pouzito obycejne prumerovani pixelu
// smerem od spodu nahoru. Clanek o generovani jednoho z tech lepsich typu
// lze najit na http://radek.jinak.cz/tutors/ohen/ohen.html


#include <stdio.h>
#include <stdlib.h>
#include <time.h>// Kvuli srand()
#include <SDL.h>


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

// NEMEL by byt pouzit hardwarovy surface, protoze by pixely neustale
// putovaly ke graficke karte a zpet!!!
#define WIN_FLAGS SDL_SWSURFACE|SDL_RESIZABLE
#define WIN_WIDTH 320
#define WIN_HEIGHT 120
// S jinou barevnou hloubkou nez 32 bpp tento program nepracuje!!!
#define WIN_BPP 32

#define WIN_TITLE "Ohen"

// Paleta ohne obsahuje 256 barev
#define NUM_COL 256


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;			// Surface okna
SDL_Cursor *g_cursor = NULL;		// Kurzor
Uint32 g_last_time;			// Pro casovac


// Paleta ohne (BGR format); hodnoty vygenerovany z obrazku fire_pallete.bmp
// pomoci zakomentovaneho kodu v Init(). Obrazek palety lze vytvorit napr.
// v grafickem editoru barevnym prechodem od cerne pres cervenou ke
// zlute barve. Rucni vytvareni neni zrovna nejlepsi napad ;)
Uint8 g_fpal[NUM_COL][3] =
{
	{   0,   0,   0 },{   0,   0,   3 },{   0,   0,   5 },{   0,   0,   8 },
	{   1,   1,  11 },{   1,   1,  15 },{   1,   1,  18 },{   2,   1,  22 },
	{   2,   2,  27 },{   2,   2,  31 },{   3,   2,  35 },{   3,   3,  40 },
	{   4,   3,  45 },{   3,   4,  50 },{   4,   4,  55 },{   4,   5,  59 },
	{   4,   5,  65 },{   5,   5,  70 },{   5,   5,  75 },{   6,   6,  80 },
	{   6,   6,  85 },{   6,   6,  90 },{   7,   7,  95 },{   7,   7, 100 },
	{   8,   8, 105 },{   8,   8, 110 },{   8,   8, 115 },{   8,   9, 119 },
	{   9,   9, 124 },{   9,   9, 128 },{  10,  10, 133 },{  10,  10, 138 },
	{  10,  10, 143 },{  11,  11, 148 },{  11,  11, 153 },{  11,  11, 158 },
	{  11,  12, 163 },{  12,  12, 168 },{  13,  13, 173 },{  13,  13, 178 },
	{  13,  13, 184 },{  13,  13, 188 },{  14,  14, 193 },{  14,  14, 198 },
	{  17,  19, 238 },{  17,  20, 238 },{  17,  21, 238 },{  16,  23, 239 },
	{  16,  25, 239 },{  17,  27, 239 },{  16,  29, 239 },{  16,  31, 239 },
	{  16,  33, 239 },{  16,  35, 240 },{  15,  37, 239 },{  15,  40, 240 },
	{  15,  42, 240 },{  15,  45, 240 },{  15,  47, 240 },{  14,  50, 241 },
	{  15,  53, 241 },{  14,  55, 241 },{  14,  58, 241 },{  14,  61, 242 },
	{  13,  64, 242 },{  13,  67, 242 },{  13,  70, 242 },{  13,  72, 242 },
	{  12,  75, 243 },{  12,  78, 243 },{  12,  81, 243 },{  12,  84, 243 },
	{  12,  87, 244 },{  11,  90, 244 },{  11,  94, 244 },{  11,  96, 244 },
	{  11,  99, 244 },{  10, 102, 245 },{  10, 105, 245 },{  10, 108, 245 },
	{  10, 110, 245 },{   9, 113, 246 },{   9, 116, 246 },{   9, 119, 246 },
	{   9, 122, 246 },{   9, 124, 247 },{   8, 127, 246 },{   8, 129, 247 },
	{   8, 132, 247 },{   8, 135, 247 },{   8, 137, 247 },{   7, 140, 248 },
	{   7, 143, 248 },{   7, 146, 248 },{   7, 149, 249 },{   6, 152, 249 },
	{   6, 155, 249 },{   6, 158, 249 },{   6, 161, 249 },{   6, 164, 250 },
	{   5, 167, 250 },{   5, 170, 250 },{   5, 173, 250 },{   5, 176, 250 },
	{   4, 178, 251 },{   4, 181, 251 },{   4, 185, 251 },{   4, 187, 251 },
	{   4, 190, 251 },{   3, 193, 252 },{   3, 196, 252 },{   3, 198, 252 },
	{   3, 201, 253 },{   3, 204, 252 },{   2, 206, 253 },{   2, 209, 253 },
	{   2, 211, 253 },{   2, 214, 253 },{   1, 216, 254 },{   1, 218, 254 },
	{   1, 220, 254 },{   1, 223, 254 },{   1, 224, 254 },{   1, 226, 255 },
	{   1, 228, 254 },{   0, 229, 254 },{   0, 231, 255 },{   0, 233, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },
	{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 },{   0, 234, 255 }
};


/*
 * Funkcni prototypy
 */

bool Init();				// Inicializace
void Destroy();				// Deinicializace
void Draw();				// Vykresleni
bool ProcessEvent();			// Osetruje udalosti
int  main(int argc, char *argv[]);	// Vstup do programu

// Zamyka/odemyka surface pro primy pristup k pixelum
bool Lock(SDL_Surface *surface);
void Unlock(SDL_Surface *surface);


/*
 * Zamyka/odemyka surface
 */

bool Lock(SDL_Surface *surface)
{
	if(SDL_MUSTLOCK(surface))
	{
		if(SDL_LockSurface(surface) < 0)
		{
			return false;
		}
	}

	return true;
}

void Unlock(SDL_Surface *surface)
{
	if(SDL_MUSTLOCK(surface))
	{
		SDL_UnlockSurface(surface);
	}
}


/*
 * Inicializacni funkce
 */

bool Init()
{
	// Inicializace generatoru nahodnych cisel
	srand((unsigned int)time((time_t*)NULL));

	// Inicializace SDL
	if(SDL_Init(SDL_SUBSYSTEMS) == -1)
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n",
				SDL_GetError());
		return false;
	}

	// Vytvori okno s definovanymi vlastnostmi
	g_screen = SDL_SetVideoMode(WIN_WIDTH, WIN_HEIGHT, WIN_BPP, WIN_FLAGS);

	if(g_screen == NULL)
	{
		fprintf(stderr, "Unable to set %dx%d video: %s\n",
				WIN_WIDTH, WIN_HEIGHT, SDL_GetError());
		return false;
	}

	SDL_WM_SetCaption(WIN_TITLE, NULL);

	// Kurzor mysi
/* hvezdicka - cerna, krizek - bila, pomlcka - pruhledna
	---**---
	-*----*-
	--------
	*--xx--*
	*--xx--*
	--------
	-*----*-
	---**---
*/
	Uint8 data[] = { 0x18, 0x42, 0x00, 0x81, 0x81, 0x00, 0x42, 0x18 };
	Uint8 mask[] = { 0x18, 0x42, 0x00, 0x99, 0x99, 0x00, 0x42, 0x18 };

	g_cursor = SDL_CreateCursor(data, mask, 8, 8, 3, 3);
	SDL_SetCursor(g_cursor);


	// Generovani "textovych hodnot" palety ohne z obrazku
	// Zadne chyby nejsou testovany - pouzije se jen jednou...
/*	SDL_Surface *tmp;
	tmp = SDL_LoadBMP("fire_pallete.bmp");

	FILE* fw;
	fw = fopen("pal.txt", "w");
	
	Uint8 *col;

	for(int i = 0; i < NUM_COL; i++)
	{
		fprintf(fw, "{ ");

		col = (Uint8 *)tmp->pixels + (i*tmp->format->BytesPerPixel);// B
		fprintf(fw, "%3d, ", *col);
		col = (Uint8 *)tmp->pixels + (i*tmp->format->BytesPerPixel + 1);// G
		fprintf(fw, "%3d, ", *col);
		col = (Uint8 *)tmp->pixels + (i*tmp->format->BytesPerPixel + 2);// R
		fprintf(fw, "%3d ", *col);

		fprintf(fw, "},");
		
		if(i % 4 == 3)
			fprintf(fw, "\n");
	}

	fclose(fw);
*/
	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	if(g_cursor != NULL)
		SDL_FreeCursor(g_cursor);

	SDL_Quit();
}


/*
 * Vykresleni sceny
 */

void Draw()
{
	Uint8 *p_pix;			// Ukazatel na prave modifikovane pixely
	int rnd;			// Uklada nahodne vygenerovane cislo
	register int x, line, col;	// Cykly
	int mouse_x, mouse_y;		// Pozice mysi

	if(!Lock(g_screen))
		return;

	// Pokud je stiskle leve mysitko, zacerni ctyri pixely na pozici kurzoru
	if(SDL_GetMouseState(&mouse_x, &mouse_y) & SDL_BUTTON(1))
	{
		p_pix = (Uint8 *)g_screen->pixels + mouse_y*g_screen->pitch + mouse_x*4;

		// Test okraju
		if(mouse_x < g_screen->w - 2 && mouse_y < g_screen->h - 2)
		{
			*p_pix = 255; *(p_pix + g_screen->pitch) = 255; p_pix++;
			*p_pix = 255; *(p_pix + g_screen->pitch) = 255; p_pix++;
			*p_pix = 255; *(p_pix + g_screen->pitch) = 255; p_pix+=2;
			*p_pix = 255; *(p_pix + g_screen->pitch) = 255; p_pix++;
			*p_pix = 255; *(p_pix + g_screen->pitch) = 255; p_pix++;
			*p_pix = 255; *(p_pix + g_screen->pitch) = 255;
		}
	}

	// Generovani paliva na poslednim radku
	p_pix = (Uint8 *)g_screen->pixels + (g_screen->h - 1)*g_screen->pitch;

	for(x = 0; x < g_screen->w; x++)
	{
		rnd = rand() % NUM_COL;

		for(col = 0; col < 3; col++, p_pix++)
			*p_pix = g_fpal[rnd][col];

		p_pix++;
	}

	// Horeni (prumerovani pixelu od spoda nahoru)
	for(line = g_screen->h - 2; line > 0; line--)
	{
		// Adresace radku
		p_pix = (Uint8 *)g_screen->pixels + line*g_screen->pitch;

		// Okraje se nemusi testovat, protoze pozice bude vzdy ve validni
		// pameti (posledni radek palivo)
		for(x = 0; x < g_screen->w; x++)
		{
			for(col = 0; col < 3; col++, p_pix++)
			{
				*p_pix = (Uint8)((*(p_pix)		// Aktualni
					+ *(p_pix + g_screen->pitch)	// Spodni
					+ *(p_pix + g_screen->pitch - 4)// Levy
					+ *(p_pix + g_screen->pitch + 4)// pravy
					) / 4.07f);// Ne 4, aby nehorel tak vysoko
			}
			p_pix++;
		}
	}

	Unlock(g_screen);
	SDL_Flip(g_screen);// Zobrazeni
}


/*
 * Osetreni udalosti
 * V tomto pripade ceka se na klavesu ESC, ktera ukonci program a reaguje
 * na zmenu velikosti okna
 */

bool ProcessEvent()
{
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			// Klavesnice
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						return false;
						break;

					default:
						break;
				}
				break;

			// Zmena velikosti okna
			case SDL_VIDEORESIZE:
				g_screen = SDL_SetVideoMode(event.resize.w,
					event.resize.h, WIN_BPP, WIN_FLAGS);

				if(g_screen == NULL)
				{
					fprintf(stderr,
						"Unable to resize window: %s\n",
						SDL_GetError());
					return false;
				}
				break;

			// Pozadavek na ukonceni
			case SDL_QUIT:
				return false;
				break;

			default:
				break;
		}
	}

	return true;
}


/*
 * Vstup do programu
 */

int main(int argc, char *argv[])
{
	printf(WIN_TITLE);
	printf("\nPress ESC key to quit.\n");

	// Inicializace
	if(!Init())
	{
		Destroy();
		return 1;
	}

	g_last_time = SDL_GetTicks();

	// Hlavni smycka programu
	bool done = false;
	while(!done)
	{
		done = !ProcessEvent();
		Draw();

		// Rychlostni optimalizace (hodne provizorni :(
		Uint32 time = SDL_GetTicks();

		if(time-g_last_time < 20)
		{
			SDL_Delay(40-(time-g_last_time));
		}

		g_last_time = time;
	}

	// Deinicializace a konec
	Destroy();
	return 0;
}
