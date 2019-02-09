/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *
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
#include <SDL_thread.h>


int vlakno(void *arg)
{
	// N�jak� k�d
	for(int i = 0; i < 10000; i++)
	{
		fprintf(stderr, "vlakno()\n");
//		SDL_Delay(1);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	// Inicializace SDL

	SDL_Thread *thread;
	if((thread = SDL_CreateThread(vlakno, NULL)) == NULL)
	{
		fprintf(stderr, "Nelze vytvorit vlakno: %s",
				SDL_GetError());
		return 1;
	}

	// N�jak� k�d
	for(int i = 0; i < 10000; i++)
	{
		fprintf(stderr, "main()\n");
//		SDL_Delay(1);
	}

	// main() �ek� na ukon�en� vl�kna
	SDL_WaitThread(thread, NULL);
	return 0;
}
