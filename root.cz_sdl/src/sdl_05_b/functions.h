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

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>// Nezapomenout prilinkovat knihovnu


// Pokusi se nahrat obrazek a zkonvertovat ho na stejny format jako ma okno
SDL_Surface *LoadImage(const char *filename, bool alpha = false);

// Vypocita FPS, nutno volat pri kazdem pruchodu hlavni smyckou
void CalculateFPS();
float GetFPS();
