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

#include "qapplication.h"

namespace libQuark
{

// Default values, can be changed when settings is loaded from file
QApplication::QApplication() :
	m_window(NULL),
	m_win_width(640),
	m_win_height(480),
	m_win_bpp(0),// Use system color depth
	m_win_flags(SDL_OPENGL | SDL_RESIZABLE),
			//| SDL_FULLSCREEN | SDL_NOFRAME

	m_near_plane(1.0),
	m_far_plane(100.0),

	m_last_time(0),
	m_miliseconds(1),
	m_fps(1.0f)// It is dividing by this
{

}

void QApplication::LoadSettings(const string& filename)
{
	QIni ini;

	try
	{
		ini.Init(filename);
	}
	catch(QFileNotFound& ex)
	{
		cerr << _("Unable to load settings from ") << ex.what()
				<< _(", using predefined values.") << endl;
		return;
	}

	m_win_width = ini.Read("application", "win_width", m_win_width);
	m_win_height = ini.Read("application", "win_height", m_win_height);
	m_win_bpp = ini.Read("application", "bpp", m_win_bpp);

	if(ini.Read("application", "win_resizable",
			(m_win_flags & SDL_RESIZABLE) ? true : false))
		m_win_flags |= SDL_RESIZABLE;
	else
		m_win_flags &= ~SDL_RESIZABLE;

	if(ini.Read("application", "win_noframe",
			(m_win_flags & SDL_NOFRAME) ? true : false))
		m_win_flags |= SDL_NOFRAME;
	else
		m_win_flags &= ~SDL_NOFRAME;

	if(ini.Read("application", "start_fullscreen",
			(m_win_flags & SDL_FULLSCREEN) ? true : false))
		m_win_flags |= SDL_FULLSCREEN;
	else
		m_win_flags &= ~SDL_FULLSCREEN;

	m_near_plane = ini.Read("application", "near_plane", m_near_plane);
	m_far_plane = ini.Read("application", "far_plane", m_far_plane);
}

QApplication::~QApplication()
{
	Destroy();
}

void QApplication::Destroy()
{
	SDL_Quit();
}

void QApplication::Init(const string& win_title)
//		throw(std::runtime_error)
{
	LoadSettings(PATH_CFG_FILE);
	CreateWindow(win_title);
}

void QApplication::CreateWindow(const string& win_title)
		throw(std::runtime_error)
{
	if(SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		throw runtime_error(string(_("Unable to initialize SDL: "))
				+ string(SDL_GetError()));
	}

	SDL_WM_SetCaption(win_title.c_str(), NULL);
	SDL_WM_SetIcon(SDL_LoadBMP(PATH_ICON), NULL);

	SetPixelFormat();

	m_window = SDL_SetVideoMode(m_win_width, m_win_height,
			m_win_bpp, m_win_flags);

	if(m_window == NULL)
	{
		throw runtime_error(string(_("Unable to create window: "))
				+ string(SDL_GetError()));
	}

	InitGL();
	ResizeWindow();
	m_last_time = SDL_GetTicks();// Init time
}

void QApplication::SetPixelFormat()
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);

	// My card does not support GL_ARB_multitexture, not tested
//	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
//	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
}

void QApplication::ResizeWindow()
{
	if(m_win_height == 0)// Dividing by zero
		m_win_height = 1;

	glViewport(0, 0, m_win_width, m_win_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLdouble)m_win_width/(GLdouble)m_win_height,
		m_near_plane, m_far_plane);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void QApplication::MainLoop()
{
	SDL_Event event;

	while(true)
	{
		while(SDL_PollEvent(&event))
		{
			if(!ProcessEvent(event))
				return;// Exit program
		}

		m_miliseconds = SDL_GetTicks() - m_last_time;

		if(m_miliseconds == 0)// Dividing by zero
			m_miliseconds = 1;

		m_fps = 1.0f / (m_miliseconds / 1000.0f);
		m_last_time = SDL_GetTicks();

		Update();
		Draw();
		glFlush();
		SDL_GL_SwapBuffers();
	}
}

bool QApplication::ProcessEvent(SDL_Event& event)
{
	switch(event.type)
	{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					return false;// Exit program
					break;

				case SDLK_F1:
					if(!ToggleFullscreen())
						return false;// Exit program
					break;

				default:// Other key
					break;
			}
			break;

		case SDL_VIDEORESIZE:
			m_win_width = event.resize.w;
			m_win_height = event.resize.h;

#ifdef QAPPLICATION_CALL_SETVIDEOMODE
			m_window = SDL_SetVideoMode(m_win_width,
				m_win_height, m_win_bpp, m_win_flags);

			if(m_window == NULL)
			{
				cerr << _("Unable to resize window: ")
						<< SDL_GetError() << endl;
				return false;// Exit program
			}
#endif
			ResizeWindow();
			break;

		case SDL_ACTIVEEVENT:// Minimalization of window
			if (event.active.state & SDL_APPACTIVE)
			{
				if (!event.active.gain)
				{
					// Warning: There can be some events yet
					SDL_WaitEvent(NULL);
				}
			}
			break;

		case SDL_QUIT:
			return false;// Exit program
			break;

		default:// Other events
			break;
	}

	return true;// OK
}

void QApplication::PushQuitEvent()
{
	SDL_Event event;
	event.type = SDL_QUIT;

	if(SDL_PushEvent(&event) == -1)
	{
		// The event cannot be sent
		cerr << _("QApplication: Unable to exit application standardly, calling exit()!")
				<< endl;

		this->Destroy();// Cleaning
		exit(1);// Hard kill :-(
	}
}

bool QApplication::ToggleFullscreen()
{
	if(m_win_flags & SDL_FULLSCREEN)// In fullscreen, to window
		m_win_flags &= ~SDL_FULLSCREEN;
	else// In window, to fullscreen
		m_win_flags |= SDL_FULLSCREEN;

	if(SDL_WM_ToggleFullScreen(m_window) == 0)// Can't toogle (MS Win)
	{
		// Complete recreation of window.

		cerr << _("Unable to toggle fullscreen, trying to recreate window.")
				<< endl;

		m_window = SDL_SetVideoMode(m_win_width, m_win_height,
				m_win_bpp, m_win_flags);

		if(m_window == NULL)
		{
			cerr << _("Unable to recreate window: ")
					<< SDL_GetError() << endl;
			return false;// Exit program
		}

		// Reinicialization of OpenGL (parameters, textures...).
		// Old OpenGL context isn't avaible yet
		try
		{
			InitGL();
		}
		catch(...)
		{
			cerr << _("QApplication: Unable to reinitialize OpenGL\n")
					<< endl;
			return false;// Exit program
		}

		ResizeWindow();// Set OpenGL perspective
	}

	return true;// OK
}

};
