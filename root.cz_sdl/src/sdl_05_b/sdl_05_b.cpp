/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Objekt se pohybuje po prime draze oknem a odrazi se od okraju         *
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
#include <stdlib.h>
#include <time.h>// Kvuli srand()
#include <SDL.h>
#include "functions.h"


/*
 * Symbolicke konstanty
 * Neni spatny napad nacitat nektere parametry ze souboru...
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_FLAGS SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE
#define WIN_WIDTH 640
#define WIN_HEIGHT 480
#define WIN_BPP 0

#define WIN_TITLE "Objekt odrazejici se od okraju"


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;			// Surface okna
SDL_Surface *g_bg_img;			// Textura pozadi
SDL_Surface *g_obj_img;			// Obrazek objektu

float g_posx, g_posy;			// Pozice objektu
float g_velx, g_vely;			// Smer pohybu


/*
 * Funkcni prototypy
 */

bool Init();				// Inicializace
void Destroy();				// Deinicializace
void Draw();				// Vykresleni
void Update();				// Aktualizace sceny
bool ProcessEvent();			// Osetruje udalosti
int main(int argc, char *argv[]);	// Vstup do programu


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

	// Loading obrazku
	if(((g_bg_img = LoadImage("bg.png", false)) == NULL)
			|| ((g_obj_img = LoadImage("object.png", true)) == NULL))
	{
		return false;
	}

	// Pozice a smer pohybu objektu (nahodne)
	g_posx = rand() % (g_screen->w - g_obj_img->w);
	g_posy = rand() % (g_screen->h - g_obj_img->h);
	g_velx = (rand() % 600) - 300;// Max. 300 pixelu za sekundu
	g_vely = (rand() % 600) - 300;

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	if(g_bg_img != NULL)
		SDL_FreeSurface(g_bg_img);

	if(g_obj_img != NULL)
		SDL_FreeSurface(g_obj_img);

	SDL_Quit();
}


/*
 * Vykresleni sceny; misto prekreslovani celeho okna by bylo lepsi aktualizovat
 * jenom tu cast, ktera se zmenila
 */

void Draw()
{
	SDL_Rect rect;
	int x, y;

	// Pozadi dlazdicove pres cele okno
	for(x = 0; x < g_screen->w; x += g_bg_img->w)
	{
		for(y = 0; y < g_screen->h; y += g_bg_img->h)
		{
			rect.x = x;
			rect.y = y;
			SDL_BlitSurface(g_bg_img, NULL, g_screen, &rect);
		}
	}

	// Objekt na nove pozici
	rect.x = (Sint16)g_posx;
	rect.y = (Sint16)g_posy;
	SDL_BlitSurface(g_obj_img, NULL, g_screen, &rect);

	// Prohozeni predniho a zadniho bufferu
	SDL_Flip(g_screen);
}


/*
 * Aktualizace sceny
 */

void Update()
{
	g_posx += g_velx / GetFPS();
	g_posy += g_vely / GetFPS();

	if(g_posx < 0)
	{
		g_posx = 0;
		g_velx = -g_velx;
	}
	else if(g_posx > (g_screen->w - g_obj_img->w))
	{
		g_posx = g_screen->w - g_obj_img->w;
		g_velx = -g_velx;
	}

	if(g_posy < 0)
	{
		g_posy = 0;
		g_vely = -g_vely;
	}
	else if(g_posy > (g_screen->h - g_obj_img->h))
	{
		g_posy = g_screen->h - g_obj_img->h;
		g_vely = -g_vely;
	}

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
		CalculateFPS();
		Update();
		Draw();
	}

	// Deinicializace a konec
	Destroy();
	return 0;
}
