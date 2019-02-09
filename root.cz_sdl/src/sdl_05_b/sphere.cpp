/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   http://woq.nipax.cz/                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "sphere.h"

CSphere::CSphere(CSphere* left, CSphere* right, int x, int y, int width, int height)
	:
	m_left(left),
	m_right(right),
	m_x(x),
	m_y(y),
	m_width(width),
	m_height(height)
{

}

CSphere::~CSphere()
{

}

bool Click(int x, int y)
{
	return (x > m_x && x < m_x+m_width && y > m_y && m_y < m_y+m_height);
}