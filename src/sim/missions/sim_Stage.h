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
#ifndef SIM_STAGE_H
#define SIM_STAGE_H

////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include <sim/sim_Base.h>
#include <sim/sim_Data.h>

#include <sim/missions/sim_Objective.h>

#include <sim/utils/sim_Text.h>
#include <sim/utils/sim_XmlNode.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Mission stage class.
 *
 * Mission stage is completed when all objectiven related to this stage are
 * completed.
 *
 * Units that are not ownship should be initialized in stages.
 */
class Stage : public Base
{
public:

    typedef std::vector< Objective* >  Objectives;
    typedef std::vector< std::string > InitUnits;

    /** Message struct. */
    struct Message
    {
        float delay;                ///< [s] message delay
        float duration;             ///< [s] message duration

        bool overlay;               ///< specifies if message is overlay

        Text text;                  ///< message text

        bool pointer_custom;        ///< specifies if custom pointer is visible
        bool pointer_target;        ///< specifies if target pointer is visible
        bool pointer_rpm_dec;       ///< specifies if decrease RPM pointer is visible
        bool pointer_rpm_inc;       ///< specifies if increase RPM pointer is visible
        bool pointer_trigger;       ///< specifies if trigger pointer is visible

        float pointer_x;            ///< [pt] custom pointer position x-coordinate
        float pointer_y;            ///< [pt] custom pointer position y-coordinate
        float pointer_phi;          ///< [rad] custom pointer rotation angle

        TutorialTip tip;            ///< tutorial tip
    };

    typedef std::vector< Message > Messages;

    /** Reads stage from XML file. */
    static Stage* read( const XmlNode &node, bool tutorial );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    static int readInitUnits( const XmlNode &node, InitUnits &initUnits );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    static int readMessage( const XmlNode &node, Message &message, bool tutorial );

    /** @return SIM_SUCCESS on success or SIM_FAILURE on failure. */
    static int readObjectives( const XmlNode &node, Objectives &objectives );

    /** Constructor. */
    Stage();

    /** Destructor. */
    virtual ~Stage();

    void addInitUnit( const std::string &unitName );

    /** Adds an objective to the stage. */
    void addObjective( Objective *objective );

    /** Initializes stage. */
    void init();

    /**
     * Updates stage.
     * @param timeStep [s] simulation time step
     */
    void update( double timeStep );

    /** */
    const Messages& getMessages() const { return _messages; }

    /** */
    const Objectives& getObjectives() const { return _objectives; }

    /** */
    inline Status getStatus() const { return _status; }

    inline float getStageTime() const { return _stageTime; }
    inline float getTimeLimit() const { return _timeLimit; }

private:

    InitUnits _initUnits;           ///< init units
    Messages _messages;             ///< stage messages
    Objectives _objectives;         ///< stage objectives
    Status _status;                 ///< stage status

    float _timeLimit;               ///< [s] stage time limit
    float _stageTime;               ///< [s] stage time (time since stage beginning)

    bool _inited;                   ///< specifies if stage has been inited
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_STAGE_H
