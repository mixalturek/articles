/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program demonstruje zakaz generovani udalosti funkcemi                *
 *   SDL_EventState() a SDL_SetEventFilter() a take programove vkladani    *
 *   udalosti do fronty pomoci SDL_PushEvent(). Pri prichodu udalosti,     *
 *   ktere mohou byt programem zakazany, se vypisuje zprava do konzole.    *
 *   Jsou-li v dane chvili zakazany (prepinani mezernikem), nic se         *
 *   samozrejme nevypise.                                                  *
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

#define WIN_TITLE "Fronta udalosti"


/*
 * Funkcni prototypy
 */

void PushQuitEvent();			// Vlozi do fronty SDL_QUIT
void IgnoreKeyUp();			// Povoli/zrusi uvolneni klavesy
int  EventFilter(const SDL_Event *event);// Filtr udalosti

bool Init();				// Inicializace
void Destroy();				// Deinicializace
bool ProcessEvent();			// Osetruje udalosti
bool ToggleFullscreen();		// Zmena okno/fullscreen
int  main(int argc, char *argv[]);	// Vstup do programu


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;			// Surface okna
// Flagy okna
Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE;


/*
 * Posle pozadavek na ukonceni
 * (vlozi do fronty udalost SDL_QUIT)
 */


void PushQuitEvent()
{
	SDL_Event event;	// Vytvoreni udalosti
	event.type = SDL_QUIT;	// Nastaveni parametru
	SDL_PushEvent(&event);	// Vlozeni do fronty
}


/*
 * Povoli/zrusi vkladani udalosti o uvolneni klavesy
 */

void IgnoreKeyUp()
{
	// Dotaz na aktualni stav
	if(SDL_EventState(SDL_KEYUP, SDL_QUERY) == SDL_ENABLE)
	{
		// Zrusi vkladani
		SDL_EventState(SDL_KEYUP, SDL_IGNORE);
	}
	else
	{
		// Povoli vkladani
		SDL_EventState(SDL_KEYUP, SDL_ENABLE);
	}
}


/*
 * Filtr udalosti (aktivovan na konci Init())
 * Neumozni vlozeni udalosti tykajicich se mysi do fronty
 * V tomto pripade by bylo mozna vhodnejsi pouzit SDL_EventState()
 */

int EventFilter(const SDL_Event *event)
{
	switch(event->type)
	{
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		// Zde muze byt navic test parametru

		return 0;// Nevkladat do fronty
		break;

	default:
		break;
	}

	return 1;// Vlozit do fronty
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

	// Nastaveni filtru udalosti
	SDL_SetEventFilter(EventFilter);

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
 * Osetreni udalosti
 */

bool ProcessEvent()
{
	SDL_Event event;

	while(SDL_WaitEvent(&event))
	{
		switch(event.type)
		{
		// Stisk klavesy
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			// Konec programu
			case SDLK_ESCAPE:
			case SDLK_q:
				PushQuitEvent();
				break;

			// Zapne/vypne ignorovani mysi
			case SDLK_SPACE:
				IgnoreKeyUp();
				break;

			// Prepnuti do/z fullscreenu
			case SDLK_F1:
			case SDLK_f:
				if(!ToggleFullscreen())
					return false;
				break;

			// Minimalizace okna
			case SDLK_m:
				SDL_WM_IconifyWindow();
				break;

			default:
				break;
			}
			break;

		// Uvolneni klavesy, pouze vypis do konzole, ze nastala
		case SDL_KEYUP:
			printf("SDL_KEYUP "); fflush(stdout);
			break;

		// Udalosti mysi, nemely by nikdy nastat (viz EventFilter())
		case SDL_MOUSEMOTION:
			printf("SDL_MOUSEMOTION "); fflush(stdout);
			break;
		case SDL_MOUSEBUTTONDOWN:
			printf("SDL_MOUSEBUTTONDOWN "); fflush(stdout);
			break;
		case SDL_MOUSEBUTTONUP:
			printf("SDL_MOUSEBUTTONUP "); fflush(stdout);
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
	}

	// Deinicializace a konec
	Destroy();
	return 0;
}
