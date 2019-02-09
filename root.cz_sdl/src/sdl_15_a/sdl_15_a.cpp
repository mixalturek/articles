/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program demonstruje vytvoreni jednoducheho menu, ktere je zapouzdrene *
 *   do specialni tridy. Vyber polozek je aplikaci elegantne oznamovan     *
 *   prostrednictvim uzivatelskych udalosti.                               *
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
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "qsdlmenu.h"			// Trida menu

using namespace std;
using namespace libQuark;


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0

#define WIN_TITLE "Menu"

// Pocet obrazku pozadi
#define NUM_BG 5

// Kody zivatelskych udalosti
#define USR_EVT_CHANGE_BG 0
#define USR_EVT_HIDE_MENU 1
#define USR_EVT_EXIT_PROG 2


/*
 * Funkcni prototypy
 */

bool Init();				// Inicializace
void Destroy();				// Deinicializace
void Draw();				// Vykresleni
bool ProcessEvent();			// Osetruje udalosti
int  main(int argc, char *argv[]);	// Vstup do programu

bool ToggleFullscreen();		// Zmena okno/fullscreen
SDL_Surface *LoadImage(const char *filename, bool alpha = false);


/*
 * Globalni promenne
 */

Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF;
SDL_Surface *g_screen = NULL;	// Okno
SDL_Surface *g_bg[NUM_BG];	// Pozadi
int g_act_bg = 0;		// Index aktualniho pozadi

TTF_Font *g_font;		// Font
QSDLMenu g_menu;		// Objekt menu
bool g_menu_active = true;	// Menu aktivni?


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

	// Inicializace SDL_ttf
	if(TTF_Init() == -1)
	{
		printf("Unable to initialize SDL_ttf: %s\n",
				TTF_GetError());
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
	g_screen = SDL_SetVideoMode(WIN_WIDTH, WIN_HEIGHT,
			WIN_BPP, g_win_flags);
	if(g_screen == NULL)
	{
		fprintf(stderr, "Unable to set %dx%d video: %s\n",
				WIN_WIDTH, WIN_HEIGHT, SDL_GetError());
		return false;
	}

	// Titulek okna
	SDL_WM_SetCaption(WIN_TITLE, NULL);

	// Nahrani fontu
	g_font = TTF_OpenFont("DejaVuSansCondensed.ttf", 16);
	if(!g_font)
	{
		printf("Unable to open font: %s\n", TTF_GetError());
		return false;
	}
	TTF_SetFontStyle(g_font, TTF_STYLE_BOLD);// Tucne

	// Inicializace menu
	g_menu.Init(g_font, 10);

	// Nahrani obrazku
	char tmp[10];

	for(int i = 0; i < NUM_BG; i++)
	{
		sprintf(tmp, "bg%d.jpg", i);
		if((g_bg[i] = LoadImage(tmp, false)) == NULL)
			return false;

		// Pridani polozky do menu
		g_menu.AddItem(USR_EVT_CHANGE_BG, tmp);
	}

	g_menu.AddItem(USR_EVT_HIDE_MENU, "Skryt");
	g_menu.AddItem(USR_EVT_EXIT_PROG, "Konec");

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	g_menu.Destroy();

	// Smaze obrazky
	for(int i = 0; i < NUM_BG; i++)
	{
		if(g_bg[i] != NULL)
		{
			SDL_FreeSurface(g_bg[i]);
			g_bg[i] = NULL;
		}
	}

	// Smaze font a uklidi po SDL_ttf
	if(g_font != NULL)
	{
		TTF_CloseFont(g_font);
		g_font = NULL;
	}

	TTF_Quit();
	SDL_Quit();
}


/*
 * Vykresleni sceny
 */

void Draw()
{
	SDL_Rect rect;
	int x, y;

	// Pozadi dlazdicove pres cele okno
	for(x = 0; x < g_screen->w; x += g_bg[g_act_bg]->w)
	{
		for(y = 0; y < g_screen->h; y += g_bg[g_act_bg]->h)
		{
			rect.x = x;
			rect.y = y;
			SDL_BlitSurface(g_bg[g_act_bg], NULL,
					g_screen, &rect);
		}
	}

	// Menu
	if(g_menu_active)
		g_menu.Draw(125, 70);

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
		// Stisk klavesy
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				if(!g_menu_active)
				{
					g_menu_active = true;
				}
				else
				{
					// Esc, menu aktivni, posl. polozka
					if(g_menu.GetSel() ==
						(int)g_menu.GetNumItems()-1)
					{
						// Konec aplikace
						return false;
					}
					else
					{
						// Zvoli posledni polozku
						g_menu.SelectLast();
					}
				}
				break;

			case SDLK_UP:
				if(g_menu_active)
					g_menu.SelectPrevious();
				break;

			case SDLK_DOWN:
				if(g_menu_active)
					g_menu.SelectNext();
				break;

			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				if(g_menu_active)
					g_menu.Choose();
				break;

			case SDLK_F1:
				if(!ToggleFullscreen())
					return false;
				break;

			default:
				break;
			}
			break;

		// Uzivatelska udalost
		case SDL_USEREVENT:
			switch(event.user.code)
			{
			// Zmena pozadi
			case USR_EVT_CHANGE_BG:
				// Poradi je ve tride stejne jako v poli
				g_act_bg = g_menu.GetSel();
				break;

			// Skryti menu
			case USR_EVT_HIDE_MENU:
				g_menu_active = false;
				g_menu.SelectFirst();// Vyber pro priste
				break;

			// Konec programu
			case USR_EVT_EXIT_PROG:
				return false;
				break;

			default:
				break;
			}
			break;

		// Zmena velikosti okna
		case SDL_VIDEORESIZE:
			g_screen = SDL_SetVideoMode(event.resize.w,
					event.resize.h, WIN_BPP,
					g_win_flags);

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

		// Prekresleni
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
