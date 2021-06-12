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
#ifndef SIM_OBJECTIVEDISTANCE_H
#define SIM_OBJECTIVEDISTANCE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/missions/sim_Objective.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Distance objective class.
 *
 * Objective is completed if ownship distance to the specified unit
 * is less than given limit.
 *
 * This objective should be used with another type of objective, because it is
 * assumed to be completed as long as ownship is closer to the specified unit
 * than given distance so if there were no other objective mission stage would
 * be immediately completed.
 */
class ObjectiveDistance : public Objective
{
public:

    /** Reads objective from XML file. */
    static ObjectiveDistance* read( const XmlNode &node );

    /** Constructor. */
    ObjectiveDistance();

    /** Destructor. */
    virtual ~ObjectiveDistance();

    /** Updates objective state. */
    void update();

private:

    UInt32 _unitId;     ///< unit ID

    float _limit;       ///< [m] maximum distance
    float _limit_2;     ///< [m] maximum distance squared
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OBJECTIVEDISTANCE_H
