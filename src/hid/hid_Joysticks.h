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
#ifndef HID_JOYSTICKS_H
#define HID_JOYSTICKS_H

////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#       define HID_WINMM_JOYSTICK
#else
#   ifdef _LINUX_
#       define HID_LINUX_JOYSTICK
#   endif
#endif

////////////////////////////////////////////////////////////////////////////////

#include <string>

#ifdef HID_LINUX_JOYSTICK
#   include <fcntl.h>
#   include <unistd.h>
#   include <linux/joystick.h>
#endif

#ifdef HID_WINMM_JOYSTICK
#   include <Windows.h>
#endif

#include <sim/utils/sim_Singleton.h>

////////////////////////////////////////////////////////////////////////////////

#ifdef HID_LINUX_JOYSTICK
#   define HID_MAX_JOYS 32
#   define HID_MAX_POVS 4
#   define HID_MAX_AXES 24
#   define HID_MAX_BUTT 32
#   define HID_AXIS_RANGE 32767.0f
#endif

#ifdef HID_WINMM_JOYSTICK
#   define HID_MAX_JOYS 2
#   define HID_MAX_POVS 1
#   define HID_MAX_AXES 6
#   define HID_MAX_BUTT 32
#   define HID_AXIS_RANGE 65535.0f
#endif

#ifndef HID_AXIS_RANGE
#   error 'HID_AXIS_RANGE' not defined!
#endif

////////////////////////////////////////////////////////////////////////////////

namespace hid
{

/** This class handles joystick like input devices. */
class Joysticks : public sim::Singleton< Joysticks >
{
    friend class sim::Singleton< Joysticks >;

public:

    /** */
    enum Axes
    {
        AxisX = 0,                  ///<  0: X-axis deflection
        AxisY,                      ///<  1: Y-axis deflection

#       ifdef HID_LINUX_JOYSTICK
        AxisZ,                      ///<  2: Z-axis deflection
        AxisRX,                     ///<  3: Rotation about X-axis
        AxisRY,                     ///<  4: Rotation about Y-axis
        AxisRZ,                     ///<  5: Rotation about Z-axis
        Throttle,                   ///<  6: Throttle
        Rudder,                     ///<  7: Rudder
        Gas,                        ///<  8: Gas
        Wheel,                      ///<  9: Wheel
        Brake,                      ///< 10: Brake
        Hat0X,                      ///< 11: Hat 0 X-axis
        Hat0Y,                      ///< 12: Hat 0 Y-axis
        Hat1X,                      ///< 13: Hat 1 X-axis
        Hat1Y,                      ///< 14: Hat 1 Y-axis
        Hat2X,                      ///< 15: Hat 2 X-axis
        Hat2Y,                      ///< 16: Hat 2 Y-axis
        Hat3X,                      ///< 17: Hat 3 X-axis
        Hat3Y,                      ///< 18: Hat 3 Y-axis
        Pressure,                   ///< 19:
        Distance,                   ///< 20:
        TiltX,                      ///< 21:
        TiltY,                      ///< 22:
        ToolWidth                   ///< 23:
#       endif

#       ifdef HID_WINMM_JOYSTICK
        AxisZ,                      ///<  2: Z-axis deflection
        AxisR,                      ///<  3:
        AxisU,                      ///<  4:
        AxisV                       ///<  5:
#       endif
    };

    /** */
    struct Data
    {
        std::string name;               ///< device name
        
        float axis[ HID_MAX_AXES ];     ///< -1.0 ... 1.0 normalized axis position
        bool  butt[ HID_MAX_BUTT ];     ///< false: released, true: pressed
        short povs[ HID_MAX_POVS ];     ///< [deg] POVs

        short axisCount;                ///< number of device axes
        short buttCount;                ///< number of devise buttons
        short povsCount;                ///< number of devices POVs

        bool active;                    ///< specifies active device
        bool feedback;                  ///< specifies FF capable device

        bool hasAxis[ HID_MAX_AXES ];   ///<

#       ifdef HID_LINUX_JOYSTICK
        bool hasPOV[ HID_MAX_POVS ];    ///<

        Axes axesMap[ HID_MAX_AXES + 2*HID_MAX_POVS ];  ///<
#       endif

#       ifdef HID_WINMM_JOYSTICK
        bool hasPOV;                    ///<
#       endif
    };

private:

    /**
     * You should use static function getInstance() due to get refernce
     * to Joytsicks class instance.
     */
    Joysticks();

    /** Using this constructor is forbidden. */
    Joysticks( const Joysticks & ) : sim::Singleton< Joysticks >() {}

public:

    static const std::string _axisNames[ HID_MAX_AXES ];

    /** Destructor. */
    virtual ~Joysticks();

    /** This function initiate Joystick object. */
    void init();
    
    /** This function updates Joystick object. */
    void update();

    /** @return number of active joysticks */
    short getJoysCount() const;

    /**
     * @param joystick index number
     * @return joystick data
     */
    Joysticks::Data getJoyData( short joyNum ) const;

private:

    short _joysCount;   ///< number of active joysticks

    Joysticks::Data _joyData[ HID_MAX_JOYS ];

#   ifdef HID_LINUX_JOYSTICK
    int _joysFD[ HID_MAX_JOYS ];
    js_event _joyEvent;
#   endif

#   ifdef HID_WINMM_JOYSTICK
    DWORD _buttons[ HID_MAX_BUTT ];
#   endif
};

} // end of hid namepsace

////////////////////////////////////////////////////////////////////////////////

#endif // HID_JOYSTICKS_H
