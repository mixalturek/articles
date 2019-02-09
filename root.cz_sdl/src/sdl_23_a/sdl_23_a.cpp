/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program zobrazuje spoustu kosticek, ktere je mozno kliknutim mysi     *
 *   chytit a nasledne s nimi pohybovat. Jsou implementovany i kolize      *
 *   a vlastni barevny kurzor, ktery se po kliknuti na nìjakou kostku      *
 *   zmeni na jiny.                                                        *
 *                                                                         *
 *   Program je modifikací ukazkoveho prikladu ze 13. dilu, obrazky se     *
 *   ted nacitaji ze ZIP archivu, jinak zadna zmena. Aby sel program       *
 *   zkompilovat, musi byt v systemu nainstalovana knihovna zziplib        *
 *   (http://zziplib.sourceforge.net/)                                     *
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

// btw, kurzory pochazeji z
// http://kde-look.org/content/show.php?content=20183

#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <zziplib.h>// Prilinkovat -lzzip
#include "SDL_rwops_zzip.h"// http://zziplib.sourceforge.net/zzip-sdl-rwops.html


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0
#define WIN_TITLE "Obrazky ze ZIP archivu"

// Sirka okraje okna (zarovnani kostek), pocet objektu
#define WIN_BORDER 3
#define NUM_OBJ 75


/*
 * Funkcni prototypy
 */

bool Init();				// Inicializace
void Destroy();				// Deinicializace
void Draw();				// Vykresleni
bool ProcessEvent();			// Osetruje udalosti
int  main(int argc, char *argv[]);	// Vstup do programu

bool PtInRect(int xpos, int ypos, int x, int y);// Pomocna
bool ToggleFullscreen();		// Zmena okno/fullscreen

// Pomoci knihovny zziplib nahraje obrazek ze ZIP archivu
SDL_Surface *LoadImageFromZIP(const char *filename, bool alpha = false);


/*
 * Globalni promenne
 */

Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF;
SDL_Surface *g_screen = NULL;		// Okno
SDL_Surface *g_bg = NULL;		// Pozadi
SDL_Surface *g_obj = NULL;		// Kostka
SDL_Surface *g_cur = NULL;		// Kurzor
SDL_Surface *g_cur_press = NULL;	// Stisknuty kurzor

int g_act = -1;				// Ktera kostka je drzena mysi
int g_xpos[NUM_OBJ], g_ypos[NUM_OBJ];	// Pozice kostek


/*
 * Funkce se pokusi nahrat obrazek ze souboru ulozeneho v ZIP archivu
 * a zkonvertovat ho na stejny pixel format, jako ma okno (framebuffer)
 */

SDL_Surface *LoadImageFromZIP(const char *filename, bool alpha)
{
	SDL_Surface *tmp;// Pomocny
	SDL_Surface *ret;// Bude vracen
	SDL_RWops *rw;// SDL_RWops pro nahravani

	// Vytvori SDL_RWops ze souboru v ZIP archivu
	if((rw = SDL_RWFromZZIP(filename, "r")) == NULL)
	{
		fprintf(stderr, "Unable to load file from ZIP archive.\n");
		return NULL;
	}

	// Nacte obrazek ze SDL_RWops, 1 - o uvolnovani se stara SDL
	if((tmp = IMG_Load_RW(rw, 1)) == NULL)// Prilinkovat SDL_image
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		return NULL;
	}

	// Konverze do formatu okna (zrychli vykreslovani)
	if((ret = (alpha) ? SDL_DisplayFormatAlpha(tmp)
			: SDL_DisplayFormat(tmp)) == NULL)
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_FreeSurface(tmp);
		return NULL;
	}

	// Uz neni potreba
	SDL_FreeSurface(tmp);

	return ret;
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

	// Loading obrazku
	// Cesta vyjadruje archiv data.zip a v nem data/bg.jpg (viz zziplib)
	if(((g_bg = LoadImageFromZIP("data/data/bg.jpg", false)) == NULL)
		|| ((g_obj = LoadImageFromZIP("data/data/obj.png",
				true)) == NULL)
		|| ((g_cur = LoadImageFromZIP("data/data/cur.png",
				true)) == NULL)
		|| ((g_cur_press = LoadImageFromZIP("data/data/cur_press.png",
				true)) == NULL))
	{
		return false;
	}

	// Pocatecni pozice objektu
	for(int i = 0, x = WIN_BORDER, y = g_obj->h+WIN_BORDER;
			i < NUM_OBJ; i++, x += g_obj->w+1)
	{
		if(x + g_obj->w > g_screen->w)
		{
			x = WIN_BORDER;
			y += g_obj->h+1;
		}

		g_xpos[i] = x;
		g_ypos[i] = g_screen->h - y;
	}

	// Skryje standardni kurzor (mame vlastni)
	SDL_ShowCursor(0);

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

	if(g_cur != NULL)
		SDL_FreeSurface(g_cur);

	if(g_cur_press != NULL)
		SDL_FreeSurface(g_cur_press);

	SDL_Quit();
}


/*
 * Vykresleni sceny
 */

void Draw()
{
	int x, y;		// Pozice kurzoru
	SDL_Rect rect;		// Pomocny obdelnik

	// Pozadi
	SDL_BlitSurface(g_bg, NULL, g_screen, NULL);

	// Kostky
	for(int i = 0; i < NUM_OBJ; i++)
	{
		rect.x = g_xpos[i];
		rect.y = g_ypos[i];

		SDL_BlitSurface(g_obj, NULL, g_screen, &rect);
	}

	// Kurzor, mel by se vzdy kreslit jako posledni

	if((SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
			&& g_act != -1)
	{
		rect.x = x - 14;// Pozice + zarovnani na aktivni bod
		rect.y = y - 14;
		SDL_BlitSurface(g_cur_press, NULL, g_screen, &rect);
	}
	else
	{
		rect.x = x - 14;
		rect.y = y - 14;
		SDL_BlitSurface(g_cur, NULL, g_screen, &rect);
	}

	SDL_Flip(g_screen);
}


/*
 * Testuje, zda je bod xpos, ypos uvnitr obdelniku x, y, g_obj->w, g_obj->h
 */

bool PtInRect(int xpos, int ypos, int x, int y)
{
	return (xpos >= x && xpos <= x+g_obj->w &&
		ypos >= y && ypos <= y+g_obj->h);
}


/*
 * Osetreni udalosti
 */

bool ProcessEvent()
{
	SDL_Event event;
	int i;			// Pomocna pro cykly

	while(SDL_WaitEvent(&event))
	{
		switch(event.type)
		{
		// Stisk klavesy
		case SDL_KEYDOWN:
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

		// Stisk tlacitka mysi
		case SDL_MOUSEBUTTONDOWN:
			// Bylo to nad nekterou kostkou?
			for(i = 0; i < NUM_OBJ; i++)
			{
				if(PtInRect(event.button.x, event.button.y,
						g_xpos[i], g_ypos[i]))
				{
					g_act = i;
				}
			}
			break;

		// Uvolneni mysi, zrusi vyber
		case SDL_MOUSEBUTTONUP:
			g_act = -1;
			break;

		// Pohyb mysi
		case SDL_MOUSEMOTION:
			// Pokud neni kostka drzena, ignorovat
			if(g_act == -1)
				break;

			// Aktualizuje polohu drzene kostky o pohyb mysi
			g_xpos[g_act] += event.motion.xrel;
			g_ypos[g_act] += event.motion.yrel;

			// PtInRect mezi kostkami
			for(i = 0; i < NUM_OBJ; i++)
			{
				// Netestovat sam se sebou
				if(i == g_act)
					continue;

				// Test vsech ctyrech rohu
				if(PtInRect(g_xpos[g_act],
						g_ypos[g_act],
						g_xpos[i],
						g_ypos[i])
				|| PtInRect(g_xpos[g_act] + g_obj->w,
						g_ypos[g_act],
						g_xpos[i],
						g_ypos[i])
				|| PtInRect(g_xpos[g_act],
						g_ypos[g_act] + g_obj->h,
						g_xpos[i],
						g_ypos[i])
				|| PtInRect(g_xpos[g_act] + g_obj->w,
						g_ypos[g_act] + g_obj->h,
						g_xpos[i],
						g_ypos[i]))
				{
					// Pri narazu se pohyb vyrusi
					g_xpos[g_act] -= event.motion.xrel;
					g_ypos[g_act] -= event.motion.yrel;

					// TODO: chtelo by pridat SDL_WarpMouse(),
					// ale problemy se zacyklenim :-(
					// Pokud to nekdo vyresite, dejte vedet...
				}
			}

			// PtInRect s okraji okna
			if(g_xpos[g_act] < WIN_BORDER)
				g_xpos[g_act] = WIN_BORDER;
			else if(g_xpos[g_act] > g_screen->w - g_obj->w - WIN_BORDER)
				g_xpos[g_act] = g_screen->w - g_obj->w - WIN_BORDER;
			if(g_ypos[g_act] < WIN_BORDER)
				g_ypos[g_act] = WIN_BORDER;
			else if(g_ypos[g_act] > g_screen->h - g_obj->h - WIN_BORDER)
				g_ypos[g_act] = g_screen->h - g_obj->h - WIN_BORDER;

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
