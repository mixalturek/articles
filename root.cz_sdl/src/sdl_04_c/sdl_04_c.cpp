/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program vypisuje informace o grafickem hardware                       *
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
 * Pro vetsi prehlednost a rychlejsi upravy symbolicke konstanty
 */

#define SUBSYSTEMS SDL_INIT_VIDEO
#define WIN_FLAGS SDL_FULLSCREEN|SDL_HWSURFACE

// Pro snadnejsi vypisy flagy "zformatovane" do retezce
#define STR_FLAGS "SDL_FULLSCREEN|SDL_HWSURFACE"


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;


/*
 * Vypise jmeno grafickeho ovladace
 */

void PrintGraphicDriverName()
{
	char buffer[20];
	SDL_VideoDriverName(buffer, 20);

	printf("Name of the graphic driver: %s\n", buffer);
	printf("\n");
}


/*
 * Vypise informace o grafickem hardware
 */

/* Pozn.: SDL FAQ (Development)
Q: I have an accelerated video card, but SDL tells me that I have zero video
   memory and no acceleration!

A: Not all display targets can make use of hardware acceleration. In particular,
this is the case for the X11 target which always presents you with a software
framebuffer. Your video memory will always be reported to be zero if no
acceleration is available. Note that this has nothing to do with
3D acceleration, just 2D hardware acceleration support in the underlying
operating system video drivers.

If you want hardware acceleration on Linux, you can use the DGA driver
(fullscreen-only, under X11) or the framebuffer console driver (fullscreen-only,
under the console). See the Linux FAQ for more information on using these
drivers.

If you want hardware acceleration on Windows, make sure SDL is built with
DirectX support and run your program in fullscreen mode.

An excellent article by Bob Pendleton covering SDL hardware surfaces is
available at the O'Reilly Network

-----
Do konzole zapiste "$ export SDL_VIDEODRIVER=dga" a spustte program...
*/

void PrintGraphicDriverInfo()
{
	printf("Info about graphic hardware:\n");

	SDL_VideoInfo *vinfo;
	vinfo = (SDL_VideoInfo *)SDL_GetVideoInfo();

	printf("Is it possible to create hardware surfaces?          ");
	if(vinfo->hw_available) printf("YES\n"); else printf("NO\n");

	printf("Is there a window manager available?                 ");
	if(vinfo->wm_available) printf("YES\n"); else printf("NO\n");

	printf("Are hardware to hardware blits accelerated?          ");
	if(vinfo->blit_hw) printf("YES\n"); else printf("NO\n");

	printf("Are hardware to hardware colorkey blits accelerated? ");
	if(vinfo->blit_hw_CC) printf("YES\n"); else printf("NO\n");

	printf("Are hardware to hardware alpha blits accelerated?    ");
	if(vinfo->blit_hw_A) printf("YES\n"); else printf("NO\n");

	printf("Are software to hardware blits accelerated?          ");
	if(vinfo->blit_sw) printf("YES\n"); else printf("NO\n");

	printf("Are software to hardware colorkey blits accelerated? ");
	if(vinfo->blit_sw_CC) printf("YES\n"); else printf("NO\n");

	printf("Are software to hardware alpha blits accelerated?    ");
	if(vinfo->blit_sw_A) printf("YES\n"); else printf("NO\n");

	printf("Are color fills accelerated?                         ");
	if(vinfo->blit_fill) printf("YES\n"); else printf("NO\n");

	printf("Total amount of video memory: %u kB.\n", vinfo->video_mem);

	printf("\n");

	// Mozna by nebylo od veci vypisovat i OpenGL info, ale to uz se netyka
	// samotneho SDL

	// Nemuze byt volano zde, ale az po vytvoreni okna s podporou OpenGL
	// printf("OpenGL info:\n");
	// printf("Vendor: %s\n", glGetString(GL_VENDOR));
	// printf("Renderer: %s\n",glGetString(GL_RENDERER));
	// printf("Version: %s\n",glGetString(GL_VERSION));
	// printf("Extensions: %s\n",glGetString(GL_EXTENSIONS));
}


/*
 * Vypise dostupna rozliseni (kod prevzat ze SDL manualu)
 */

void PrintModes()
{
	// Ziska dostupne mody
	SDL_Rect **modes;
	modes = SDL_ListModes(NULL, WIN_FLAGS);

	printf("Available graphic modes for '%s' flags:\n", STR_FLAGS);

	// Jsou vubec nejake mody dostupne?
	if(modes == (SDL_Rect **)0)
	{
		printf("No modes available!\n");
	}

	// Jsou nejaka omezeni na rozliseni?
	if(modes == (SDL_Rect **)-1)
	{
		printf("All resolutions available.\n");
	}
	else
	{
		for(int i = 0; modes[i]; i++)
			printf("%d x %d\n", modes[i]->w, modes[i]->h);
	}

	printf("\n");
}


/*
 * Inicializacni funkce
 */

bool Init()
{
	// Inicializace SDL
	if(SDL_Init(SUBSYSTEMS) == -1)
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n",
				SDL_GetError());
		return false;
	}

	// Okno neni potreba...

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
 * Vstup do programu
 */

int main(int argc, char *argv[])
{
	// Inicializace
	if(!Init())
	{
		Destroy();
		return 1;
	}

	// Vypise info
	printf("\n");
	PrintGraphicDriverName();
	PrintGraphicDriverInfo();
	PrintModes();

	// Deinicializace a konec
	Destroy();
	return 0;
}
