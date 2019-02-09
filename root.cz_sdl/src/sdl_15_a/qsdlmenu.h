/***************************************************************************
 *   libQuark - Archive of useful C++ classes                              *
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (at) seznam.cz, http://woq.nipax.cz/                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef LIBQUARK_QSIMPLEMENU_H
#define LIBQUARK_QSIMPLEMENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "qmenu.h"

using namespace std;

extern SDL_Surface *g_screen;

namespace libQuark
{

// With drawing menu using SDL_ttf
class QSDLMenu : public QMenu
{
public:
	QSDLMenu();
	virtual ~QSDLMenu();

	void Init(TTF_Font* font, int num_items = 20);
	void Draw(int x, int y, int num_items = 5);// Top left

private:
	TTF_Font* m_font;
};

}

#endif
