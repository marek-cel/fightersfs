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

#include <sim/missions/sim_Stage.h>

#include <sim/entities/sim_Entities.h>
#include <sim/missions/sim_Objective.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_Convert.h>
#include <sim/utils/sim_XmlUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Stage* Stage::read( const XmlNode &node, bool tutorial )
{
    Stage *stage = new Stage();

    if ( node.isValid() )
    {
        if ( 0 == String::icompare( node.getName(), "stage" ) )
        {
            // time limit
            XmlNode timeLimitNode = node.getFirstChildElement( "time_limit" );

            float timeLimit = 0.0f;

            if ( SIM_SUCCESS == XmlUtils::read( timeLimitNode, timeLimit ) )
            {
                if ( timeLimit > 0.0f )
                {
                    stage->m_timeLimit = timeLimit;
                }
            }

            // init units
            XmlNode initUnitsNode = node.getFirstChildElement( "init_units" );

            InitUnits initUnits;

            if ( SIM_SUCCESS == readInitUnits( initUnitsNode, initUnits ) )
            {
                stage->m_initUnits = initUnits;
            }

            // message            
            XmlNode messageNode = node.getFirstChildElement( "message" );

            while ( messageNode.isValid() )
            {
                Message message;

                if ( SIM_SUCCESS == readMessage( messageNode, message, tutorial ) )
                {
                    stage->m_messages.push_back( message );
                }

                messageNode = messageNode.getNextSiblingElement( "message" );
            }

            // objectives
            XmlNode objectivesNode = node.getFirstChildElement( "objectives" );

            Objectives objectives;

            if ( SIM_SUCCESS == readObjectives( objectivesNode, objectives ) )
            {
                stage->m_objectives = objectives;
            }

            return stage;
        }
    }

    DELPTR( stage );

    return stage;
}

////////////////////////////////////////////////////////////////////////////////

int Stage::readInitUnits( const XmlNode &node, InitUnits &initUnits )
{
    if ( node.isValid() )
    {
        XmlNode unitNode = node.getFirstChildElement( "unit" );

        while ( unitNode.isValid() )
        {
            std::string unitName = unitNode.getAttribute( "name" );

            if ( unitName.length() > 0 )
            {
                initUnits.push_back( unitName );
            }
            else
            {
                return SIM_FAILURE;
            }

            unitNode = unitNode.getNextSiblingElement( "unit" );
        }
    }

    return SIM_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

int Stage::readMessage( const XmlNode &node, Message &message, bool tutorial )
{
    int status = SIM_SUCCESS;

    if ( node.isValid() )
    {
        message.delay    = 0.0;
        message.duration = 0.0;

        message.overlay = false;

        message.pointer_custom  = false;
        message.pointer_target  = false;
        message.pointer_rpm_dec = false;
        message.pointer_rpm_inc = false;
        message.pointer_trigger = false;

        message.pointer_x   = 0.0;
        message.pointer_y   = 0.0;
        message.pointer_phi = 0.0;

        message.tip = NoTip;

        // overlay
        message.overlay = String::toInt( node.getAttribute( "overlay" ) ) == 1 ? true : false;

        // durations
        if ( status == SIM_SUCCESS )
        {
            float duration = 0.0f;

            XmlNode durationNode = node.getFirstChildElement( "duration" );

            status = XmlUtils::read( durationNode, duration );

            if ( status == SIM_SUCCESS && duration > 0.0f )
            {
                message.duration = duration;
            }
            else
            {
                status = SIM_FAILURE;
            }
        }

        // delay
        if ( status == SIM_SUCCESS )
        {
            float delay = 0.0f;

            XmlNode delayNode = node.getFirstChildElement( "delay" );

            if ( delayNode.isValid() )
            {
                status = XmlUtils::read( delayNode, delay );

                if ( status == SIM_SUCCESS && delay >= 0.0f )
                {
                    message.delay = delay;
                }
                else
                {
                    status = SIM_FAILURE;
                }
            }
        }

        // caption
        if ( status == SIM_SUCCESS )
        {
            Text text;

            XmlNode lineNode = node.getFirstChildElement( "caption" ).getFirstChildElement( "line" );

            while ( status == SIM_SUCCESS && lineNode.isValid() )
            {
                Text line;

                if ( SIM_SUCCESS == XmlUtils::read( lineNode, line ) )
                {
                    if ( text.get().length() > 0 )
                    {
                        text.append( "\n" );
                        text.append( line );
                    }
                    else
                    {
                        text = line;
                    }
                }
                else
                {
                    break;
                }

                lineNode = lineNode.getNextSiblingElement( "line" );
            }

            if ( status == SIM_SUCCESS && text.get().length() > 0 )
            {
                message.text = text;
            }
            else
            {
                status = SIM_FAILURE;
            }
        }

        // pointers
        if ( status == SIM_SUCCESS && tutorial )
        {
            message.pointer_custom  = String::toInt( node.getAttribute( "pointer_custom"  ) ) == 1 ? true : false;
            message.pointer_target  = String::toInt( node.getAttribute( "pointer_target"  ) ) == 1 ? true : false;
            message.pointer_rpm_dec = String::toInt( node.getAttribute( "pointer_rpm_dec" ) ) == 1 ? true : false;
            message.pointer_rpm_inc = String::toInt( node.getAttribute( "pointer_rpm_inc" ) ) == 1 ? true : false;
            message.pointer_trigger = String::toInt( node.getAttribute( "pointer_trigger" ) ) == 1 ? true : false;

            // custom pointer
            if ( message.pointer_custom )
            {
                XmlNode pointerNode = node.getFirstChildElement( "pointer_custom" );

                if ( pointerNode.isValid() )
                {
                    XmlNode xNode   = pointerNode.getFirstChildElement( "x" );
                    XmlNode yNode   = pointerNode.getFirstChildElement( "y" );
                    XmlNode phiNode = pointerNode.getFirstChildElement( "phi" );

                    if ( xNode.isValid() && yNode.isValid() && phiNode.isValid() )
                    {
                        std::string pointer_x;
                        std::string pointer_y;
                        std::string pointer_phi;

                        if ( status == SIM_SUCCESS ) status = XmlUtils::read(   xNode, pointer_x   );
                        if ( status == SIM_SUCCESS ) status = XmlUtils::read(   yNode, pointer_y   );
                        if ( status == SIM_SUCCESS ) status = XmlUtils::read( phiNode, pointer_phi );

                        if ( status == SIM_SUCCESS )
                        {
                            message.pointer_x = String::toFloat( pointer_x );
                            message.pointer_y = String::toFloat( pointer_y );
                            message.pointer_phi = Convert::deg2rad( String::toFloat( pointer_phi ) );
                        }
                    }
                    else
                    {
                        status = SIM_FAILURE;
                    }
                }
                else
                {
                    status = SIM_FAILURE;
                }
            }

            // tutorial tip
            int tip = String::toInt( node.getAttribute( "tutorial_tip" ) );

            switch ( tip )
            {
                case BankLeft:  message.tip = BankLeft;  break;
                case BankRight: message.tip = BankRight; break;
                case PitchUp:   message.tip = PitchUp;   break;
                case PitchDown: message.tip = PitchDown; break;
                default:        message.tip = NoTip;     break;
            }
        }
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////

int Stage::readObjectives( const XmlNode &node, Objectives &objectives )
{
    XmlNode objectiveNode = node.getFirstChildElement();

    while ( objectiveNode.isValid() )
    {
        Objective *objective = Objective::read( objectiveNode );

        if ( objective )
        {
            objectives.push_back( objective );
        }
        else
        {
            return SIM_FAILURE;
        }

        objectiveNode = objectiveNode.getNextSiblingElement();
    }

    return SIM_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

Stage::Stage() :
    m_status ( Pending ),

    m_timeLimit ( 0.0f ),
    m_stageTime ( 0.0f ),

    m_inited ( false )
{
    m_objectives.clear();
}

////////////////////////////////////////////////////////////////////////////////

Stage::~Stage()
{
    for ( Objectives::iterator it = m_objectives.begin(); it != m_objectives.end(); ++it )
    {
        DELPTR( *it );
    }

    m_objectives.clear();
}

////////////////////////////////////////////////////////////////////////////////

void Stage::addInitUnit( const std::string &unitName )
{
    m_initUnits.push_back( unitName );
}

////////////////////////////////////////////////////////////////////////////////

void Stage::addObjective( Objective *objective )
{
    m_objectives.push_back( objective );
}

////////////////////////////////////////////////////////////////////////////////

void Stage::init()
{
    if ( !m_inited )
    {
        for ( InitUnits::iterator it = m_initUnits.begin(); it != m_initUnits.end(); ++it )
        {
            Unit *unit = Entities::instance()->getUnitByName( *it );

            if ( unit )
            {
                if ( Standby == unit->getState() )
                {
                    unit->activate();
                }
            }
        }

        m_inited = true;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Stage::update( double timeStep )
{
    m_stageTime += (float)timeStep;

    if ( m_status == Pending )
    {
        if ( m_objectives.size() > 0 )
        {
            bool failure = false;
            bool success = true;

            Objectives::iterator it = m_objectives.begin();

            while( it != m_objectives.end() )
            {
                if ( (*it) )
                {
                    (*it)->update();

                    if ( !failure )
                    {
                        if ( (*it)->getStatus() == Failure ) failure = true;
                        if ( (*it)->getStatus() != Success ) success = false;
                    }
                }

                ++it;
            }

            if ( m_stageTime > m_timeLimit && m_timeLimit > 0.0f )
            {
                failure = true;
            }

            if ( success ) m_status = Success;
            if ( failure ) m_status = Failure;
        }
        else
        {
            m_status = Success;
        }
    }
}
