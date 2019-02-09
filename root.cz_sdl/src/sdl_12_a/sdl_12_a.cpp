/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program vykresluje objekt, se kterym je mozno sipkami pohybovat.      *
 *   Stiskem se nemeni poloha primo, ale pres rychlost je aplikovano       *
 *   zrychleni. V kazdem pruchodu se, nezavisle na stisku sipek, aplikuje  *
 *   gravitace. V pripade, ze objekt narazi do steny (okraj okna),         *
 *   odrazi se a jeho rychlost je zmensena. Jako bonus:-) kdyz se          *
 *   na klavesnici natuka urcita posloupnost klaves ("cheat"), bude        *
 *   aplikovan cheat...                                                    *
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

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0

#define WIN_TITLE "Odrazy"


/*
 * Funkcni prototypy
 */

bool Init();				// Inicializace
void Destroy();				// Deinicializace
void Draw();				// Vykresleni
void Update();				// Aktualizace sceny
bool ProcessEvent();			// Osetruje udalosti
int  main(int argc, char *argv[]);	// Vstup do programu

void CalculateFPS();			// Vypocet FPS
bool ToggleFullscreen();		// Zmena okno/fullscreen
SDL_Surface *LoadImage(const char *filename, bool alpha = false);


/*
 * Globalni promenne
 */

// Grafika
Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF;
SDL_Surface *g_screen;			// Surface okna
SDL_Surface *g_bg;			// Surface pozadi
SDL_Surface *g_obj;			// Surface objektu

// Pohyby
float g_xpos = 0, g_ypos = 0;		// Pozice
float g_xvel = 20, g_yvel = 0;		// Rychlost
const float g_xacc = 60, g_yacc = 60;	// Zrychleni

// Cheaty
bool g_cheat_flag = false;		// Flag cheatu
char g_cheat_str[] = "cheat";		// Retezec cheatu
unsigned int g_cheat_pos = 0;		// Pozice v retezci

// Pomocne prov vypocet FPS
float g_fps = 1.0f;			// Aktualni FPS
Uint32 g_last_time = 0;			// Cas minuleho prekresleni


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
	if(((g_bg = LoadImage("bg.jpg", false)) == NULL)
			|| ((g_obj = LoadImage("obj.png", true)) == NULL))
	{
		return false;
	}

	g_last_time = SDL_GetTicks();

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	if(g_bg != NULL)
		SDL_FreeSurface(g_bg);

	if(g_obj != NULL)
		SDL_FreeSurface(g_obj);

	SDL_Quit();
}


/*
 * Vykresleni sceny
 */

void Draw()
{
	SDL_Rect rect = { (short int)g_xpos, (short int)g_ypos,
			g_obj->w, g_obj->h };

	if(!g_cheat_flag)
		SDL_BlitSurface(g_bg, NULL, g_screen, NULL);


	SDL_BlitSurface(g_obj, NULL, g_screen, &rect);
	SDL_Flip(g_screen);
}

void Update()
{
	SDL_PumpEvents();

	Uint8* keys;
	keys = SDL_GetKeyState(NULL);

	// Sipkami se ovlada rychlost objektu
	// (aplikace zrychleni na rychlost)
	if(keys[SDLK_RIGHT] == SDL_PRESSED)
		g_xvel += g_xacc / g_fps;
	if(keys[SDLK_LEFT] == SDL_PRESSED)
		g_xvel -= g_xacc / g_fps;
	if(keys[SDLK_UP] == SDL_PRESSED)
		g_yvel -= g_yacc / g_fps;
	if(keys[SDLK_DOWN] == SDL_PRESSED)
		g_yvel += g_yacc / g_fps;

	// Gravitace
	g_yvel += 40.0f / g_fps;

	// Zmena pozice o rychlost
	g_xpos += g_xvel / g_fps;
	g_ypos += g_yvel / g_fps;

	// Naraz do steny, odraz a zmenseni rychlosti
	if(g_xpos < 0)
	{
		g_xpos = 0;
		g_xvel /= -1.5f;
		// Bez deleni fps - jednorazova akce...
	}
	if(g_xpos > g_screen->w - g_obj->w)
	{
		g_xpos = g_screen->w - g_obj->w;
		g_xvel /= -1.5f;
	}
	if(g_ypos < 0)
	{
		g_ypos = 0;
		g_yvel /= -1.5f;
	}
	if(g_ypos > g_screen->h - g_obj->h)
	{
		g_ypos = g_screen->h - g_obj->h;
		g_yvel /= -1.5f;
	}
}


/*
 * Osetreni udalosti
 */

bool ProcessEvent()
{
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		// Stisk klavesy
		case SDL_KEYDOWN:
			// CHEATY
			// Spravna klavesa v posloupnosti
			if(g_cheat_str[g_cheat_pos] == event.key.keysym.sym)
			{
				// Cela posloupnost spravne
				if(++g_cheat_pos == strlen(g_cheat_str))
				{
					// Aplikace cheatu
					g_cheat_flag = !g_cheat_flag;
					g_cheat_pos = 0;// Pristi pruchod
				}
			}
			else// Spatny cheat
			{
				g_cheat_pos = 0;
			}

			// Klasicke vetveni podle klavesy
			switch(event.key.keysym.sym)
			{
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
		CalculateFPS();		// Vypocet FPS
		Update();		// Aktualizace sceny
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
 * Vypocet poctu snimku za sekundu, pohyby budou nezavisle na rychlosti pocitace
 * Pri ~100 fps, prenechava cas jinym procesum
 */

void CalculateFPS()
{
	// SDL_GetTicks() vraci cas v sekundach od inicializace SDL
	Uint32 miliseconds = SDL_GetTicks() - g_last_time;

	if(miliseconds < 10)// Max ~100 fps
	{
		SDL_Delay(10-miliseconds);// Cas pro dalsi procesy
		miliseconds = SDL_GetTicks() - g_last_time;
	}

	g_fps = 1.0f / (miliseconds / 1000.0f);
	g_last_time = SDL_GetTicks();
}


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
