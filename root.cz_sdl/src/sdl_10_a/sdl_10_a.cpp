/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program ukazuje komunikaci s okennim manazerem pomoci klavesnice      *
 *   Ovladani: Escape - konec aplikace                                     *
 *             F1     - prepnuti fullscreen/kno                            *
 *             M      - minimalizace okna                                  *
 *             G      - zmeni zpùsob grabovani vstupu (SDL_GrabInput())    *
 *             jina   - vypise "cislo" a jmeno klavesy                     *
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
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0

#define WIN_TITLE "Uvod do udalosti"


/*
 * Funkcni prototypy
 */

bool Init();				// Inicializace
void Destroy();				// Deinicializace
void Draw();				// Vykresleni
bool ProcessEvent();			// Osetruje udalosti
bool ToggleFullscreen();		// Zmena okno/fullscreen
int  main(int argc, char *argv[]);	// Vstup do programu


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;			// Surface okna
// Flagy okna, tentokrat musi byt v promenne (viz ToggleFullscreen())
Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE;


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

	// Nastaveni ikony
	SDL_Surface *icon = SDL_LoadBMP("./icon.bmp");
	if(icon != NULL)
	{
		SDL_WM_SetIcon(icon, NULL);
		SDL_FreeSurface(icon);
	}

	// Vytvori okno s definovanymi vlastnostmi
	g_screen = SDL_SetVideoMode(WIN_WIDTH, WIN_HEIGHT, WIN_BPP, g_win_flags);
	if(g_screen == NULL)
	{
		fprintf(stderr, "Unable to set %dx%d video: %s\n",
				WIN_WIDTH, WIN_HEIGHT, SDL_GetError());
		return false;
	}

	// Titulek okna
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
	// Nic se nevykresluje
}


/*
 * Osetreni udalosti
 */

bool ProcessEvent()
{
	// Objekt udalosti
	SDL_Event event;

	// Zpracovava udalosti tak dlouho, dokud je jeste nejaka ve fronte
	while(SDL_PollEvent(&event))
	{
		// Vetvi podle typu udalosti
		switch(event.type)
		{
		// Klavesnice
		case SDL_KEYDOWN:
			// Vetvi podle klavesy
			switch(event.key.keysym.sym)
			{
			// Esc - konec aplikace
			case SDLK_ESCAPE:
				return false;
				break;

			// F1 - prepnuti fullscreen/okno
			case SDLK_F1:
				if(!ToggleFullscreen())
					return false;
				break;

			// M - minimalizace okna
			case SDLK_m:
				SDL_WM_IconifyWindow();
				break;

			// G - zmeni typ grabovani vstupu
			case SDLK_g:
				if(SDL_WM_GrabInput(SDL_GRAB_QUERY)
						== SDL_GRAB_ON)
					SDL_WM_GrabInput(SDL_GRAB_OFF);
				else
					SDL_WM_GrabInput(SDL_GRAB_ON);
				break;

			// Jina - vypise "cislo" klavesy a jeji jmeno
			default:
				printf("%d - %s\n", event.key.keysym.sym,
					SDL_GetKeyName(event.key.keysym.sym));
				break;
			}
			break;

		// Zmena velikosti okna
		case SDL_VIDEORESIZE:
			g_screen = SDL_SetVideoMode(event.resize.w,
					event.resize.h, WIN_BPP, g_win_flags);

			if(g_screen == NULL)
			{
				fprintf(stderr, "Unable to resize window: %s\n",
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
 * Zmena modu okno/fullscreen (melo by fungovat vsude, nejen pod X11)
 */

bool ToggleFullscreen()
{
	// Negace priznakoveho bitu ve flagach
	if(g_win_flags & SDL_FULLSCREEN)// Z fullscreenu do okna
		g_win_flags &= ~SDL_FULLSCREEN;
	else// Z okna do fullscreenu
		g_win_flags |= SDL_FULLSCREEN;

	// Pokus o prepnuti
	if(SDL_WM_ToggleFullScreen(g_screen) == 0)
	{
		fprintf(stderr, "Unable to toggle fullscreen,"
				"trying to recreate window\n");

		SDL_FreeSurface(g_screen);
		g_screen = SDL_SetVideoMode(WIN_WIDTH, WIN_HEIGHT,
				WIN_BPP, g_win_flags);

		if(g_screen == NULL)
		{
			fprintf(stderr, "Unable to recreate window: %s\n",
					SDL_GetError());
			return false;// Ukonci program
		}

		Draw();// Prekresli scenu
	}

	return true;// OK
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
