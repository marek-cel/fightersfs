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
#ifndef SIM_BOMBERTORPEDO_H
#define SIM_BOMBERTORPEDO_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Bomber.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Torpedo bomber class. */
class BomberTorpedo : public Bomber
{
public:

    static const std::string _tagName;  ///< unit tag name

    static const float _dropAltMin;     ///< [m]
    static const float _dropAltMax;     ///< [m]
    static const float _dropDistMax;    ///< [m]
    static const float _tanMaxTht;      ///<

    /** Constructor. */
    BomberTorpedo( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~BomberTorpedo();

    /**
     * Reads bomber data.
     * @return SIM_SUCCESS on success or SIM_FAILURE on failure.
     */
    virtual int read( const XmlNode &node );

protected:

    bool _attack;           ///< specifies if aircraft is attacking

    virtual void limitTht( float &tht );

    virtual void updateTarget();
    virtual void updateWeapons();

};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_BOMBERTORPEDO_H
