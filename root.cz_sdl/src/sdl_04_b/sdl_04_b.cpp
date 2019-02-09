/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Ukazka vytvoreni okna s overovanim vlastnosti                         *
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
 * Nasledujici parametry se budou pri vytvareni okna testovat a pokud nebudou,
 * vyhovovat, program se pokusi najit jine (validni) a zaroveò bude vypisovat
 * ruzne informace (zkuste zadat jine flagy, zaporne rozmery apod.)
 *
 * btw, pro SDL_VideoModeOK() neni narozdil od SDL_SetVideoMode() nulova
 * barevna hloubka validni
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_FLAGS SDL_HWSURFACE|SDL_FULLSCREEN
#define WIN_WIDTH 640
#define WIN_HEIGHT 480
#define WIN_BPP 32

#define WIN_TITLE "Ukazka vytvoreni okna s overovanim vlastnosti"

// Pro snadnejsi vypisy flagy "zformatovane" do retezce
#define STR_FLAGS "SDL_HWSURFACE|SDL_FULLSCREEN"


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;


/*
 * Vypise parametry vytvareneho okna
 */

void PrintParams(int width, int height, int bpp, bool success)
{
	if(success)
	{
		printf("Parameters of created window:\n"
			"\twidth  = %d\n"
			"\theight = %d\n"
			"\tbpp    = %d\n"
			"\tflags  = %s\n", width, height, bpp, STR_FLAGS);
	}
	else
	{
		fprintf(stderr, "Unable to create window\n"
			"\twidth  = %d\n"
			"\theight = %d\n"
			"\tbpp    = %d\n"
			"\tflags  = %s\n", width, height, bpp, STR_FLAGS);
	}
}


/*
 * Inicializacni funkce
 */

bool Init(int width, int height, int bpp, Uint32 flags)
{
	// Inicializace SDL
	if(SDL_Init(SDL_SUBSYSTEMS) == -1)
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n",
				SDL_GetError());
		return false;
	}

	printf("SDL was initialized\n");
	PrintParams(width, height, bpp, true);
	printf("Checking window parameters...\n");

	bpp = SDL_VideoModeOK(width, height, bpp, flags);

	if(bpp == 0)// Predane parametry nejsou v poradku
	{
		// Nejvhodnejsi barevna hloubka podle SDL
		bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

		printf("SDL_VideoModeOK(): Parameters are not valid, "
				"checking new color depth...\n");
		printf("SDL_GetVideoInfo()->vfmt->BitsPerPixel: %d\n", bpp);

		bpp = SDL_VideoModeOK(width, height, bpp, flags);

		if(bpp == 0)
		{
			printf("SDL_VideoModeOK(): Parameters are not "
					"valid, finding new dimensions\n");

			// Hodnota bpp byla prepsana volanim SDL_VideoModeOK()
			bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

			// Zkusi ziskat dostupna rozliseni podle flagu
			SDL_Rect **modes;
			modes = SDL_ListModes(NULL, flags);

			if(modes == (SDL_Rect **)0)// Zadne dostupne mody
			{
				printf("SDL_ListModes(): No modes available\n");
				return false;
			}
			else if(modes == (SDL_Rect **)-1)// Zadna omezeni
			{
				printf("SDL_ListModes(): All resolutions "
						"available, using 640x480.\n");
				width = 640;
				height = 480;
			}
			else// Pouzije maximalni dostupne rozmery
			{
				printf("SDL_ListModes(): Using maximal "
						"available dimensions\n");
				width = modes[0]->w;
				height = modes[0]->h;
			}

			// Pokud se program dostal az sem, melo by uz byt
			// vse v poradku, ale pro jistotu test novych rozliseni

			printf("Everything should be OK now, but some "
					"other check would be great :]\n");
			PrintParams(width, height, bpp, true);

			bpp = SDL_VideoModeOK(width, height, bpp, flags);

			if(bpp == 0)
			{
				printf("SDL_VideoModeOK(): Something is still "
						"not valid :-(((((\n");
				PrintParams(width, height, bpp, false);
				return false;
			}
		}
	}

	printf("SDL_VideoModeOK(): Parameters are valid\n");
	printf("Creating window...\n");

	g_screen = SDL_SetVideoMode(width, height, bpp, flags);

	if(g_screen == NULL)
	{
		printf("Window creation failed\n");
		PrintParams(width, height, bpp, false);
		fprintf(stderr, "SDL_GetError(): %s\n", SDL_GetError());
		return false;
	}

	printf("Window was successfuly created\n");
	PrintParams(width, height, bpp, true);

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
 * Osetruje udalosti (bude probrano nekdy v budoucnu)
 * V tomto pripade se ceka na klavesu ESC, ktera ukonci program
 */

bool ProcessEvent()
{
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
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
	printf("\nPress ESC key to quit (after window is created).\n");

	// Inicializace
	if(!Init(WIN_WIDTH, WIN_HEIGHT, WIN_BPP, WIN_FLAGS))
	{
		Destroy();
		return 1;
	}

	// Hlavni smycka programu
	bool done = false;
	while(!done)
	{
		done = !ProcessEvent();
		// TODO: Tady by se vykreslovalo
	}

	// Deinicializace a konec
	Destroy();
	return 0;
}
