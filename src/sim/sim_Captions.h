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

    /** @brief Default constructor. */
    Captions();

    /** @brief Copy constructor. */
    Captions( const Captions & ) : sim::Singleton< Captions >() {}

public:

    /** @brief Destructor. */
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
