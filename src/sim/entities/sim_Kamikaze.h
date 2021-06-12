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
#ifndef SIM_KAMIKAZE_H
#define SIM_KAMIKAZE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/sim_Target.h>

#include <sim/entities/sim_Aircraft.h>
#include <sim/entities/sim_UnitMarine.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Kamikaze class. */
class Kamikaze : public Aircraft
{
public:

    static const std::string _tagName;  ///< unit tag name

    /** Constructor. */
    Kamikaze( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~Kamikaze();

    /** Overloaded due to extra demage from kamikaze when collide. */
    virtual void collide( Unit *unit );

    /** Updates entity. */
    virtual void update( double timeStep );

    /** Returns aircraft target unit pointer. */
    virtual UnitMarine* getTarget() const;

protected:

    Target< UnitMarine > *_target;      ///< target

    bool _engaged;                      ///< specifies if aircraft is engaged into kamikaze attack

    virtual void limitTht( float &tht );

    virtual void updateDestination();
    virtual void updateTarget();

};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_KAMIKAZE_H
