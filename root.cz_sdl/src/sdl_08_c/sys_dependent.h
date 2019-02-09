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

#ifndef LIBQUARK_SYS_DEPENDENT_H
#define LIBQUARK_SYS_DEPENDENT_H

namespace libQuark
{

/*
Defined - function SDL_SetVideoMode() is caled after resizing window
Define in Linux - actualization of "window viewport"
Don't define in MS Windows - loss of OpenGL context (textures disapear etc.)
*/
#define QAPPLICATION_CALL_SETVIDEOMODE

/*
Pointers to OpenGL Externsion functions are sometimes defined. Use this define
when compiler writes something as
error: `glActiveTextureARB' undeclared (first use this function)
*/
#define DEF_GLEXT_FUNCTIONS_POINTERS

};

#endif
