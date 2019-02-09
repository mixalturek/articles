/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program ukazuje rozdily mezi ruznymi nastavenimi vlastnosti surface   *
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
#include "functions.h"


/*
 * Symbolicke konstanty
 * Neni spatny napad nacitat nektere parametry ze souboru...
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_FLAGS SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE
#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0

#define WIN_TITLE "Vlastnosti surface"


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;			// Surface okna
SDL_Surface *g_img;			// Obrazek


/*
 * Funkcni prototypy
 */

bool Init();				// Inicializace
void Destroy();				// Deinicializace
void Draw();				// Vykresleni
bool ProcessEvent();			// Osetruje udalosti
int  main(int argc, char *argv[]);	// Vstup do programu


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

	// Loading obrazku
	if((g_img = LoadImage("bird.png")) == NULL)
		return false;

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	if(g_img != NULL)
		SDL_FreeSurface(g_img);

	SDL_Quit();
}


/*
 * Vykresleni sceny
 */

void Draw()
{
	SDL_Rect rect;

	// V oblasti 10 pixelu okolo okraju okna nebude nic vykresleno
	// Pokud se nebude menit, lepsi umistit do inicializace...
	rect.x = 10;
	rect.y = 10;
	rect.w = g_screen->w - 20;
	rect.h = g_screen->h - 20;
	SDL_SetClipRect(g_screen, &rect);


	// Cervene pozadi okna
	// Stejneho efektu by se dalo dosahnout bez volani SDL_SetClipRect()
	// predanim &rect misto NULL primo do teto funkce
	SDL_FillRect(g_screen, NULL, SDL_MapRGB(g_screen->format, 255, 0, 0));


	// Uprostred leve horni ctvrtiny okna originalni obrazek
	rect.x = (g_screen->w >> 2) - (g_img->w >> 1);
	rect.y = (g_screen->h >> 2) - (g_img->h >> 1);

	SDL_SetColorKey(g_img, 0, 0);
	SDL_SetAlpha(g_img, 0, 0);
	SDL_BlitSurface(g_img, NULL, g_screen, &rect);


	// Uprostred prave horni ctvrtiny okna obrazek s transparentni bilou
	rect.x = (g_screen->w >> 2)*3 - (g_img->w >> 1);
	rect.y = (g_screen->h >> 2) - (g_img->h >> 1);

	SDL_SetColorKey(g_img, SDL_SRCCOLORKEY,
			SDL_MapRGB(g_img->format, 255, 255, 255));
//	SDL_SetAlpha(g_img, 0, 0);// Uz je nastaveno
	SDL_BlitSurface(g_img, NULL, g_screen, &rect);


	// Uprostred leve dolni ctvrtiny okna obrazek s 50% pruhlednosti
	rect.x = (g_screen->w >> 2) - (g_img->w >> 1);
	rect.y = (g_screen->h >> 2)*3 - (g_img->h >> 1);

	SDL_SetColorKey(g_img, 0, 0);
	SDL_SetAlpha(g_img, SDL_SRCALPHA, 128);
	SDL_BlitSurface(g_img, NULL, g_screen, &rect);


	// Uprostred prave dolni ctvrtiny okna kombinace obou predchozich
	rect.x = (g_screen->w >> 2)*3 - (g_img->w >> 1);
	rect.y = (g_screen->h >> 2)*3 - (g_img->h >> 1);

	SDL_SetColorKey(g_img, SDL_SRCCOLORKEY,
			SDL_MapRGB(g_img->format, 255, 255, 255));
//	SDL_SetAlpha(g_img, SDL_SRCALPHA, 128);// Uz je nastaveno
	SDL_BlitSurface(g_img, NULL, g_screen, &rect);


	// Prohozeni bufferu
	SDL_Flip(g_screen);
}


/*
 * Osetruje udalosti (bude probrano nekdy v budoucnu)
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
