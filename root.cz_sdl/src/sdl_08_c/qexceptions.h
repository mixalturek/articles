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

#ifndef LIBQUARK_QEXCETPIONS_H
#define LIBQUARK_QEXCETPIONS_H

#include <stdexcept>
#include "libquark.h"

using namespace std;

namespace libQuark
{

// Standard exceptions are used too
// std::runtime_error
// std::bad_alloc

// Thrown to indicate that the 'filename' file was not found
// or program is unable to open it
class QFileNotFound : public runtime_error
{
public:
	QFileNotFound(const string& filename) : runtime_error(filename)
	{
	}
};

// Thrown to indicate bad file format (loaded image etc.)
class QBadFileFormat : public runtime_error
{
public:
	QBadFileFormat(const string& filename) : runtime_error(filename)
	{
	}
};

};

#endif
