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
#ifndef SIM_ANGLES_H
#define SIM_ANGLES_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Quat>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/**
 * Bryant (rotation angles in z-y-x convention) angles class.
 * @see https://en.wikipedia.org/wiki/Euler_angles
 */
class Angles
{
public:

    /** Constructor. */
    Angles();

    /** Copy constructor. */
    Angles( const Angles &angl );

    /**
     * Constructor.
     * @param [rad] angle of rotation along x-axis
     * @param [rad] angle of rotation along y-axis
     * @param [rad] angle of rotation along z-axis
     */
    Angles( double phi, double tht, double psi );

    /** Constructor. */
    Angles( const osg::Quat &quat );

    /** Normalizes angles. */
    void normalize();

    inline double   phi() const { return m_phi; }
    inline double   tht() const { return m_tht; }
    inline double   psi() const { return m_psi; }
    inline double & phi()       { return m_phi; }
    inline double & tht()       { return m_tht; }
    inline double & psi()       { return m_psi; }

    osg::Quat getRotate() const;

    /** Sets angles values. */
    void set( double phi, double tht, double psi );

    /** */
    void set( const osg::Quat &quat );

    /** Assignment operator. */
    Angles& operator= ( const Angles &angl );

    /** Equality operator. */
    bool operator== ( const Angles &angl ) const;

    /** Inequality operator. */
    bool operator!= ( const Angles &angl ) const;

private:

    double m_phi;   ///< [rad] angle of rotation about x-axis
    double m_tht;   ///< [rad] angle of rotation about y-axis
    double m_psi;   ///< [rad] angle of rotation about z-axis
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_ANGLES_H
