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
#ifndef SIM_WRECKAGEAIRCRAFT_H
#define SIM_WRECKAGEAIRCRAFT_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/cgi/sim_Effects.h>
#include <sim/entities/sim_Wreckage.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Aircraft wreckage effect class. */
class WreckageAircraft : public Wreckage
{
public:

    /** Constructor. */
    WreckageAircraft( const std::string &modelFile, const std::string &livery,
                      osgParticle::SmokeEffect *smokeTrail, bool ownship = false );

    /** Destructor. */
    virtual ~WreckageAircraft();

    /** */
    virtual void init( const Vec3 &pos,
                       const Quat &att,
                       const Vec3 &vel,
                       const Vec3 &omg );

    /** Loads aircraft (models, textures, etc.). */
    virtual void load();

    /** Updates effect. */
    virtual void update( double timeStep );

    /** */
    virtual void setState( State state );

private:

    const bool m_ownship;

    osg::ref_ptr<osg::Node> m_model;

    osg::ref_ptr<Effects::Smoke> m_smokeTrail;  ///< smoke trail

    osg::ref_ptr<osg::Switch> m_switchFire;     ///<

    std::string m_livery;       ///< livery path
    std::string m_modelFile;    ///< model file path

    float m_rollRate;           ///< [rad/s] roll rate

    float m_altitude_agl;       ///< [m] altitude above ground level
    float m_elevation;          ///< [m] terrain elevation

    /** */
    void updateElevation();

    /** */
    void updateVelocity();

    /** */
    void createFire();

    /** */
    osg::Node* getModel( const std::string &modelFile );

    /** */
    void loadLivery();

    /** */
    void loadModel();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_WRECKAGEAIRCRAFT_H
