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

/** Sky dome module class. */
class SkyDome : public Module
{
public:

    static std::string m_skyDomeFile;   ///< sky dome file
    static float m_sunCoef;

    /** Constructor. */
    SkyDome( Module *parent = 0 );

    /** Destructor. */
    virtual ~SkyDome();

    /** Updates sky dome. */
    void update();

private:

#   ifdef SIM_TEST
    osg::ref_ptr<osg::Switch> m_switch;
#   endif

    osg::ref_ptr<osg::PositionAttitudeTransform> m_patSky;  ///< sky PAT
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patSun;  ///< sun PAT

    void createSky();
    void createSun();

    void createDome( osg::Geode *dome, osg::Texture2D *texture,
                     float radius, bool blend = false );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_SKYDOME_H
