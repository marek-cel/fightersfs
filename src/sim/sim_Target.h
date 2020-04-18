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
#ifndef SIM_TARGET_H
#define SIM_TARGET_H

////////////////////////////////////////////////////////////////////////////////

#include <sim/entities/sim_Entities.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Target accquisition class template. */
template < class TYPE >
class Target
{
public:

    /**
     * Constructor.
     * @param parent parent entity
     * @param target affiliation
     * @param rangeForward [m] maximum distance when looking for most in front target
     * @param rangeNearest [m] maximum distance when looking for nearest target
     */
    Target( Entity *parent, Affiliation affiliation,
            float rangeForward = 5000.0f,
            float rangeNearest = 5000.0f ) :
        m_rangeForward ( rangeForward ),
        m_rangeNearest ( rangeNearest ),

        m_parent ( parent ),
        m_target ( 0 ),
        m_affiliation ( affiliation ),
        m_id ( 0 ),
        m_valid ( false )
    {}

    /** Destructor. */
    virtual ~Target() {}

    /**
     * Finds target most in front of parent.
     * @param max_a [rad] maximum angle from parent front
     */
    inline void findForward( float max_a = M_PI )
    {
        findForward( 0, max_a );
    }

    /**
     * Finds target most in front of parent.
     * @param excluded unit excluded from search
     * @param max_a [rad] maximum angle from parent front
     */
    void findForward( TYPE* excluded, float max_a = M_PI )
    {
        m_target = 0;
        m_valid = false;

        Group::List *entities = Entities::instance()->getEntities();
        Group::List::iterator it = entities->begin();

        UInt32 excludedId = ( excluded != 0 ) ? excluded->getId() : 0;

        osg::Vec3f v_new;
        osg::Vec3f v_old;

        osg::Vec3f n_new;
        osg::Vec3f n_old;

        float a_new = 0.0f;
        float a_old = std::numeric_limits< float >::max();

        float limit2 = m_rangeForward * m_rangeForward;

        const Vec3 pos_abs = m_parent->getAbsPos();
        const Quat att_abs = m_parent->getAbsAtt();

        while ( it != entities->end() )
        {
            TYPE *target = dynamic_cast< TYPE* >( *it );

            if ( target )
            {
                if ( target->getAffiliation() == m_affiliation && target->isActive() )
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
                                if ( m_valid )
                                {
                                    if ( a_new < a_old )
                                    {
                                        m_target = target;
                                        m_id = m_target->getId();

                                        v_old = v_new;
                                        n_old = n_new;
                                        a_old = a_new;
                                    }
                                }
                                else
                                {
                                    m_target = target;
                                    m_id = m_target->getId();

                                    v_old = v_new;
                                    n_old = n_new;
                                    a_old = a_new;

                                    m_valid = true;
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
     * Finds target nearest to parent.
     * @param max_a [rad] maximum angle from parent front
     */
    inline void findNearest( float max_a = M_PI )
    {
        findNearest( 0, max_a );
    }

    /**
     * Finds target nearest to parent.
     * @param excluded unit excluded from search
     * @param max_a [rad] maximum angle from parent front
     */
    void findNearest( TYPE* excluded, float max_a = M_PI )
    {
        m_target = 0;
        m_valid = false;

        Group::List *entities = Entities::instance()->getEntities();
        Group::List::iterator it = entities->begin();

        UInt32 excludedId = ( excluded != 0 ) ? excluded->getId() : 0;

        osg::Vec3f v_new;
        osg::Vec3f n_new;

        float r_new = 0.0f;

        float d2_new = 0.0f;
        float d2_old = std::numeric_limits< float >::max();

        float limit2 = m_rangeNearest * m_rangeNearest;

        const Vec3 pos_abs = m_parent->getAbsPos();
        const Quat att_abs = m_parent->getAbsAtt();

        while ( it != entities->end() )
        {
            TYPE *target = dynamic_cast< TYPE* >( *it );

            if ( target )
            {
                if ( target->getAffiliation() == m_affiliation && target->isActive() )
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
                                if ( m_valid )
                                {
                                    if ( d2_new < d2_old )
                                    {
                                        m_target = target;
                                        m_id = m_target->getId();

                                        d2_old = d2_new;
                                    }
                                }
                                else
                                {
                                    m_target = target;
                                    m_id = m_target->getId();

                                    d2_old = d2_new;

                                    m_valid = true;
                                }
                            }
                        }
                    }
                }
            }

            ++it;
        }
    }

    /** Updates target. */
    void update()
    {
        if ( m_valid )
        {
            m_target = dynamic_cast< TYPE* >( Entities::instance()->getEntityById( m_id ) );

            if ( m_target == 0 )
            {
                m_target = 0;
                m_valid = false;
            }
            else
            {
                if ( !m_target->isActive() )
                {
                    m_target = 0;
                    m_valid = false;
                }
            }
        }
        else
        {
            m_target = 0;
            m_valid = false;
        }
    }

    /** Returns true if target is valid. */
    inline bool isValid() const { return m_valid; }

    /** Returns target unit if exists, otherwise returns 0. */
    inline TYPE* getTarget() const { return m_target; }

    /** Sets target unit. */
    inline void setTarget( TYPE *target )
    {
        m_target = target;

        if ( m_target )
        {
            m_id = m_target->getId();
            m_valid = true;
        }
        else
        {
            m_valid = false;
        }
    }

private:

    const float m_rangeForward; ///< [m] maximum distance when looking for most in front target
    const float m_rangeNearest; ///< [m] maximum distance when looking for nearest target

    Entity *m_parent;           ///< parent entity
    TYPE   *m_target;           ///< target unit

    Affiliation m_affiliation;  ///< target affiliation

    UInt32 m_id;                ///< target ID

    bool m_valid;               ///< specifies if target is valid
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_TARGET_H
