/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Hello, SDL!                                                           *
 *   Program vytvori SDL okno a pote ceka na stisk klavesy ESC.            *
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

int main(int argc, char *argv[])
{
	printf("Hello, SDL!\n");
	printf("Press ESC key to quit.\n");

	// Inicializuje se pouze SDL video
	if(SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n",
				SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// Vytvori okno 640x480 s implicitni barevnou hloubkou
	SDL_Surface *screen;
	screen = SDL_SetVideoMode(640, 480, 0, SDL_ANYFORMAT);

	if(screen == NULL)
	{
		fprintf(stderr, "Unable to set 640x480 video: %s\n",
				SDL_GetError());
		SDL_Quit();
		return 2;
	}

	// Titulek okna
	SDL_WM_SetCaption("Hello, SDL!", NULL);

	int done = 0;
	SDL_Event event;

	// Hlavni smycka programu
	while(done == 0)
	{
		// Smycka na zpracovani udalosti
		while(SDL_PollEvent(&event))
		{
			// Vetvi podle typu udalosti
			switch(event.type)
			{
				// Udalost klavesnice
				case SDL_KEYDOWN:
					// Vetvi podle klavesy
					switch(event.key.keysym.sym)
					{
						// Klavesa ESC */
						case SDLK_ESCAPE:
							done = 1;
							break;

						default:
							break;
					}
					break;

				// Zprava s pozadavkem na ukonceni
				case SDL_QUIT:
					done = 1;
					break;

				default:
					break;
			}
		}

		// TODO: Tady by se vykreslovalo
	}

	// Deinicializace SDL a konec
	SDL_Quit();
	return 0;
}
