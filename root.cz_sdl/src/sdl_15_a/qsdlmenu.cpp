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

#include "qsdlmenu.h"

namespace libQuark
{

QSDLMenu::QSDLMenu() :
	m_font(NULL)
{

}

QSDLMenu::~QSDLMenu()
{

}

void QSDLMenu::Init(TTF_Font* font, int num_items)
{
	QMenu::Init(num_items);
	m_font = font;
}

void QSDLMenu::Draw(int x, int y, int num_items)
{
	if(m_event_codes.size() == 0 || m_font == NULL)
		return;

	int i;// For loops
	int min, max;

	min = m_selected - num_items/2;
	if(min < 0)
		min = 0;

	max = min + num_items;
	if(max > (int)m_event_codes.size())
		max = m_event_codes.size();

	if(max-min < num_items)
	{
		min = max - num_items;
		if(min < 0)
			min = 0;
	}


	// Color of active and inactive item and shade
	SDL_Color col = { 255, 255, 255, 0 };
	SDL_Color col_active = { 255, 0, 0, 0 };
	SDL_Color col_shade = { 100, 100, 100, 0 };
	SDL_Rect rect;
	SDL_Surface *text;

	rect.x = x;// Position of the first line
	rect.y = y;

	// Above active item
	for(i = min; i < m_selected; i++, rect.y += TTF_FontLineSkip(m_font))
	{
		// Shade of item (bottom left)
		text = TTF_RenderText_Blended(m_font,
				m_event_strings[i].c_str(), col_shade);
		SDL_BlitSurface(text, NULL, g_screen, &rect);
		SDL_FreeSurface(text);
		rect.x-=2; rect.y-=2;

		// Item
		text = TTF_RenderText_Blended(m_font,
				m_event_strings[i].c_str(),col);
		SDL_BlitSurface(text, NULL, g_screen, &rect);
		SDL_FreeSurface(text);
		rect.x+=2;
	}

	// Active item
	text = TTF_RenderText_Blended(m_font,
			m_event_strings[i].c_str(), col_shade);
	SDL_BlitSurface(text, NULL, g_screen, &rect);
	SDL_FreeSurface(text);
	rect.x-=2; rect.y-=2;

	text = TTF_RenderText_Blended(m_font, m_event_strings[i].c_str(),
			col_active);
	SDL_BlitSurface(text, NULL, g_screen, &rect);
	SDL_FreeSurface(text);
	rect.x+=2;

	// Below active item
	rect.y += TTF_FontLineSkip(m_font);
	for(i++; i < max; i++, rect.y += TTF_FontLineSkip(m_font))
	{
		text = TTF_RenderText_Blended(m_font,
				m_event_strings[i].c_str(), col_shade);
		SDL_BlitSurface(text, NULL, g_screen, &rect);
		SDL_FreeSurface(text);
		rect.x-=2; rect.y-=2;

		text = TTF_RenderText_Blended(m_font,
				m_event_strings[i].c_str(),col);
		SDL_BlitSurface(text, NULL, g_screen, &rect);
		SDL_FreeSurface(text);
		rect.x+=2;
	}
}

}
