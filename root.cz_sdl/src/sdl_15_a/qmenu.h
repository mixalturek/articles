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

#ifndef LIBQUARK_QMENU_H
#define LIBQUARK_QMENU_H

#include <vector>
#include <stdexcept>
#include <SDL.h>

using namespace std;

namespace libQuark
{

class QMenu
{
public:
	QMenu();
	virtual ~QMenu();

	void Init(int num_items = 20);// Assumed size
	void Destroy();
//	virtual void Draw() = 0;

	void AddItem(int usr_event_code, const string& title);
	void SelectFirst() { m_selected = 0; }
	void SelectLast() { m_selected = m_event_codes.size()-1; }
	void SelectNext();
	void SelectPrevious();
	void Choose() const;

	int GetSel() const { return m_selected; }
	int GetSelCode() const { return m_event_codes[m_selected]; }
	const string& GetSelString() const
			{ return m_event_strings[m_selected];}
	unsigned int GetNumItems() const { return m_event_codes.size(); }

protected:
	vector<int> m_event_codes;
	vector<string> m_event_strings;
	int m_selected;
};

}

#endif
