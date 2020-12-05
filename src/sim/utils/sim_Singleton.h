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

/** @brief Singleton template class. */
template < class TYPE >
class Singleton
{
public:

    /** @brief Instace accessor. */
    inline static TYPE* instance()
    {
        if ( !_instance )
        {
            _instance = new TYPE();
        }

        return _instance;
    }

public:

    /** @brief Destructor. */
    virtual ~Singleton() {}

protected:

    /** @brief Constructor. */
    Singleton() {}

    /** @brief Constructor. */
    Singleton( const Singleton & ) {}

private:

    static TYPE *_instance;     ///< instance
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

template < class TYPE > TYPE* sim::Singleton< TYPE >::_instance = 0;

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_SINGLETON_H
