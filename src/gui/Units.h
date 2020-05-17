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

#include <QDomDocument>
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
            Text role;              ///< role
            Text powerplant;        ///< powerplant
            Text armament;          ///< armament

            QString manufacturer;   ///< manufacturer
            QString firstFlight;    ///< first flight date
            QString introduction;   ///< introduction date
            QString retired;        ///< retired date
            QString primaryUser;    ///< primary user
            QString crew;           ///< crew

            int numberBuilt;        ///< number built

            double length;          ///< [m] length
            double wingspan;        ///< [m] wingspan
            double height;          ///< [m] height
            double MTOW;            ///< [kg] maximum take-off weight
            double maxSpeed;        ///< [km/h] maximum speed
            double range;           ///< [km] range


        };

        /** Maritime unit data struct. */
        struct DataMarine
        {
            Text type;              ///< type
            Text armament;          ///< armament

            int numberBuilt;        ///< number built
            int complement;         ///< complement

            double displacement;    ///< [t] displacement
            double length;          ///< [m] length
            double beam;            ///< [m] beam
            double draft;           ///< [m] draft
            double maxSpeed;        ///< [kts] maximum speed
        };

        /** Ground unit data struct. */
        struct DataGround
        {
            Text type;              ///< type
            Text armament;          ///< armament

            int numberBuilt;        ///< number built
        };

        UInt32 index;               ///< unit index

        Type type;                  ///< unit type
        Text name;                  ///< unit name

        DataAerial aerial;          ///< aerial unit data
        DataMarine marine;          ///< maritime unit data
        DataGround ground;          ///< ground unit data
    };

    UInt32 getCount() const { return _units.size(); }

    Data getData( int index ) const;

private:

    /** Constructor. */
    Units();

    QVector< Data > _units;         ///< units data

    void readFile( const QString &path );

    void readUnits( QDomElement &rootNode );
    void readUnits( QDomElement &nodeGroup, Type type );

    void readUnit( QDomElement &nodeUnit, Type type );
    void readUnit( QDomElement &nodeInfo, Data::DataAerial &data );
    void readUnit( QDomElement &nodeInfo, Data::DataMarine &data );
    void readUnit( QDomElement &nodeInfo, Data::DataGround &data );
};

////////////////////////////////////////////////////////////////////////////////

#endif // UNITS_H
