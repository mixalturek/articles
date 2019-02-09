/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Aplikace inicilizuje SDL_mixer, otevre audio zarizeni a nahraje       *
 *   zvuk. Zacatek prehravani je umoznen stiskem mezerniku a konec pomoci  *
 *   enteru. V obou pripadech je ponechana doba tri sekund na postupne     *
 *   nabirani na sile resp. odezneni.                                      *
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
#include <string.h>
#include <SDL.h>
#include <SDL_mixer.h>// Nezapomenout prilinkovat -lSDL_mixer


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_AUDIO
#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0
#define WIN_TITLE "SDL_mixer"


/*
 * Funkcni prototypy
 */

bool Init();					// Inicializace
void Destroy();					// Deinicializace
bool ProcessEvent();				// Osetruje udalosti
int  main(int argc, char *argv[]);		// Vstup do programu


/*
 * Globalni promenne
 */

Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE;
SDL_Surface *g_screen;

Mix_Chunk *g_sound = NULL;			// Prehravany zvuk


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

	// Inicializuje SDL_mixer a otevre audio zarizeni
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,
			MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Unable to initializace SDL_mixer: %s\n",
				Mix_GetError());
		return false;
	}

	// Nahraje zvuk do aplikace
	g_sound = Mix_LoadWAV("test.wav");
	if(g_sound == NULL)
	{
		fprintf(stderr, "Unable to load sound: %s\n",
				Mix_GetError());
		return false;
	}

	// Vypise parametry audia
	int numtimesopened, frequency, channels;
	Uint16 format;

	numtimesopened = Mix_QuerySpec(&frequency, &format, &channels);

	if(numtimesopened == 0)
	{
		printf("Mix_QuerySpec(): %s\n", Mix_GetError());
	}
	else
	{
		char format_str[10];
		switch(format)
		{
			case AUDIO_U8: strcpy(format_str, "U8"); break;
			case AUDIO_S8: strcpy(format_str, "S8"); break;
			case AUDIO_U16LSB: strcpy(format_str, "U16LSB"); break;
			case AUDIO_S16LSB: strcpy(format_str, "S16LSB"); break;
			case AUDIO_U16MSB: strcpy(format_str, "U16MSB"); break;
			case AUDIO_S16MSB: strcpy(format_str, "S16MSB"); break;
			default: strcpy(format_str, "unknown"); break;
		}

		printf("\nfrequency = %d Hz\nformat = %s\nchannels = %d\n",
			frequency, format_str, channels);
	}

	// Zacne prehravat
	Mix_FadeInChannel(0, g_sound, -1, 3000);

	// Titulek okna
	SDL_WM_SetCaption(WIN_TITLE, NULL);

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	// Uvolni zvuk
	if(g_sound != NULL)
	{
		Mix_FreeChunk(g_sound);
		g_sound = NULL;
	}

	Mix_CloseAudio();		// Deinicializuje SDL_mixer
	SDL_Quit();			// Deinicializuje SDL
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
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				return false;
				break;

			// Zacne prehravat
			case SDLK_SPACE:
				Mix_FadeInChannel(0, g_sound, 100, 3000);
				break;

			// Ukonci prehravani
			case SDLK_RETURN:
				Mix_FadeOutChannel(0,3000);
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
