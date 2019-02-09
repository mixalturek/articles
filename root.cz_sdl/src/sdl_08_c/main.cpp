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

#include "libquark.h"
#include "qgridapp.h"

using namespace std;
using namespace libQuark;

void my_terminate();
void my_unexpected();

void (*old_terminate)() = set_terminate(my_terminate);
void (*old_unexpected)() = set_unexpected(my_unexpected);

void my_terminate()
{
	cerr << _("Uncatched exception or throwing exception in some object's destructor!") << endl;
	old_terminate();
}

void my_unexpected()
{
	cerr << _("Unexpected exception was thrown!") << endl;
	old_terminate();
}

int main(int argc, char *argv[])
{
	QGridApp app;

	try
	{
		app.Init(_("Grid"));
	}
	catch(std::runtime_error& ex)
	{
		cerr << ex.what() << endl;
		app.Destroy();
		return EXIT_FAILURE;
	}
	catch(...)
	{
		cerr << _("Some unhandled exception occurs!") << endl;
		app.Destroy();
		return EXIT_FAILURE;
	}

	app.MainLoop();
	app.Destroy();

	return EXIT_SUCCESS;
}
