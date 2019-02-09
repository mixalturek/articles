/***************************************************************************
 *   libQuark - Archive of useful C++ classes                              *
 *   Copyright (C) 2004 by Michal Turek - Woq                              *
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

#ifndef LIBQUARK_QGRIDAPP_H
#define LIBQUARK_QGRIDAPP_H

#include "libquark.h"
#include "qapplication.h"
#include "qcamera.h"

#define SIZE 200.0f
#define DIST 2.0f
#define GRID_CONSTANT 6.0f

namespace libQuark
{

class QGridApp : public QApplication
{
public:
	QGridApp();
	virtual ~QGridApp();
	virtual void Init(const string& win_title) throw(std::runtime_error);
	virtual void Destroy();

protected:
	virtual void InitGL();
	virtual void Draw();
	virtual void Update();
	virtual bool ProcessEvent(SDL_Event& event);

private:
	QCamera m_cam;
};

};

#endif
