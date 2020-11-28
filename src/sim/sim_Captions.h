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
#ifndef SIM_CAPTIONS_H
#define SIM_CAPTIONS_H

////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#include <sim/sim_Types.h>

#include <sim/utils/sim_Singleton.h>
#include <sim/utils/sim_Text.h>
#include <sim/utils/sim_XmlNode.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** */
class Captions : public sim::Singleton< Captions >
{
    friend class sim::Singleton< Captions >;

private:

    /** Default constructor. */
    Captions();

    /** Copy constructor. */
    Captions( const Captions & ) : sim::Singleton< Captions >() {}

public:

    /** Destructor. */
    virtual ~Captions();

    inline std::string getLoading        () const { return _loading         .get(); }
    inline std::string getResume         () const { return _resume          .get(); }
    inline std::string getBegin          () const { return _begin           .get(); }
    inline std::string getMissionSuccess () const { return _mission_success .get(); }
    inline std::string getMissionFailure () const { return _mission_failure .get(); }
    inline std::string getFriendlyFire   () const { return _friendly_fire   .get(); }
    inline std::string getTargetKilled   () const { return _target_killed   .get(); }
    inline std::string getTargetHit      () const { return _target_hit      .get(); }

private:

    Text _loading;          ///<
    Text _resume;           ///<
    Text _begin;            ///<
    Text _mission_success;  ///<
    Text _mission_failure;  ///<
    Text _friendly_fire;    ///<
    Text _target_killed;    ///<
    Text _target_hit;       ///<

    void readCaptions();

    void readText( XmlNode &node, Text &text, const char *name );
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_CAPTIONS_H
