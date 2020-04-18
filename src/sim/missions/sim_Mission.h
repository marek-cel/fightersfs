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
 * <p>Mission is completed when all stages are completed.</p>
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
    bool isTutorial() const { return m_tutorial; }

private:

    Routes m_routes;                ///< all routes
    Stages m_stages;                ///< mission stages

    Route *m_ownshipRoute;          ///< ownship route

    UInt32 m_stageIndex;            ///< current stage index
    UInt32 m_messageIndex;          ///< current message index

    Status m_status;                ///< mission status

    bool m_tutorial;                ///< specified if mission is tutorial

    bool m_tutorialRoute;           ///< specifies if tutorial route has been inited

    bool m_ready;                   ///< specifies if mission is ready

    float m_realTime;               ///< [s] mission real time
    float m_timeEnd;                ///< [s] time since mission end
    float m_timeLeft;               ///< [s] time left to finish stage

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
