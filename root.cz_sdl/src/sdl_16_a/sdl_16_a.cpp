/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program tvori zaklad pro hru ve stylu Pacmana. Na pozadi je           *
 *   dlazdicove vykreslena mrizka, ve ktere se pohybuje hrac ovladany      *
 *   sipkami. Pohyby jsou implementovany pomoci systemoveho timeru.        *
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
#include <SDL_image.h>


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_TIMER

#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0

#define WIN_TITLE "Systemovy casovac"

// Natoceni objektu
#define STOP	0
#define RIGHT	1
#define LEFT	2
#define UP	3
#define DOWN	4

// Uzivatelska udalost casovace
#define USR_EVT_TIMER 0


/*
 * Funkcni prototypy
 */

Uint32 TimerCallback(Uint32 interval, void* param);	// Callback casovace
bool Init();						// Inicializace
void Destroy();						// Deinicializace
void Draw();						// Vykresleni
bool ProcessEvent();					// Osetruje udalosti
int  main(int argc, char *argv[]);			// Vstup do programu

bool ToggleFullscreen();				// Zmena okno/fullscreen
SDL_Surface *LoadImage(const char *filename, bool alpha = false);


/*
 * Globalni promenne
 */

Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE;
SDL_Surface *g_screen;			// Surface okna
SDL_Surface *g_bg;			// Surface pozadi
SDL_Surface *g_obj;			// Surface objektu

int g_xpos = 0, g_ypos = 0;		// Pozice (v mrizce, ne v okne)
int g_dir = RIGHT;			// Smer pohybu

SDL_TimerID g_timer_id = NULL;		// Casovac
int g_interval = 200;			// Interval [ms]


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

	// Loading obrazku
	if(((g_bg = LoadImage("bg.png", false)) == NULL)
			|| ((g_obj = LoadImage("obj.png", true)) == NULL))
	{
		return false;
	}

	// Spusteni casovace
	g_timer_id = SDL_AddTimer(g_interval, TimerCallback, NULL);

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	// Zastaveni casovace
	if(g_timer_id != NULL)
		SDL_RemoveTimer(g_timer_id);

	if(g_bg != NULL)
		SDL_FreeSurface(g_bg);

	if(g_obj != NULL)
		SDL_FreeSurface(g_obj);

	SDL_Quit();
}


/*
 * Callback funkce casovace, posle uzivatelskou udalost
 */

Uint32 TimerCallback(Uint32 interval, void* param)
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = USR_EVT_TIMER;
	event.user.data1 = NULL;
	event.user.data2 = NULL;

	SDL_PushEvent(&event);

	return g_interval;
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

	// Objekt
	src_rect.x = g_dir * g_obj->w / 5;
	src_rect.y = 0;
	src_rect.w = g_obj->w / 5;
	src_rect.h = g_obj->h;

	dst_rect.x = g_xpos * g_obj->w / 5;
	dst_rect.y = g_ypos * g_obj->h;
	SDL_BlitSurface(g_obj, &src_rect, g_screen, &dst_rect);

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
			case USR_EVT_TIMER:
				switch(g_dir)
				{
				case UP:
					if(g_ypos <= 0)
						g_dir = STOP;
					else
						g_ypos--;
					break;

				case DOWN:
					if(g_ypos*g_obj->h >= g_screen->h
							- g_obj->h)
						g_dir = STOP;
					else
						g_ypos++;
					break;

				case LEFT:
					if(g_xpos <= 0)
						g_dir = STOP;
					else
						g_xpos--;
					break;

				case RIGHT:
					if(g_xpos*g_obj->w/5 >= g_screen->w
							- g_obj->w/5)
						g_dir = STOP;
					else
						g_xpos++;
					break;

				default:
					break;
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
