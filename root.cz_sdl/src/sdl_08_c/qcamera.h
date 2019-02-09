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

#ifndef LIBQUARK_QCAMERA_H
#define LIBQUARK_QCAMERA_H

#include <math.h>
#include <SDL_opengl.h>

#include "libquark.h"
#include "qvector.h"
#include "qini.h"

// Standard up vector
#define UP QVector<float>(0.0f, 1.0f, 0.0f)

// [Section] in INI file (LoadSettings())
#define SEC "camera"

namespace libQuark
{

class QCamera
{
public:
	QCamera();
	~QCamera();

	float GetXPos() const { return m_pos.GetX(); }
	float GetYPos() const { return m_pos.GetY(); }
	float GetZPos() const { return m_pos.GetZ(); }

	const QVector<float>& GetPos() const { return m_pos; }
	const QVector<float>& GetDir() const { return m_dir; }
	const QVector<float> GetLeft() const { return UP.Cross(m_dir); }
	const QVector<float> GetRight() const { return m_dir.Cross(UP); }

	float GetHorizontalAngle() const { return m_horz_angle; }
	float GetVerticalAngle() const { return m_vert_angle; }

	void SetPos(const QVector<float>& pos) { m_pos = pos; }
	void SetXPos(float x) { m_pos.SetX(x); }
	void SetYPos(float y) { m_pos.SetY(y); }
	void SetZPos(float z) { m_pos.SetZ(z); }

	void SetHorizontalAngle(float horz_angle);
	void SetVerticalAngle(float vert_angle);

	void LookAt() const;// gluLookAt() call

	void GoFront(float fps) { m_pos += (m_dir * m_speed / fps); }
	void GoBack(float fps) { m_pos -= (m_dir * m_speed / fps); }
	void GoLeft(float fps) { m_pos += (UP.Cross(m_dir) * m_speed / fps); }
	void GoRight(float fps) { m_pos += (m_dir.Cross(UP) * m_speed / fps); }
	void Rotate(int xrel, int yrel, float fps);

	// When the area (playground etc.) has borders
	void ChangePosToQuadArea(int x_half, int z_half);

	void LoadSettings(const string& filename) throw(QFileNotFound);

private:
	QVector<float> m_pos;
	QVector<float> m_dir;// Relative to m_pos
	float m_horz_angle;// Angles are in degrees
	float m_vert_angle;
	float m_speed;
	float m_speed_rot;
	float m_relative_vert_pos;
	float m_max_vert_angle;
};

};

#endif
