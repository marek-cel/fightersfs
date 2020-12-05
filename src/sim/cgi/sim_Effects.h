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
#ifndef SIM_EFFECTS_H
#define SIM_EFFECTS_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Group>

#include <osgParticle/SmokeEffect>

#include <sim/sim_Base.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Effects class. */
class Effects : public Base
{
public:

    typedef osgParticle::SmokeEffect Smoke;

    /** @brief Creates explosion effect. */
    static osg::Group* createExplosion( float scale );

    /** @brief Creates flames effect. */
    static osg::Group* createFlames( const char *texture );

    /** @brief Creates smoke effect. */
    static osg::Group* createSmoke( float lifeTime = 1.5f,
                                    float size0 = 1.0f, float size1 = 0.2f,
                                    float spread = 1.0f,
                                    float intensity = 1.0f,
                                    float speed = 1.0f );

    /** @brief Creates smoke trail effect. */
    static Smoke* createSmokeTrail();

};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_EFFECTS_H
