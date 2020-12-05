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
#ifndef SIM_MANIPULATORSHIFT_H
#define SIM_MANIPULATORSHIFT_H

////////////////////////////////////////////////////////////////////////////////

#include <osgGA/CameraManipulator>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** @brief Shift view camera manipulator class. */
class ManipulatorShift : public osgGA::CameraManipulator
{
    typedef CameraManipulator inherited;

public:

    /** @brief Constructor. */
    ManipulatorShift();

    /** @brief Constructor. */
    ManipulatorShift( const ManipulatorShift &cmi,
                      const osg::CopyOp &copyOp = osg::CopyOp::SHALLOW_COPY );

    void setByMatrix( const osg::Matrixd &matrix );
    void setByInverseMatrix( const osg::Matrixd &matrix );
    osg::Matrixd getMatrix() const;
    osg::Matrixd getInverseMatrix() const;

    bool handle( const osgGA::GUIEventAdapter &ea,
                 osgGA::GUIActionAdapter &us );

private:

    osg::Matrixd _matrix;

    /** */
    void updateMatrix();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_MANIPULATORSHIFT_H
