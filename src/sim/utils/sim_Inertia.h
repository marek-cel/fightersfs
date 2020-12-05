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
#ifndef SIM_INERTIA_H
#define SIM_INERTIA_H

////////////////////////////////////////////////////////////////////////////////

#include <math.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Inertia template class. */
template < class TYPE >
class Inertia
{
public:

    /** @brief Static inertia function. */
    static TYPE update( TYPE timeStep, TYPE currentValue,
                        TYPE newValue, TYPE timeConstant )
    {
        return currentValue + ( 1.0 - exp( -timeStep / timeConstant ) ) * ( newValue - currentValue );
    }

    /** @brief Constructor. */
    Inertia() :
        _timeConstant( 1.0 ),
        _currentValue( 0.0 )
    {}

    /** @brief Constructor. */
    Inertia( TYPE timeConstant, TYPE currentValue = 0.0 ) :
        _timeConstant( timeConstant ),
        _currentValue( currentValue )
    {}

    /** @brief Updates value due to time step and time constant. */
    void update( TYPE timeStep, TYPE newValue )
    {
        _currentValue = update( timeStep, _currentValue, newValue, _timeConstant );
    }

    /** @brief Returns inertia output. */
    inline TYPE getValue() const { return _currentValue; }

    /** @brief Sets inertia output. */
    inline void setValue( TYPE newValue ) { _currentValue = newValue; }

private:

    TYPE _timeConstant;     ///< time constant
    TYPE _currentValue;     ///< current value

};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_INERTIA_H
