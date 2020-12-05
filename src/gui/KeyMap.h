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
#ifndef KEYMAP_H
#define KEYMAP_H

////////////////////////////////////////////////////////////////////////////////

#include <hid/hid_Assignment.h>

#include <osgGA/GUIEventAdapter>

////////////////////////////////////////////////////////////////////////////////

/** @brief Key mapping class. */
class KeyMap
{
public:

    static hid::Assignment::Key remapHID( int key_qt );

    static osgGA::GUIEventAdapter::KeySymbol remapOSG( int key_qt );
};

////////////////////////////////////////////////////////////////////////////////

#endif // KEYMAP_H
