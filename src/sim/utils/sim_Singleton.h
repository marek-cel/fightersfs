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
#ifndef SIM_SINGLETON_H
#define SIM_SINGLETON_H

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Singleton template class. */
template < class TYPE >
class Singleton
{
public:

    /** Instace accessor. */
    inline static TYPE* instance()
    {
        if ( !m_instance )
        {
            m_instance = new TYPE();
        }

        return m_instance;
    }

public:

    /** Destructor. */
    virtual ~Singleton() {}

protected:

    /** Constructor. */
    Singleton() {}

    /** Constructor. */
    Singleton( const Singleton & ) {}

private:

    static TYPE *m_instance;    ///< instance
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

template < class TYPE > TYPE* sim::Singleton< TYPE >::m_instance = 0;

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_SINGLETON_H
