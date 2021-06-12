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
#ifndef SIM_SKYDOME_H
#define SIM_SKYDOME_H

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_TEST
#   include <osg/Switch>
#endif

#include <osg/PositionAttitudeTransform>
#include <osg/Texture2D>

#include <sim/cgi/sim_Module.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Sky dome module class. */
class SkyDome : public Module
{
public:

    static std::string _skyDomeFile;    ///< sky dome file
    static float _sunCoef;

    /** @brief Constructor. */
    SkyDome( Module *parent = NULLPTR );

    /** @brief Destructor. */
    virtual ~SkyDome();

    /** @brief Updates sky dome. */
    void update();

private:

#   ifdef SIM_TEST
    osg::ref_ptr<osg::Switch> _switch;
#   endif

    osg::ref_ptr<osg::PositionAttitudeTransform> _patSky;   ///< sky PAT
    osg::ref_ptr<osg::PositionAttitudeTransform> _patSun;   ///< sun PAT

    void createSky();
    void createSun();

    void createDome( osg::Geode *dome, osg::Texture2D *texture,
                     float radius, bool blend = false );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_SKYDOME_H
