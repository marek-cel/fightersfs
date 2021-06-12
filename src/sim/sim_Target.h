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
#ifndef SIM_TARGET_H
#define SIM_TARGET_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Entities.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Target accquisition class template. */
template < class TYPE >
class Target
{
public:

    /**
     * @brief Constructor.
     * @param parent parent entity
     * @param target affiliation
     * @param rangeForward [m] maximum distance when looking for most in front target
     * @param rangeNearest [m] maximum distance when looking for nearest target
     */
    Target( Entity *parent, Affiliation affiliation,
            float rangeForward = 5000.0f,
            float rangeNearest = 5000.0f ) :
        _rangeForward ( rangeForward ),
        _rangeNearest ( rangeNearest ),

        _parent ( parent ),
        _target ( 0 ),
        _affiliation ( affiliation ),
        _id ( 0 ),
        _valid ( false )
    {}

    /** @brief Destructor. */
    virtual ~Target() {}

    /**
     * @brief Finds target most in front of parent.
     * @param max_a [rad] maximum angle from parent front
     */
    inline void findForward( float max_a = M_PI )
    {
        findForward( 0, max_a );
    }

    /**
     * @brief Finds target most in front of parent.
     * @param excluded unit excluded from search
     * @param max_a [rad] maximum angle from parent front
     */
    void findForward( TYPE* excluded, float max_a = M_PI )
    {
        _target = 0;
        _valid = false;

        Group::List *entities = Entities::instance()->getEntities();
        Group::List::iterator it = entities->begin();

        UInt32 excludedId = ( excluded != 0 ) ? excluded->getId() : 0;

        osg::Vec3f v_new;
        osg::Vec3f v_old;

        osg::Vec3f n_new;
        osg::Vec3f n_old;

        float a_new = 0.0f;
        float a_old = std::numeric_limits< float >::max();

        float limit2 = _rangeForward * _rangeForward;

        const Vec3 pos_abs = _parent->getAbsPos();
        const Quat att_abs = _parent->getAbsAtt();

        while ( it != entities->end() )
        {
            TYPE *target = dynamic_cast< TYPE* >( *it );

            if ( target )
            {
                if ( target->getAffiliation() == _affiliation && target->isActive() )
                {
                    if ( excluded == 0 || excludedId != target->getId() )
                    {
                        v_new = target->getPos() - pos_abs;

                        if ( v_new.length2() < limit2 )
                        {
                            n_new =  att_abs.inverse() * v_new;
                            n_new *= 1.0f / n_new.length();

                            float r = sqrt( n_new.y()*n_new.y() + n_new.z()*n_new.z() );

                            a_new = fabs( atan2( r, -n_new.x() ) );

                            if ( a_new < max_a )
                            {
                                if ( _valid )
                                {
                                    if ( a_new < a_old )
                                    {
                                        _target = target;
                                        _id = _target->getId();

                                        v_old = v_new;
                                        n_old = n_new;
                                        a_old = a_new;
                                    }
                                }
                                else
                                {
                                    _target = target;
                                    _id = _target->getId();

                                    v_old = v_new;
                                    n_old = n_new;
                                    a_old = a_new;

                                    _valid = true;
                                }
                            }
                        }
                    }
                }
            }

            ++it;
        }
    }

    /**
     * @brief Finds target nearest to parent.
     * @param max_a [rad] maximum angle from parent front
     */
    inline void findNearest( float max_a = M_PI )
    {
        findNearest( 0, max_a );
    }

    /**
     * @brief Finds target nearest to parent.
     * @param excluded unit excluded from search
     * @param max_a [rad] maximum angle from parent front
     */
    void findNearest( TYPE* excluded, float max_a = M_PI )
    {
        _target = 0;
        _valid = false;

        Group::List *entities = Entities::instance()->getEntities();
        Group::List::iterator it = entities->begin();

        UInt32 excludedId = ( excluded != 0 ) ? excluded->getId() : 0;

        osg::Vec3f v_new;
        osg::Vec3f n_new;

        float r_new = 0.0f;

        float d2_new = 0.0f;
        float d2_old = std::numeric_limits< float >::max();

        float limit2 = _rangeNearest * _rangeNearest;

        const Vec3 pos_abs = _parent->getAbsPos();
        const Quat att_abs = _parent->getAbsAtt();

        while ( it != entities->end() )
        {
            TYPE *target = dynamic_cast< TYPE* >( *it );

            if ( target )
            {
                if ( target->getAffiliation() == _affiliation && target->isActive() )
                {
                    if ( excluded == 0 || excludedId != target->getId() )
                    {
                        v_new = target->getPos() - pos_abs;

                        d2_new = v_new.length2();

                        if ( d2_new < limit2 )
                        {
                            n_new =  att_abs.inverse() * v_new;
                            n_new *= 1.0f / n_new.length();

                            r_new = sqrt( n_new.y()*n_new.y() + n_new.z()*n_new.z() );

                            float a = atan2( r_new, -n_new.x() );

                            if ( fabs( a ) < max_a )
                            {
                                if ( _valid )
                                {
                                    if ( d2_new < d2_old )
                                    {
                                        _target = target;
                                        _id = _target->getId();

                                        d2_old = d2_new;
                                    }
                                }
                                else
                                {
                                    _target = target;
                                    _id = _target->getId();

                                    d2_old = d2_new;

                                    _valid = true;
                                }
                            }
                        }
                    }
                }
            }

            ++it;
        }
    }

    /** @brief Updates target. */
    void update()
    {
        if ( _valid )
        {
            _target = dynamic_cast< TYPE* >( Entities::instance()->getEntityById( _id ) );

            if ( _target == 0 )
            {
                _target = 0;
                _valid = false;
            }
            else
            {
                if ( !_target->isActive() )
                {
                    _target = 0;
                    _valid = false;
                }
            }
        }
        else
        {
            _target = 0;
            _valid = false;
        }
    }

    /** @brief Returns true if target is valid. */
    inline bool isValid() const { return _valid; }

    /** @brief Returns target unit if exists, otherwise returns 0. */
    inline TYPE* getTarget() const { return _target; }

    /** @brief Sets target unit. */
    inline void setTarget( TYPE *target )
    {
        _target = target;

        if ( _target )
        {
            _id = _target->getId();
            _valid = true;
        }
        else
        {
            _valid = false;
        }
    }

private:

    const float _rangeForward;  ///< [m] maximum distance when looking for most in front target
    const float _rangeNearest;  ///< [m] maximum distance when looking for nearest target

    Entity *_parent;            ///< parent entity
    TYPE   *_target;            ///< target unit

    Affiliation _affiliation;   ///< target affiliation

    UInt32 _id;                 ///< target ID

    bool _valid;                ///< specifies if target is valid
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_TARGET_H
