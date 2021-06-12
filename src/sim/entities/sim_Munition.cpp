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

#include <sim/entities/sim_Munition.h>

#include <sim/cgi/sim_Models.h>
#include <sim/entities/sim_Entities.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Munition::Munition( UInt16 dp, UInt32 shooterId, float life_span, Group *parent ) :
    Entity( parent, Active, life_span ),

    _dp ( dp ),
    _shooterId ( shooterId )
{}

////////////////////////////////////////////////////////////////////////////////

Munition::~Munition() {}

////////////////////////////////////////////////////////////////////////////////

void Munition::load()
{
    if ( _model.valid() )
    {
        _switch->removeChild( _model.get() );
    }

    _model = Models::get( _modelFile );

    if ( _model.valid() )
    {
        _switch->addChild( _model.get() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Munition::update( double timeStep )
{
    ///////////////////////////
    Entity::update( timeStep );
    ///////////////////////////

    if ( isActive() )
    {
        if ( isTopLevel() )
        {
            List::iterator it = Entities::instance()->getEntities()->begin();

            while ( it != Entities::instance()->getEntities()->end() )
            {
                Unit *target = dynamic_cast< Unit* >(*it);

                if ( target )
                {
                    if ( target->isActive() )
                    {
                        Vec3 v1 = _att * ( _vel * timeStep );
                        Vec3 v2 = target->getPos() - _pos;

                        float len1 = v1.length();

                        Vec3 v1_norm = v1 * (1.0/len1);

                        float proj2 = v2 * v1_norm;

                        if ( proj2 >= 0.0 && proj2 <= len1 )
                        {
                            float dist2 = ( v2 - v1_norm * proj2 ).length2();

                            if ( dist2 < target->getRadius2() && _shooterId != target->getId() )
                            {
                                hit( target );
                                setState( Inactive );
                                break;
                            }
                        }
                    }
                }

                ++it;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Munition::setModelFile( const std::string &modelFile )
{
    _modelFile = modelFile;
    load();
}

////////////////////////////////////////////////////////////////////////////////

void Munition::hit( Unit *target )
{
    target->hit( _dp, this );
    reportTargetHit( target );
}

////////////////////////////////////////////////////////////////////////////////

void Munition::reportTargetHit( Unit *target )
{
    Unit *shooter = dynamic_cast< Unit* >( Entities::instance()->getEntityById( _shooterId ) );

    if ( shooter )
    {
        shooter->reportTargetHit( target );
    }
}
