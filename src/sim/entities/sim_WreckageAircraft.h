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
#ifndef SIM_WRECKAGEAIRCRAFT_H
#define SIM_WRECKAGEAIRCRAFT_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/cgi/sim_Effects.h>
#include <sim/entities/sim_Wreckage.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Aircraft wreckage effect class. */
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

    const bool _ownship;

    osg::ref_ptr<osg::Node> _model;

    osg::ref_ptr<Effects::Smoke> _smokeTrail;   ///< smoke trail

    osg::ref_ptr<osg::Switch> _switchFire;      ///<

    std::string _livery;        ///< livery path
    std::string _modelFile;     ///< model file path

    float _rollRate;            ///< [rad/s] roll rate

    float _altitude_agl;        ///< [m] altitude above ground level
    float _elevation;           ///< [m] terrain elevation

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
