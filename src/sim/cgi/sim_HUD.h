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
#ifndef SIM_HUD_H
#define SIM_HUD_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osg/Switch>

#include <osgText/Text>

#include <sim/sim_Base.h>
#include <sim/sim_Types.h>

////////////////////////////////////////////////////////////////////////////////

namespace sim
{

/** Head-up-Display view class. */
class HUD : public Base
{
public:

    static const UInt16 _maxRadarMarks;     ///< maximum number of radar marks

    static const float _sizeCaptions;       ///< captions font size
    static const float _sizePlayerBar;      ///< player bar font size
    static const float _sizeMessage;        ///< message font size

    static const float _deg2px;             ///<
    static const float _rad2px;             ///<

    static const float _rad11deg;           ///< [rad] 11 deg

    /** Constructor. */
    HUD( float linesWidth, int width, int height );

    /** Destructor. */
    virtual ~HUD();

    /** Initializes HUD. */
    void init( bool tutorial );

    /** */
    void load();

    /** Updates HUD. */
    void update();

    /** Returns OSG node.  */
    inline osg::Group* getNode() { return _root.get(); }

private:

    const float _linesWidth;            ///< [px] lines width

    const int _width;                   ///< [px] viewport width
    const int _height;                  ///< [px] viewport height

    const int _maxX;                    ///< [px] maximum x-coordinate value (depending on screen ratio)

    osg::ref_ptr<osg::Group> _root;     ///< HUD root group

    osg::ref_ptr<osg::Switch> _switch;  ///< HUD master switch

    osg::ref_ptr<osgText::Font> _font;  ///< HUD font

    osg::ref_ptr<osg::Switch> _switchCaption;               ///< captions switch
    osg::ref_ptr<osg::Switch> _switchEnemyIndicators;       ///< enemy indicators switch
    osg::ref_ptr<osg::Switch> _switchMessage;               ///< message switch
    osg::ref_ptr<osg::Switch> _switchPointerCustom;         ///< custom pointer switch
    osg::ref_ptr<osg::Switch> _switchPointerTarget;         ///< target cue pointer switch
    osg::ref_ptr<osg::Switch> _switchRadarMarksEnemy;       ///< radar marks switch enemy
    osg::ref_ptr<osg::Switch> _switchRadarMarksFriend;      ///< radar marks switch friend
    osg::ref_ptr<osg::Switch> _switchTargetIndicators;      ///< target indicator switch
    osg::ref_ptr<osg::Switch> _switchWaypointIndicators;    ///< waypoint indicators switch
#   ifndef SIM_DESKTOP
    osg::ref_ptr<osg::Switch> _switchTrigger;               ///< trigger switch
    osg::ref_ptr<osg::Switch> _switchPointerRpm;            ///< RPM pointer switch
    osg::ref_ptr<osg::Switch> _switchPointerTrigger;        ///< trigger pointer switch
    osg::ref_ptr<osg::Switch> _switchTutorialTips;          ///< tutorial tips switch (only in tutorial mode)
#   endif

    osg::ref_ptr<osg::PositionAttitudeTransform> _patCaption;           ///< caption position and scale
    osg::ref_ptr<osg::PositionAttitudeTransform> _patCrosshair;         ///< crosshair (target sight) position
    osg::ref_ptr<osg::PositionAttitudeTransform> _patHitIndicator;      ///< hit indicator position and attitude
    osg::ref_ptr<osg::PositionAttitudeTransform> _patIndicatorALT1;     ///< altimeter 1000 ft hand rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> _patIndicatorALT2;     ///< altimeter 100 ft hand rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> _patIndicatorASI;      ///< airspeed inidicator hand rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> _patIndicatorRadar;    ///< radar heading rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> _patIndicatorVSI;      ///< vertical speed indicator hand rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> _patPlayerBar;         ///< player health bar scale and position
    osg::ref_ptr<osg::PositionAttitudeTransform> _patPointerCustom;     ///< custom pointer position and rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> _patTargetBar;         ///< target health bar position and scaler
    osg::ref_ptr<osg::PositionAttitudeTransform> _patTargetBox;         ///< target box position
    osg::ref_ptr<osg::PositionAttitudeTransform> _patTargetCue;         ///< target cue postion relative to target box
    osg::ref_ptr<osg::PositionAttitudeTransform> _patTargetDir;         ///< target direction indicator rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> _patWaypointBox;       ///< wayposint box position
    osg::ref_ptr<osg::PositionAttitudeTransform> _patWaypointDir;       ///< waypoint direction indicator rotation
#   ifndef SIM_DESKTOP
    osg::ref_ptr<osg::PositionAttitudeTransform> _patControlsThrottle;  ///< throttle grip position
    osg::ref_ptr<osg::PositionAttitudeTransform> _patPointerRpm;        ///< RPM pointer position
#   endif

    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > _patRadarMarksEnemy;    ///< list of enemy markings PATs
    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > _patRadarMarksFriend;   ///< list of friend markings PATs

    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > _patEnemyBox;           ///< target box position
    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > _patEnemyDir;           ///< target direction indicator rotation

    osg::ref_ptr<osg::Geode> _geodeRadarMarkEnemy;      ///< radar mark geode enemy
    osg::ref_ptr<osg::Geode> _geodeRadarMarkFriend;     ///< radar mark geode friend

    osg::ref_ptr<osg::Geometry> _hitIndicator;          ///< hit indicator geometry
    osg::ref_ptr<osg::Geometry> _playerLifeBar;         ///< player life bar geometry
    osg::ref_ptr<osg::Geometry> _targetLifeBar;         ///< target life bar geometry
#   ifndef SIM_DESKTOP
    osg::ref_ptr<osg::Geometry> _tutorialTip;           ///< tutorial tip
#   endif

    osg::ref_ptr<osg::Material> _materialIndicators;    ///< indicators material
#   ifndef SIM_DESKTOP
    osg::ref_ptr<osg::Material> _materialControls;      ///< controls material
#   endif

    osg::ref_ptr<osgText::Text> _textPlayerHP;          ///< player's hit points text
    osg::ref_ptr<osgText::Text> _textCaption;           ///< caption text
    osg::ref_ptr<osgText::Text> _textMessage;           ///< message text

    bool _tutorial;             ///< specifies if HUD is in tutorial mode

    float _timerTutorial;       ///< [s] timer for tutorial symbols

    void createBox( osg::Geode *geode, osg::Vec4 color, float width = 1.0f );

    void createCaption();

#   ifndef SIM_DESKTOP
    void createControls();
    void createControlsThrottle();
    void createControlsThrottleGrip( osg::Group *parent, float alpha, float z_offset = 0.0f );
    void createControlsTrigger();
#   endif

    void createCrosshair();

    void createDir( osg::Geode *geode, osg::Vec4 color );

    void createHitIndicator();

    void createEnemyIndicators();

    /**
     * Creates indicator face.
     * @param parent parent group
     * @param radius face radius
     * @param material face material
     * @param textureFile texture file "independent" path
     * @param alpha face transparency
     */
    void createFace( osg::Group *parent, float radius,
                     osg::Material *material,
                     const std::string &textureFile,
                     float alpha = 1.0f );

    void createIndicators();
    void createIndicatorHand( osg::Group *parent, float l, float w );
    void createIndicatorALT();
    void createIndicatorASI();
    void createIndicatorRadar();
    void createIndicatorVSI();

    void createMessage();

    void createPlayerBar();

    void createPointer( osg::Group *parent );

    void createRadarMark( osg::Geode *geode, osg::Vec4 color );

    void createTargetIndicators();
    void createTargetIndicatorBar();
    void createTargetIndicatorBox();
    void createTargetIndicatorCue();
    void createTargetIndicatorDir();

    void createTutorialSymbols();
#   ifndef SIM_DESKTOP
    void createTutorialTips();
#   endif

    void createWaypointIndicators();
    void createWaypointIndicatorBox();
    void createWaypointIndicatorDir();

    void updateCaption();
    void updateControls();
    void updateCrosshair();
    void updateHitIndicator();
    void updateIndicators();
    void updateIndicatorRadar();
    void updateMessage();
    void updatePlayerBar();
    void updateTargetIndicators();
    void updateTutorialSymbols();
    void updateWaypointIndicators();
};

} // end of sim namespace

////////////////////////////////////////////////////////////////////////////////

#endif // SIM_HUD_H
