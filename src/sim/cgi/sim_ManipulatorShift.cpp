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

#include <sim/cgi/sim_ManipulatorShift.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

ManipulatorShift::ManipulatorShift() :
    inherited()
{}

////////////////////////////////////////////////////////////////////////////////

ManipulatorShift::ManipulatorShift( const ManipulatorShift &cmi, const osg::CopyOp &copyOp ) :
    osg::Object( cmi, copyOp ),
    inherited( cmi, copyOp ),
    m_matrix( cmi.m_matrix )
{}

////////////////////////////////////////////////////////////////////////////////

void ManipulatorShift::setByMatrix( const osg::Matrixd &matrix )
{
    m_matrix = matrix;
}

////////////////////////////////////////////////////////////////////////////////

void ManipulatorShift::setByInverseMatrix( const osg::Matrixd &matrix )
{
    setByMatrix( osg::Matrixd::inverse( matrix ) );
}

////////////////////////////////////////////////////////////////////////////////

osg::Matrixd ManipulatorShift::getMatrix() const
{
    return m_matrix;
}

////////////////////////////////////////////////////////////////////////////////

osg::Matrixd ManipulatorShift::getInverseMatrix() const
{
    return osg::Matrixd::inverse( m_matrix );
}

////////////////////////////////////////////////////////////////////////////////

bool ManipulatorShift::handle( const osgGA::GUIEventAdapter &ea,
                               osgGA::GUIActionAdapter &/*us*/ )
{
    switch ( ea.getEventType() )
    {
    case osgGA::GUIEventAdapter::FRAME:
        updateMatrix();
        return false;
        break;
    default:
        break;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////

void ManipulatorShift::updateMatrix() {}
