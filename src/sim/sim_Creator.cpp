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

#include <sim/sim_Creator.h>

#include <sim/sim_ListUnits.h>
#include <sim/sim_Log.h>

#include <sim/entities/sim_Aircraft.h>
#include <sim/entities/sim_Balloon.h>
#include <sim/entities/sim_BomberDive.h>
#include <sim/entities/sim_BomberLevel.h>
#include <sim/entities/sim_BomberTorpedo.h>
#include <sim/entities/sim_Building.h>
#include <sim/entities/sim_Fighter.h>
#include <sim/entities/sim_Kamikaze.h>
#include <sim/entities/sim_Warship.h>

#include <sim/utils/sim_String.h>
#include <sim/utils/sim_XmlDoc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Aircraft* Creator::createAircraft( int index, int livery, Affiliation affiliation )
{
    if ( index >= 0 && index < (int)ListUnits::instance()->getCount() )
    {
        ListUnits::UnitData data = ListUnits::instance()->getData( index );

        if ( data.type == ListUnits::Aerial )
        {
            std::string type = data.fighter ? "fighter" : "aircraft";

            Unit *unit = createUnit( type, data.file, affiliation );

            Aircraft *aircraft = dynamic_cast< Aircraft* >( unit );

            if ( aircraft )
            {
                if ( livery >= 0 && livery < (int)data.liveries.size() )
                {
                    aircraft->setLivery( data.liveries[ livery ] );
                }
            }

            return aircraft;
        }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

Unit* Creator::createUnit( const std::string &type, const std::string &file,
                           Affiliation affiliation )
{
    Unit *unit = 0;

    if ( 0 == String::icompare( type, Aircraft::_tagName ) )
    {
        unit = new Aircraft( affiliation );
    }
    else if ( 0 == String::icompare( type, Balloon::_tagName ) )
    {
        unit = new Balloon( affiliation );
    }
    else if ( 0 == String::icompare( type, BomberLevel::_tagName ) )
    {
        unit = new BomberLevel( affiliation );
    }
    else if ( 0 == String::icompare( type, BomberDive::_tagName ) )
    {
        unit = new BomberDive( affiliation );
    }
    else if ( 0 == String::icompare( type, BomberTorpedo::_tagName ) )
    {
        unit = new BomberTorpedo( affiliation );
    }
    else if ( 0 == String::icompare( type, Building::_tagName ) )
    {
        unit = new Building( affiliation );
    }
    else if ( 0 == String::icompare( type, Fighter::_tagName ) )
    {
        unit = new Fighter( affiliation );
    }
    else if ( 0 == String::icompare( type, Kamikaze::_tagName ) )
    {
        unit = new Kamikaze( affiliation );
    }
    else if ( 0 == String::icompare( type, Warship::_tagName ) )
    {
        unit = new Warship( affiliation );
    }
    else
    {
        Log::e() << "Unknown unit type: \"" << type << "\"" << std::endl;
    }

    if ( unit )
    {
        std::string unitFile = Base::getPath( file );

        XmlDoc doc( unitFile );

        if ( doc.isOpen() )
        {
            if ( SIM_SUCCESS != unit->read( doc.getRootNode() ) )
            {
                DELPTR( unit );

                Log::e() << "Cannot read unit file: " << unitFile << std::endl;
            }
        }
        else
        {
            DELPTR( unit );

            Log::e() << "Cannot open unit file: " << unitFile << std::endl;
        }
    }

    return unit;
}
