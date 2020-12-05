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
#ifndef HID_MANAGER_H
#define HID_MANAGER_H

////////////////////////////////////////////////////////////////////////////////

#include <string>

#include <sim/utils/sim_Singleton.h>

#include "hid_Assignment.h"

////////////////////////////////////////////////////////////////////////////////

namespace hid
{

/** HID manager class. */
class Manager : public sim::Singleton< Manager >
{
    friend class sim::Singleton< Manager >;

public:

    static const std::string _actionNames[ HID_MAX_ACTIONS ];
    static const std::string _keysNames[ HID_MAX_KEYS ];

    /** */
    static Assignment::POVs getPOV( short pov_deg );

    /** */
    static bool isAxis( Assignment::Action action );

private:

    /** Using this constructor is forbidden. */
    Manager( const Manager & ) : sim::Singleton< Manager >() {}

    /** @brief Constructor. */
    Manager();

public:

    /** @brief Destructor. */
    virtual ~Manager();

    /** */
    void init();

    /** */
    void update( double timeStep );

    inline bool getTriggerBasic() const { return _trigger_basic; }
    inline bool getTriggerExtra() const { return _trigger_extra; }

    inline float getCtrlRoll()  const { return _ctrlRoll;  }
    inline float getCtrlPitch() const { return _ctrlPitch; }
    inline float getCtrlYaw()   const { return _ctrlYaw;   }
    inline float getThrottle()  const { return _throttle;  }

    void setThrottle( float throttle );

    /** */
    void setAssingment( Assignment::Action action, const Assignment &assignment );

    /** */
    inline void setKeysState( const bool *keysState )
    {
        for ( int i = 0; i < HID_MAX_KEYS; i++ )
        {
            _keysState[ i ] = keysState[ i ];
        }
    }

private:

    Assignment _assignments[ HID_MAX_ACTIONS ];

    bool _keysState[ HID_MAX_KEYS ];

    bool _trigger_basic;        ///< primary trigger
    bool _trigger_extra;        ///< secondary trigger

    float _speedControls;       ///< [1/s]
    float _speedThrottle;       ///< [1/s]

    double _timeStep;           ///< [s]

    float _ctrlRoll;            ///< -1.0 ... 1.0
    float _ctrlPitch;           ///< -1.0 ... 1.0
    float _ctrlYaw;             ///< -1.0 ... 1.0
    float _throttle;            ///<  0.0 ... 1.0

    /** */
    void getAxisValue( const Assignment &assignment, float &value, int absolute = 0 );

    /** */
    void getCtrlValue( Assignment::Action decreaseAction,
                       Assignment::Action increaseAction,
                       float &value,
                       float speed,
                       float min,
                       float max );

    /** */
    void getRealValue( Assignment::Action decreaseAction,
                       Assignment::Action increaseAction,
                       float &value,
                       float speed,
                       float min,
                       float max );

    /** */
    void getRealValue( Assignment::Action applyAction,
                       float &value,
                       float speed,
                       float min,
                       float max );

    /** */
    void getRealValue( Assignment::Action toggleAction,
                       bool  &togglePrev,
                       bool  &state,
                       float &value,
                       float speed,
                       float min,
                       float max );

    /** */
    bool getButtState( const Assignment &assignment );

    /** */
    void reset();

    /** */
    void updateAxisActions();

    /** */
    void updateMiscActions();
};

} // end of hid namepsace

////////////////////////////////////////////////////////////////////////////////

#endif // HID_MANAGER_H
