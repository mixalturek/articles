/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program prehrava v nekonecne smycce zvuk nahrany z .wav souboru.      *
 *   Pomoci mezerniku je mozne prehravani pozastavit a +/- meni hlasitost. *
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

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_AUDIO
#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0
#define WIN_TITLE "SDL a audio"


/*
 * Funkcni prototypy
 */

void AudioCallback(void *unused, Uint8 *stream, int len);
bool Init();					// Inicializace
void Destroy();					// Deinicializace
bool ProcessEvent();				// Osetruje udalosti
int  main(int argc, char *argv[]);		// Vstup do programu


/*
 * Globalni promenne
 */

Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE;
SDL_Surface *g_screen;				// Surface okna

Uint8	*g_sound_data;				// Ukazatel na data zvuku
Uint32	g_sound_len;				// Delka dat
int	g_sound_pos;				// Pozice pri prehravani

bool	g_sound_paused = false;			// Flag pozastaveni
int	g_sound_volume = SDL_MIX_MAXVOLUME / 2;	// Hlasitost


/*
 * Audio callback funkce, posila data do streamu
 */

void AudioCallback(void *unused, Uint8 *stream, int len)
{
	// Ukazatel na cast, kde se ma zacit prehravat
	Uint8 *wave_ptr = g_sound_data + g_sound_pos;

	// Delka zvuku do konce
	int wave_left = g_sound_len - g_sound_pos;

	// Zbyvajici delka je mensi nez pozadovana
	// Cyklus, protoze cely zvuk muze byt kratsi
	while(wave_left <= len)
	{
		// Posle data na zvukovou kartu
		SDL_MixAudio(stream, wave_ptr, wave_left, g_sound_volume);

		// Posune se o prave zapsana data
		stream += wave_left;
		len -= wave_left;

		// Od zacatku zvuku
		wave_ptr = g_sound_data;
		wave_left = g_sound_len;
		g_sound_pos = 0;
	}

	// Je jistota, ze zbyvajici cast zvuku je delsi nez pozadovana
	SDL_MixAudio(stream, wave_ptr, len, g_sound_volume);
	g_sound_pos += len;
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

	// Nahraje zvuk
	SDL_AudioSpec sound_spec;// Format zvuku

	if(SDL_LoadWAV("test.wav", &sound_spec, &g_sound_data,
			&g_sound_len) == NULL)
	{
		fprintf(stderr, "Unable to load test.wav: %s\n",
				SDL_GetError());
		return false;
	}

	// Nastavi callback funkci
	sound_spec.callback = AudioCallback;

	// Otevre audio
	if(SDL_OpenAudio(&sound_spec, NULL) < 0)
	{
		fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
		return false;
	}

	// Vypise jmeno audio driveru
	char name[32];
	printf("Audio driver: %s\n", SDL_AudioDriverName(name, 32));

	// Zacne prehravat
	SDL_PauseAudio(0);

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	// Zavre audio a uvolni data wavu
	SDL_CloseAudio();
	SDL_FreeWAV(g_sound_data);

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
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			// Pozastaveni zvuku
			case SDLK_SPACE:
				g_sound_paused = !g_sound_paused;

				if(g_sound_paused)
					SDL_PauseAudio(1);
				else
					SDL_PauseAudio(0);
				break;

			// Hlasitost
			case SDLK_KP_PLUS:
				if(g_sound_volume < SDL_MIX_MAXVOLUME-10)
					g_sound_volume += 10;
				break;

			case SDLK_KP_MINUS:
				if(g_sound_volume > 10)
					g_sound_volume -= 10;
				break;

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
