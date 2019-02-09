/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program vykresli tri ctverce a linku palety sedi primym pristupem     *
 *   do graficke pameti surface okna                                       *
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

#include <stdio.h>
#include <SDL.h>

/*
 * Symbolicke konstanty
 * Neni spatny napad nacitat nektere parametry ze souboru...
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_FLAGS SDL_RESIZABLE
#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0

#define WIN_TITLE "Primy pristup k pixelum surface"


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;			// Surface okna


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

// Vykresli pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b);

// Vykresli obdelnik
void DrawRect(SDL_Surface *surface, SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b);


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
 * Vykresli pixel do surface (z vetsi casti prevzato ze SDL intro)
 * Funkce predpoklada, ze je uz surface zamknuty
 */

void DrawPixel(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	// Presahuje rozmery surface, zapis do neplatne pameti
	if(x >= surface->w || y >= surface->h)
		return;

	Uint32 color = SDL_MapRGB(surface->format, r, g, b);

	switch(surface->format->BytesPerPixel)
	{
		case 1:/* Assuming 8-bpp */
		{
			Uint8 *bufp;

			bufp = (Uint8 *)surface->pixels + y*surface->pitch + x;
			*bufp = color;
		}
		break;

		case 2:/* Probably 15-bpp or 16-bpp */
		{
			Uint16 *bufp;

			bufp = (Uint16 *)surface->pixels + y*surface->pitch/2 + x;
			*bufp = color;
		}
		break;

		case 3:/* Slow 24-bpp mode, usually not used */
		{
			Uint8 *bufp;

			bufp = (Uint8 *)surface->pixels + y*surface->pitch +
					x*surface->format->BytesPerPixel;
			*(bufp+surface->format->Rshift/8) = r;
			*(bufp+surface->format->Gshift/8) = g;
			*(bufp+surface->format->Bshift/8) = b;
		}
		break;

		case 4:/* Probably 32-bpp */
		{
			Uint32 *bufp;

			bufp = (Uint32 *)surface->pixels + y*surface->pitch/4 + x;
			*bufp = color;
		}
		break;
	}

	// Lepsi aktualizovat celou scenu najednou
//	SDL_UpdateRect(surface, x, y, 1, 1);
}


void DrawRect(SDL_Surface *surface, SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b)
{
	if(!Lock(surface))
		return;

	int i;

	for(i = rect->x; i < rect->x+rect->w+1; i++)// Horizontalni linky
	{
		DrawPixel(surface, i, rect->y, r, g, b);
		DrawPixel(surface, i, rect->y + rect->h, r, g, b);
	}

	for(i = rect->y+1; i < rect->y+rect->h; i++)// Vertikalni linky
	{
		DrawPixel(surface, rect->x, i, r, g, b);
		DrawPixel(surface, rect->x + rect->w, i, r, g, b);
	}

	Unlock(surface);
}


/*
 * Inicializacni funkce
 */

bool Init()
{
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

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	SDL_Quit();
}


/*
 * Vykresleni sceny
 */

void Draw()
{
	if(!Lock(g_screen))
		return;

	for(int i = 0; i < 255; i++)
	{
		DrawPixel(g_screen, i, 215, i, i, i);// Paleta sedi
	}

	Unlock(g_screen);

	SDL_Rect rect;

	rect.x = 40; rect.y = 40; rect.w = 40; rect.h = 40;
	DrawRect(g_screen, &rect, 255, 0, 0);

	rect.x = 60; rect.y = 60; rect.w = 60; rect.h = 60;
	DrawRect(g_screen, &rect, 0, 255, 0);

	rect.x = 90; rect.y = 90; rect.w = 90; rect.h = 90;
	DrawRect(g_screen, &rect, 0, 0, 255);

	SDL_Flip(g_screen);
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

	// Hlavni smycka programu
	bool done = false;
	while(!done)
	{
		done = !ProcessEvent();
		Draw();
	}

	// Deinicializace a konec
	Destroy();
	return 0;
}
