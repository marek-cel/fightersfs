/****************************************************************************//*
 * Copyright (C) 2020 Marek M. Cel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 ******************************************************************************/

#include <sim/entities/sim_Bullet.h>

#include <sim/cgi/sim_Models.h>
#include <sim/entities/sim_Entities.h>
#include <sim/sim_Elevation.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const float Bullet::_vel_m = 600.0f;

////////////////////////////////////////////////////////////////////////////////

Bullet::Bullet( UInt16 dp, UInt32 shooterId, float life_span, Group *parent ) :
    Munition( dp, shooterId, life_span, parent )
{
    _vel.x() = -_vel_m;
}

////////////////////////////////////////////////////////////////////////////////

Bullet::~Bullet() {}

////////////////////////////////////////////////////////////////////////////////

void Bullet::load() {}
