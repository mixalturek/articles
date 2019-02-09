/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program rozsiruje ukazkovy priklad ze 16. dilu (hra ve stylu          *
 *   Pacmana). Do sceny jsou pridany objekty, ktere ma hrac za ukol        *
 *   sbirat (reset pomoci R) a nejake ty zvuky. Hudba je volitelna,        *
 *   staci odkomentovat jedno define a nastavit cestu k libovolnemu        *
 *   souboru.                                                              *
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
#include <time.h>// Kvuli srand()
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>// -lSDL_image
#include <SDL_mixer.h>// -lSDL_mixer


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_TIMER

#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0
#define WIN_TITLE "Zvuky ve hre"

// Natoceni objektu
#define STOP	0
#define RIGHT	1
#define LEFT	2
#define UP	3
#define DOWN	4

// Uzivatelske udalosti casovace
#define USR_EVT_MOVE 0

// Velikost hraci plochy
#define WIDTH 20
#define HEIGHT 15

// Objekty
#define NUM_OBJS 10
#define OBJ_WIDTH 16
#define OBJ_HEIGHT 16
#define MAX_OBJS_IN_SCENE 100
#define NO_OBJECT 0

// Hudba, staci odkomentovat a nastavit cestu...
// #define MUSIC_PATH "music.ogg"


/*
 * Funkcni prototypy
 */

Uint32 TimerMoveCallback(Uint32 interval, void* param);	// Callback casovace
bool Init();						// Inicializace
void Destroy();						// Deinicializace
void Draw();						// Vykresleni
bool ProcessEvent();					// Osetruje udalosti
int  main(int argc, char *argv[]);			// Vstup do programu

void ResetPlayground();					// Resetuje objekty

bool ToggleFullscreen();				// Zmena okno/fullscreen
SDL_Surface *LoadImage(const char *filename, bool alpha = false);


/*
 * Globalni promenne
 */

Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF;
SDL_Surface *g_screen;			// Surface okna
SDL_Surface *g_bg;			// Surface pozadi
SDL_Surface *g_obj[NUM_OBJS];		// Surface objektu (hrac na indexu 0)

Uint8 g_playground[WIDTH][HEIGHT];	// Hraci hriste
int g_xpos = 0, g_ypos = 0;		// Pozice hrace v hristi
int g_dir = RIGHT;			// Smer pohybu

SDL_TimerID g_timer_move_id = NULL;	// Casovac pohybu
int g_interval = 200;			// Interval [ms]

// Zvuky
Mix_Chunk *g_obj_snd = NULL;
Mix_Chunk *g_move_snd = NULL;
Mix_Chunk *g_stop_snd = NULL;
#ifdef MUSIC_PATH
Mix_Music *g_music = NULL;
#endif


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

	if((g_obj_snd = Mix_LoadWAV("obj.wav")) == NULL
			|| (g_move_snd = Mix_LoadWAV("move.wav")) == NULL
			|| (g_stop_snd = Mix_LoadWAV("stop.wav")) == NULL)
	{
		printf("Unable to load sound: %s\n", Mix_GetError());
		return false;
	}

	Mix_VolumeChunk(g_move_snd, MIX_MAX_VOLUME / 3);// Moc hlasity

#ifdef MUSIC_PATH
	if((g_music = Mix_LoadMUS(MUSIC_PATH)) == NULL)
	{
		printf("Unable to load music: %s\n", Mix_GetError());
		return false;
	}
	Mix_PlayMusic(g_music, -1);
#endif

	// Loading obrazku
	int i;						// Cykly
	char tmp[20];					// Pomocny

	if((g_bg = LoadImage("bg.png", false)) == NULL)// Pozadi
		return false;

	for(i = 0; i < NUM_OBJS; i++)			// Pro jistotu
		g_obj[i] = NULL;

	for(i = 0; i < NUM_OBJS; i++)
	{
		sprintf(tmp, "%d.png", i);

		if((g_obj[i] = LoadImage(tmp, true)) == NULL)
			return false;
	}

	ResetPlayground();				// Reset objektu

	// Spusteni casovace
	g_timer_move_id = SDL_AddTimer(g_interval, TimerMoveCallback, NULL);

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	// Zastaveni casovace
	if(g_timer_move_id != NULL)
		SDL_RemoveTimer(g_timer_move_id);

	// Obrazky
	if(g_bg != NULL)
		SDL_FreeSurface(g_bg);

	for(int i = 0; i < NUM_OBJS; i++)
	{
		if(g_obj[i] != NULL)
		{
			SDL_FreeSurface(g_obj[i]);
			g_obj[i] = NULL;
		}
	}

	// Zvuky
	if(g_obj_snd != NULL)
	{
		Mix_FreeChunk(g_obj_snd);
		g_obj_snd = NULL;
	}
	if(g_move_snd != NULL)
	{
		Mix_FreeChunk(g_move_snd);
		g_move_snd = NULL;
	}
	if(g_stop_snd != NULL)
	{
		Mix_FreeChunk(g_stop_snd);
		g_stop_snd = NULL;
	}

#ifdef MUSIC_PATH
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
#endif

	Mix_CloseAudio();

	SDL_Quit();
}


/*
 * Callback funkce casovace, posle uzivatelskou udalost
 */

Uint32 TimerMoveCallback(Uint32 interval, void* param)
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = USR_EVT_MOVE;
	event.user.data1 = NULL;
	event.user.data2 = NULL;

	SDL_PushEvent(&event);

	return g_interval;
}


/*
 * Resetuje objekty na hracim hristi, vse nahodne
 */

void ResetPlayground()
{
	int i, j;

	for(i = 0; i < WIDTH; i++)
		for(j = 0; j < HEIGHT; j++)
			g_playground[i][j] = NO_OBJECT;

	for(i = 0; i < MAX_OBJS_IN_SCENE; i++)
	{
		g_playground[rand() % WIDTH][rand() % HEIGHT]
				= (rand() % NUM_OBJS) + 1;// 1. obrazek je hrac
	}
}


/*
 * Vykresleni sceny
 */

void Draw()
{
	SDL_Rect src_rect, dst_rect;
	int x, y;

	// Pozadi dlazdicove pres cele okno
	for(x = 0; x < g_screen->w; x += g_bg->w)
	{
		for(y = 0; y < g_screen->h; y += g_bg->h)
		{
			dst_rect.x = x;
			dst_rect.y = y;
			SDL_BlitSurface(g_bg, NULL, g_screen, &dst_rect);
		}
	}

	// Objekty
	for(x = 0; x < WIDTH; x ++)
	{
		for(y = 0; y < HEIGHT; y++)
		{
			if(g_playground[x][y] != NO_OBJECT)
			{
				dst_rect.x = x * OBJ_WIDTH;
				dst_rect.y = y * OBJ_HEIGHT;

				SDL_BlitSurface(g_obj[g_playground[x][y]],
						NULL, g_screen, &dst_rect);
			}
		}
	}

	// Hrac
	src_rect.x = g_dir * g_obj[0]->w / 5;
	src_rect.y = 0;
	src_rect.w = g_obj[0]->w / 5;
	src_rect.h = g_obj[0]->h;

	dst_rect.x = g_xpos * g_obj[0]->w / 5;
	dst_rect.y = g_ypos * g_obj[0]->h;
	SDL_BlitSurface(g_obj[0], &src_rect, g_screen, &dst_rect);

	// Prohozeni predniho a zadniho bufferu
	SDL_Flip(g_screen);
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
			// Pohyby
			case SDLK_UP: g_dir = UP; break;
			case SDLK_DOWN: g_dir = DOWN; break;
			case SDLK_LEFT: g_dir = LEFT; break;
			case SDLK_RIGHT: g_dir = RIGHT; break;

			// Zrychleni
			case SDLK_KP_PLUS:
				if(g_interval > 20)
					g_interval -= 10;
				break;

			// Zpomaleni
			case SDLK_KP_MINUS:
				g_interval += 10;
				break;

			// Reset objektu
			case SDLK_r:
				ResetPlayground();
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

		// Uzivatelske udalosti (timer)
		case SDL_USEREVENT:
			switch(event.user.code)
			{
			case USR_EVT_MOVE:
				if(g_dir != STOP)
					Mix_PlayChannel(-1, g_move_snd, 0);

				switch(g_dir)
				{
				case UP:
					if(g_ypos <= 0)
					{
						g_dir = STOP;
						Mix_PlayChannel(-1, g_stop_snd,
								0);
					}
					else
						g_ypos--;
					break;

				case DOWN:
					if(g_ypos*g_obj[0]->h >= g_screen->h
							- g_obj[0]->h)
					{
						g_dir = STOP;
						Mix_PlayChannel(-1, g_stop_snd,
								0);
					}
					else
						g_ypos++;
					break;

				case LEFT:
					if(g_xpos <= 0)
					{
						g_dir = STOP;
						Mix_PlayChannel(-1, g_stop_snd,
								0);
					}
					else
						g_xpos--;
					break;

				case RIGHT:
					if(g_xpos*g_obj[0]->w/5 >= g_screen->w
							- g_obj[0]->w/5)
					{
						g_dir = STOP;
						Mix_PlayChannel(-1, g_stop_snd,
								0);
					}
					else
						g_xpos++;
					break;

				default:
					break;
				}

				if(g_playground[g_xpos][g_ypos] != NO_OBJECT)
				{
					g_playground[g_xpos][g_ypos]= NO_OBJECT;
					Mix_PlayChannel(-1, g_obj_snd, 0);
				}
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
		Draw();			// Vykresleni
	}

	// Deinicializace a konec
	Destroy();
	return 0;
}


////////////////////////////////////////////////////////////////
//                     "Standardni funkce"                    //
////////////////////////////////////////////////////////////////


/*
 * Funkce se pokusi nahrat obrazek ze souboru a zkonvertovat ho
 * na stejny pixel format, jako ma okno (framebuffer)
 */

SDL_Surface *LoadImage(const char *filename, bool alpha)
{
	SDL_Surface *tmp;// Pomocny
	SDL_Surface *ret;// Bude vracen

	if((tmp = IMG_Load(filename)) == NULL)// Prilinkovat SDL_image
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		return NULL;
	}

	if((ret = (alpha) ? SDL_DisplayFormatAlpha(tmp)
			: SDL_DisplayFormat(tmp)) == NULL)
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_FreeSurface(tmp);
		return NULL;
	}

	SDL_FreeSurface(tmp);// Uz neni potreba

	return ret;
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
