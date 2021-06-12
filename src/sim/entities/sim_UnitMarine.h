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
#ifndef SIM_UNITMARINE_H
#define SIM_UNITMARINE_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_UnitSurface.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Marine unit base class. */
class UnitMarine : public UnitSurface
{
public:

#   ifdef SIM_DESKTOP
    static const char _frag[];      ///<
    static const char _vert[];      ///<

    static void createReflection( osg::Node *model, osg::Group *parent );
#   endif

    /** Constructor. */
    UnitMarine( Affiliation affiliation = Unknown );

    /** Destructor. */
    virtual ~UnitMarine();

    /** Destroys unit. */
    virtual void destroy();

    /** Loads unit (models, textures, etc.). */
    virtual void load();

    /** Sets unit hit points. */
    virtual void setHP( UInt16 hp );

protected:

    osg::ref_ptr<osg::Group> _smoke;    ///< damaged unit smoke group
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_UNITMARINE_H
