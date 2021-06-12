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
#ifndef UNITS_H
#define UNITS_H

////////////////////////////////////////////////////////////////////////////////

#include <QDomDocument>
#include <QString>
#include <QVector>

#include <defs.h>

////////////////////////////////////////////////////////////////////////////////

/** @brief Units data class. */
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

    /** @brief Constructor. */
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
