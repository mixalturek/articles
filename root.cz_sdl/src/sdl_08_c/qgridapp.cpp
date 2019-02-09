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

#include "qgridapp.h"

namespace libQuark
{

QGridApp::QGridApp() : QApplication(),
		m_cam()
{

}

QGridApp::~QGridApp()
{
	Destroy();
}

void QGridApp::Destroy()
{
	QApplication::Destroy();
}

void QGridApp::Init(const string & win_title)
		throw(std::runtime_error)
{
	QApplication::Init(win_title);
	SDL_ShowCursor(SDL_DISABLE);
}

void QGridApp::InitGL()
{
	glClearColor(0.0, 0.0, 0.0f, 0.0f);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(3.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void QGridApp::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	m_cam.LookAt();

	register float i, j;

	glBegin(GL_LINES);
		glColor4f(0.1f, 0.8f, 0.1f, 0.4f);
		for(i = -1.0f; i <= 1.0f; i += 2.0f)
		{
			for(j = -SIZE; j <= SIZE; j += DIST)
			{
				glVertex3f(j, i,-SIZE);
				glVertex3f(j, i, SIZE);
				glVertex3f(-SIZE, i, j);
				glVertex3f( SIZE, i, j);
			}
		}

		glColor4f(0.0f, 1.0f, 0.0f, 0.3f);
		for(i = -SIZE; i <= SIZE; i += GRID_CONSTANT)
		{
			for(j = -SIZE; j <= SIZE; j += GRID_CONSTANT)
			{
				glVertex3f(i,-1.0f, j);
				glVertex3f(i, 1.0f, j);
			}
		}
	glEnd();
}

void QGridApp::Update()
{
	Uint8* keys;
	keys = SDL_GetKeyState(NULL);

	// Move
	m_cam.GoFront(GetFPS());

	// Infinity repeating grid
	if(m_cam.GetXPos() < -GRID_CONSTANT)
		m_cam.SetXPos(m_cam.GetXPos() + GRID_CONSTANT);
	if(m_cam.GetXPos() > GRID_CONSTANT)
		m_cam.SetXPos(m_cam.GetXPos() - GRID_CONSTANT);
	if(m_cam.GetZPos() < -GRID_CONSTANT)
		m_cam.SetZPos(m_cam.GetZPos() + GRID_CONSTANT);
	if(m_cam.GetZPos() > GRID_CONSTANT)
		m_cam.SetZPos(m_cam.GetZPos() - GRID_CONSTANT);

	// Camera in constant height
	m_cam.SetYPos(0.0f);
}

bool QGridApp::ProcessEvent(SDL_Event& event)
{
	switch(event.type)
	{
	case SDL_MOUSEMOTION:
		// SDL_WarpMouse() generates SDL_MOUSEMOTION event :-(
		if(event.motion.x != GetWinWidth() >> 1
				|| event.motion.y != GetWinHeight() >> 1)
		{
			// First several messages MUST be ignored
			static int krize = 0;
			if(krize < 5)
			{
				krize++;
				break;
			}

			m_cam.Rotate(event.motion.xrel,
					event.motion.yrel, GetFPS());

			// Center mouse in window
			SDL_WarpMouse(GetWinWidth() >> 1, GetWinHeight() >> 1);
		}
		break;

	default:// Other events
		return QApplication::ProcessEvent(event);
		break;
	}

	return true;
}

};
