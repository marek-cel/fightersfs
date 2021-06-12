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
