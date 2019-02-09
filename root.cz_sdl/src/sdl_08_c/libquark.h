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

#ifndef LIBQUARK_LIBQUARK_H
#define LIBQUARK_LIBQUARK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "paths.h"// Disk paths
#include "sys_dependent.h"// System dependent settings
#include "qexceptions.h"// Exception classes

namespace libQuark
{

// Prepare for GetText
#define _(string) (string)

// Some math
#define PI 3.1415926535897932384626433832795
#define DEGTORAD(deg) ((PI*(deg))/180.0)
#define RADTODEG(rad) ((180.0*(rad))/PI)

}

#endif
