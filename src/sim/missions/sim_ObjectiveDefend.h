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
#ifndef SIM_OBJECTIVEDEFEND_H
#define SIM_OBJECTIVEDEFEND_H

////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include <sim/missions/sim_Objective.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Defend objective class.
 *
 * Objective is completed if there are at least minimum not destroyed
 * specified units.
 *
 * This objective should be used with another type of objective, because it is
 * assumed to be completed as long as there are minimum not destroyed units so
 * if there were no other objective mission stage would be immediately
 * completed.
 */
class ObjectiveDefend : public Objective
{
public:

    typedef std::vector< UInt32 > UnitsId;

    /** Reads objective from XML file. */
    static ObjectiveDefend* read( const XmlNode &node );

    /** Constructor. */
    ObjectiveDefend();

    /** Destructor. */
    virtual ~ObjectiveDefend();

    /** Updates objective state. */
    void update();

private:

    UnitsId _unitsId;       ///< IDs of units to be defended

    unsigned int _minimum;  ///< minimum number of units
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_OBJECTIVEDEFEND_H
