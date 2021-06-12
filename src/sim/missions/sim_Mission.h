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
#ifndef SIM_MISSION_H
#define SIM_MISSION_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Base.h>
#include <sim/sim_Data.h>
#include <sim/sim_Route.h>

#include <sim/entities/sim_Unit.h>

#include <sim/missions/sim_Mission.h>
#include <sim/missions/sim_Stage.h>

#include <sim/utils/sim_XmlNode.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Mission class.
 *
 * Each mission is divided into stages. New stage starts only when the previous
 * one is accomplished. Each stage has a set of objectives. To accomplish a
 * stage all of its objectives have to be achieved. There are different types of
 * objectives, e.g. defending units, destroying targets, getting to location
 * (waypoint), or just timeout.
 */
class Mission : public Base
{
public:

    typedef std::map< std::string, Route* > Routes;
    typedef std::vector< Stage* > Stages;

    /**
     * Constructor.
     * @param missionFile mission file path (relative to the base path)
     */
    Mission();

    /** Destructor. */
    virtual ~Mission();

    /** Adds a stage to the mission. */
    void addStage( Stage *stage );

    /** Initializes mission. */
    void init();

    /** Initializes mission. */
    void init( const std::string &missionFile );

    /**
     * Updates mission.
     * @param timeStep [s] simulation time step
     */
    void update( double timeStep );

    /** Returns true if mission is a tutorial. */
    bool isTutorial() const { return _tutorial; }

private:

    Routes _routes;                 ///< all routes
    Stages _stages;                 ///< mission stages

    Route *_ownshipRoute;           ///< ownship route

    UInt32 _stageIndex;             ///< current stage index
    UInt32 _messageIndex;           ///< current message index

    Status _status;                 ///< mission status

    bool _tutorial;                 ///< specified if mission is tutorial

    bool _tutorialRoute;            ///< specifies if tutorial route has been inited

    bool _ready;                    ///< specifies if mission is ready

    float _realTime;                ///< [s] mission real time
    float _timeEnd;                 ///< [s] time since mission end
    float _timeLeft;                ///< [s] time left to finish stage

    /** Returns route of a given name, if not exists returns 0. */
    Route* getRouteByName( const std::string &name );

    /** */
    void initStageTutorial();

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    int readMission( const XmlNode &node );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    int readRoutes( const XmlNode &node );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    int readScenery( const XmlNode &node );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    int readStages( const XmlNode &node );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    int readUnits( const XmlNode &node );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    int readUnits( const XmlNode &node, Affiliation affiliation, bool ownship = false );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    int readUnit( const XmlNode &node, Affiliation affiliation, bool ownship = false );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    int readUnitLeaders( const XmlNode &node );

    /** Updates message. */
    void updateMessage();

    /**
     * Updates stages.
     * @param timeStep [s] simulation time step
     */
    void updateStages( double timeStep );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_MISSION_H
