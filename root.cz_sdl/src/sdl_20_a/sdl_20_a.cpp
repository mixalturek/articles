/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program demonstruje prehravani hudby a hlavne zabudovane efekty       *
 *   v SDL_mixeru. Cesta k hudbe se predava jako parametr programu.        *
 *   Ovladani: [+/-] - hlasitost                                           *
 *             [mezernik] - prohodi levy a pravy kanal                     *
 *             [sipka doleva/doprava] - vystup z leveho/praveho kanalu     *
 *             [sipka nahoru/dolu] - vzdalenost od zdroje zvuku            *
 *             [1,2,3,4,6,7,8,9] - pozice zdroje zvuku                     *
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
#include <SDL_mixer.h>// -lSDL_mixer


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_TIMER

#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0
#define WIN_TITLE "Hudba a efekty"


/*
 * Funkcni prototypy
 */

bool Init();						// Inicializace
void Destroy();						// Deinicializace
void Draw();						// Vykresleni
bool ProcessEvent();					// Osetruje udalosti
int  main(int argc, char *argv[]);			// Vstup do programu
bool ToggleFullscreen();				// Zmena okno/fullscreen


/*
 * Globalni promenne
 */

Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF;
SDL_Surface *g_screen;			// Surface okna

Mix_Music *g_music = NULL;		// Hudba
int g_volume = MIX_MAX_VOLUME;		// Hlasitost
int g_volume_left = 128;		// Hlasitost leveho kanalu
int g_sound_distance = 128;		// Vzdalenost od zdroje zvuku
bool g_reverse_stereo = false;		// Prohodi levy a pravy kanal


/*
 * Inicializacni funkce
 */

bool Init(char *mus_filename)
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

	// Audio
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,
			MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Unable to open audio: %s\n", Mix_GetError());
		return false;
	}

	if((g_music = Mix_LoadMUS(mus_filename)) == NULL)
	{
		printf("Unable to load music: %s\n", Mix_GetError());
		return false;
	}

	Mix_VolumeMusic(g_volume);
	Mix_PlayMusic(g_music, -1);

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	// Vterinove odezneni
	if(Mix_PlayingMusic())
	{
		Mix_FadeOutMusic(1000);
		SDL_Delay(1000);
	}

	if(g_music)
	{
		Mix_FreeMusic(g_music);
		g_music = NULL;
	}

	Mix_CloseAudio();

	SDL_Quit();
}


/*
 * Vykresleni sceny
 */

void Draw()
{
	// Nic se nekresli
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
			// Prohodi levy a pravy kanal
			case SDLK_SPACE:
				g_reverse_stereo = !g_reverse_stereo;
				Mix_SetReverseStereo(MIX_CHANNEL_POST,
						g_reverse_stereo ? 1 : 0);
				break;

			// Zvysi hlasitost
			case SDLK_KP_PLUS:
				if(g_volume < MIX_MAX_VOLUME-11)
				{
					g_volume += 10;
					Mix_VolumeMusic(g_volume);
				}
				break;

			// Snizi hlasitost
			case SDLK_KP_MINUS:
				if(g_volume > 10)
				{
					g_volume -= 10;
					Mix_VolumeMusic(g_volume);
				}
				break;

			// Zvysi hlasitost leveho kanalu
			case SDLK_LEFT:
				if(g_volume_left < 245)
				{
					g_volume_left += 10;
					Mix_SetPanning(MIX_CHANNEL_POST,
							g_volume_left,
							255 - g_volume_left);
				}
				break;

			// Zvysi hlasitost praveho kanalu
			case SDLK_RIGHT:
				if(g_volume_left > 10)
				{
					g_volume_left -= 10;
					Mix_SetPanning(MIX_CHANNEL_POST,
							g_volume_left,
							255 - g_volume_left);
				}
				break;

			// Snizi vzdalenost od zdroje zvuku
			case SDLK_UP:
				if(g_sound_distance > 10)
				{
					g_sound_distance -= 10;
					Mix_SetDistance(MIX_CHANNEL_POST,
							g_sound_distance);
				}
				break;

			// Zvysi vzdalenost od zdroje zvuku
			case SDLK_DOWN:
				if(g_sound_distance < 245)
				{
					g_sound_distance += 10;
					Mix_SetDistance(MIX_CHANNEL_POST,
							g_sound_distance);
				}
				break;

			// Zvuk zepredu
			case SDLK_KP8:
				Mix_SetPosition(MIX_CHANNEL_POST,
						0, g_sound_distance);
				break;

			// Zvuk zepredu zprava
			case SDLK_KP9:
				Mix_SetPosition(MIX_CHANNEL_POST,
						45, g_sound_distance);
				break;

			// Zvuk zprava
			case SDLK_KP6:
				Mix_SetPosition(MIX_CHANNEL_POST,
						90, g_sound_distance);
				break;

			// Zvuk zezadu zprava
			case SDLK_KP3:
				Mix_SetPosition(MIX_CHANNEL_POST,
						135, g_sound_distance);
				break;

			// Zvuk zezadu
			case SDLK_KP2:
				Mix_SetPosition(MIX_CHANNEL_POST,
						180, g_sound_distance);
				break;

			// Zvuk zezadu zleva
			case SDLK_KP1:
				Mix_SetPosition(MIX_CHANNEL_POST,
						235, g_sound_distance);
				break;

			// Zvuk zleva
			case SDLK_KP4:
				Mix_SetPosition(MIX_CHANNEL_POST,
						270, g_sound_distance);
				break;

			// Zvuk zepredu zleva
			case SDLK_KP7:
				Mix_SetPosition(MIX_CHANNEL_POST,
						315, g_sound_distance);
				break;

			case SDLK_ESCAPE:
				return false;
				break;

			case SDLK_F1:
				if(!ToggleFullscreen())
					return false;
				break;

			default:
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

		Draw();
	}

	return true;
}


/*
 * Vstup do programu
 */

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s music_file\n", argv[0]);
		return 1;
	}

	printf(WIN_TITLE);
	printf("\nPress ESC key to quit.\n");

	// Inicializace
	if(!Init(argv[1]))
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


////////////////////////////////////////////////////////////////
//                     "Standardni funkce"                    //
////////////////////////////////////////////////////////////////

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
