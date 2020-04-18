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

#include <gui/MessageBox.h>

////////////////////////////////////////////////////////////////////////////////

MessageBox::StandardButton MessageBox::question( QWidget *parent,
                                                 const QString &title,
                                                 const QString& text )
{
    MessageBox msgBox( title, text, Yes | No, parent );
    msgBox.setDefaultButton( No );

    if ( msgBox.exec() == -1 )
    {
        return QMessageBox::Cancel;
    }

    return msgBox.standardButton( msgBox.clickedButton() );
}

////////////////////////////////////////////////////////////////////////////////

MessageBox::MessageBox( const QString &title, const QString &text,
                        StandardButtons buttons, QWidget *parent ) :
    QMessageBox( NoIcon, title, text, buttons, parent,
                 Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::FramelessWindowHint )
{}

////////////////////////////////////////////////////////////////////////////////

MessageBox::~MessageBox() {}
