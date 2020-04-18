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

    static const UInt16 m_maxRadarMarks;    ///< maximum number of radar marks

    static const float m_sizeCaptions;      ///< captions font size
    static const float m_sizePlayerBar;     ///< player bar font size
    static const float m_sizeMessage;       ///< message font size

    static const float m_deg2px;            ///<
    static const float m_rad2px;            ///<

    static const float m_rad11deg;          ///< [rad] 11 deg

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
    inline osg::Group* getNode() { return m_root.get(); }

private:

    const float m_linesWidth;           ///< [px] lines width

    const int m_width;                  ///< [px] viewport width
    const int m_height;                 ///< [px] viewport height

    const int m_maxX;                   ///< [px] maximum x-coordinate value (depending on screen ratio)

    osg::ref_ptr<osg::Group> m_root;    ///< HUD root group

    osg::ref_ptr<osg::Switch> m_switch; ///< HUD master switch

    osg::ref_ptr<osgText::Font> m_font; ///< HUD font

    osg::ref_ptr<osg::Switch> m_switchCaption;              ///< captions switch
    osg::ref_ptr<osg::Switch> m_switchEnemyIndicators;      ///< enemy indicators switch
    osg::ref_ptr<osg::Switch> m_switchMessage;              ///< message switch
    osg::ref_ptr<osg::Switch> m_switchPointerCustom;        ///< custom pointer switch
    osg::ref_ptr<osg::Switch> m_switchPointerTarget;        ///< target cue pointer switch
    osg::ref_ptr<osg::Switch> m_switchRadarMarksEnemy;      ///< radar marks switch enemy
    osg::ref_ptr<osg::Switch> m_switchRadarMarksFriend;     ///< radar marks switch friend
    osg::ref_ptr<osg::Switch> m_switchTargetIndicators;     ///< target indicator switch
    osg::ref_ptr<osg::Switch> m_switchWaypointIndicators;   ///< waypoint indicators switch
#   ifndef SIM_DESKTOP
    osg::ref_ptr<osg::Switch> m_switchTrigger;              ///< trigger switch
    osg::ref_ptr<osg::Switch> m_switchPointerRpm;           ///< RPM pointer switch
    osg::ref_ptr<osg::Switch> m_switchPointerTrigger;       ///< trigger pointer switch
    osg::ref_ptr<osg::Switch> m_switchTutorialTips;         ///< tutorial tips switch (only in tutorial mode)
#   endif

    osg::ref_ptr<osg::PositionAttitudeTransform> m_patCaption;          ///< caption position and scale
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patCrosshair;        ///< crosshair (target sight) position
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patHitIndicator;     ///< hit indicator position and attitude
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patIndicatorALT1;    ///< altimeter 1000 ft hand rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patIndicatorALT2;    ///< altimeter 100 ft hand rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patIndicatorASI;     ///< airspeed inidicator hand rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patIndicatorRadar;   ///< radar heading rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patIndicatorVSI;     ///< vertical speed indicator hand rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patPlayerBar;        ///< player health bar scale and position
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patPointerCustom;    ///< custom pointer position and rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patTargetBar;        ///< target health bar position and scaler
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patTargetBox;        ///< target box position
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patTargetCue;        ///< target cue postion relative to target box
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patTargetDir;        ///< target direction indicator rotation
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patWaypointBox;      ///< wayposint box position
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patWaypointDir;      ///< waypoint direction indicator rotation
#   ifndef SIM_DESKTOP
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patControlsThrottle; ///< throttle grip position
    osg::ref_ptr<osg::PositionAttitudeTransform> m_patPointerRpm;       ///< RPM pointer position
#   endif

    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > m_patRadarMarksEnemy;   ///< list of enemy markings PATs
    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > m_patRadarMarksFriend;  ///< list of friend markings PATs

    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > m_patEnemyBox;          ///< target box position
    std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > m_patEnemyDir;          ///< target direction indicator rotation

    osg::ref_ptr<osg::Geode> m_geodeRadarMarkEnemy;     ///< radar mark geode enemy
    osg::ref_ptr<osg::Geode> m_geodeRadarMarkFriend;    ///< radar mark geode friend

    osg::ref_ptr<osg::Geometry> m_hitIndicator;         ///< hit indicator geometry
    osg::ref_ptr<osg::Geometry> m_playerLifeBar;        ///< player life bar geometry
    osg::ref_ptr<osg::Geometry> m_targetLifeBar;        ///< target life bar geometry
#   ifndef SIM_DESKTOP
    osg::ref_ptr<osg::Geometry> m_tutorialTip;          ///< tutorial tip
#   endif

    osg::ref_ptr<osg::Material> m_materialIndicators;   ///< indicators material
#   ifndef SIM_DESKTOP
    osg::ref_ptr<osg::Material> m_materialControls;     ///< controls material
#   endif

    osg::ref_ptr<osgText::Text> m_textPlayerHP;         ///< player's hit points text
    osg::ref_ptr<osgText::Text> m_textCaption;          ///< caption text
    osg::ref_ptr<osgText::Text> m_textMessage;          ///< message text

    bool m_tutorial;            ///< specifies if HUD is in tutorial mode

    float m_timerTutorial;      ///< [s] timer for tutorial symbols

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
