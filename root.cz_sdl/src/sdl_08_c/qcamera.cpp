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

#include "qcamera.h"

namespace libQuark
{

QCamera::QCamera() :
		m_pos(0.0f, 0.0f, 0.0f),
		m_dir(0.0f, 0.0f, 1.0f),
		m_horz_angle(90.0f),
		m_vert_angle(0.0f),
		m_speed(10.0f),
		m_speed_rot(2.0f),
		m_relative_vert_pos(0.0f),
		m_max_vert_angle(80.0f)
{

}

QCamera::~QCamera()
{

}

void QCamera::SetHorizontalAngle(float horz_angle)
{
	m_horz_angle = horz_angle;

	m_dir.SetX(cos(DEGTORAD(m_horz_angle)));
	m_dir.SetZ(sin(DEGTORAD(m_horz_angle)));
}

void QCamera::SetVerticalAngle(float vert_angle)
{
	if(m_vert_angle > -m_max_vert_angle && m_vert_angle < m_max_vert_angle)
	{
		m_vert_angle = vert_angle;
		m_dir.SetY(-sin(DEGTORAD(m_vert_angle)));
	}
}

void QCamera::LookAt() const
{
	gluLookAt(	m_pos.GetX(),
			m_pos.GetY()+m_relative_vert_pos,
			m_pos.GetZ(),
			m_pos.GetX()+m_dir.GetX(),
			m_pos.GetY()+m_dir.GetY()+m_relative_vert_pos,
			m_pos.GetZ()+m_dir.GetZ(),
			0, 1, 0);
}

void QCamera::Rotate(int xrel, int yrel, float fps)
{
	m_horz_angle += xrel*m_speed_rot / fps;

	m_dir.SetX(cos(DEGTORAD(m_horz_angle)));
	m_dir.SetZ(sin(DEGTORAD(m_horz_angle)));

	if((m_vert_angle < m_max_vert_angle && yrel > 0)
			|| (m_vert_angle > -m_max_vert_angle && yrel < 0))
	{
		m_vert_angle += yrel*m_speed_rot / fps;

		m_dir.SetY(-sin(DEGTORAD(m_vert_angle)));
	}
}

void QCamera::ChangePosToQuadArea(int x_half, int z_half)
{
	if(m_pos.GetX() < -x_half)
		m_pos.SetX(-x_half);
	if(m_pos.GetX() > x_half)
		m_pos.SetX(x_half);

	if(m_pos.GetZ() < -z_half)
		m_pos.SetZ(-z_half);
	if(m_pos.GetZ() > z_half)
		m_pos.SetZ(z_half);
}

void QCamera::LoadSettings(const string& filename)
		throw(QFileNotFound)
{
	QIni ini(filename);// Exception if...

	m_pos = ini.Read(SEC, "position", m_pos);
	m_horz_angle = ini.Read(SEC, "horizontal_angle", m_horz_angle);
	m_vert_angle = ini.Read(SEC, "vertical_angle", m_vert_angle);
	m_speed = ini.Read(SEC, "speed", m_speed);
	m_speed_rot = ini.Read(SEC, "rot_speed", m_speed_rot);
	m_relative_vert_pos = ini.Read(SEC, "relative_vertical_pos",
			m_relative_vert_pos);
	m_max_vert_angle = ini.Read(SEC, "max_vertical_angle",m_max_vert_angle);

	if(m_vert_angle > m_max_vert_angle)
		m_vert_angle = m_max_vert_angle;
	else if(m_vert_angle < -m_max_vert_angle)
		m_vert_angle = -m_max_vert_angle;

	m_dir.SetX(cos(DEGTORAD(m_horz_angle)));
	m_dir.SetY(sin(DEGTORAD(m_vert_angle)));// Maybee -sin()
	m_dir.SetZ(sin(DEGTORAD(m_horz_angle)));
}

};
