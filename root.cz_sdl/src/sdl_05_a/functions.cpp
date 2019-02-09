/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Uzitecne funkce, ktere se mohou pri praci hodit...                    *
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

#include "functions.h"


/*
 * Pomocne promenne
 */

float g_fps = 1.0f;			// Aktualni FPS
Uint32 g_last_time = 0;			// Cas minuleho prekresleni


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
 * Vypocet poctu snimku za sekundu, pohyby budou nezavisle na rychlosti pocitace
 */

void CalculateFPS()
{
	// SDL_GetTicks() vraci cas v sekundach od inicializace SDL
	Uint32 miliseconds = SDL_GetTicks() - g_last_time;

	if(miliseconds == 0)// Proti deleni nulou
		miliseconds = 1;

	g_fps = 1.0f / (miliseconds / 1000.0f);
	g_last_time = SDL_GetTicks();
}


/*
 * Vrati pocet snimku za sekundu
 */

float GetFPS()
{
	return g_fps;
}
