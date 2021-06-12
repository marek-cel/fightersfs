/****************************************************************************//*
 * Copyright (C) 2021 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
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
