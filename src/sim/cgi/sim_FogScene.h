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
#ifndef SIM_FOGSCENE_H
#define SIM_FOGSCENE_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Fog>
#include <osg/Group>

#include <sim/cgi/sim_Module.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * @brief Fog scene module class.
 *
 * This is parent module for all fogged sub-modules.
 */
class FogScene : public Module
{
public:

    static int _visibility;     ///< [m]

    /** @brief Constructor. */
    FogScene( Module *parent = NULLPTR );

    /** @brief Destructor. */
    virtual ~FogScene();

    /** @brief Updates fog scene module. */
    void update();

private:

    osg::ref_ptr<osg::Fog> _fog;    ///< fog
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_FOGSCENE_H
