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
#ifndef UNITS_H
#define UNITS_H

////////////////////////////////////////////////////////////////////////////////

#include <QString>
#include <QVector>

#include <defs.h>

////////////////////////////////////////////////////////////////////////////////

/** Units data class. */
class Units : public Singleton< Units >
{
    friend class sim::Singleton< Units >;

public:

    enum Type
    {
        Aerial = 0,                 ///< aerial unit
        Marine = 1,                 ///< maritime unit
        Ground = 2                  ///< ground unit
    };

    /** */
    struct Data
    {
        /** Aerial unit data struct. */
        struct DataAerial
        {

        };

        /** Maritime unit data struct. */
        struct DataMarine
        {

        };

        /** Ground unit data struct. */
        struct DataGround
        {

        };

        /** Unit data union. */
        union DataUnion
        {
            DataAerial aerial;      ///< aerial unit data
            DataMarine marine;      ///< maritime unit data
            DataGround ground;      ///< ground unit data
        };

        UInt32 index;               ///< unit index

        Type type;                  ///< unit type
        Text name;                  ///< unit name

        DataUnion data;             ///< unit data
    };

    UInt32 getCount() const { return _units.size(); }

    Data getData( UInt32 index ) const;

private:

    /** Constructor. */
    Units();

    QVector< Data > _units;         ///< units data

    void readFile( const std::string &file );
    void readUnits( XmlNode &rootNode );
    void readUnits( XmlNode &groupNode, Type type );
    void readUnit( XmlNode &unitNode, Type type );
};

////////////////////////////////////////////////////////////////////////////////

#endif // UNITS_H
