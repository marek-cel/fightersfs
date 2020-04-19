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

#include <sim/entities/sim_UnitMarine.h>

#ifdef SIM_TEST
#   include <osg/ClipNode>
#   include <osg/Geode>
#   include <osg/Geometry>
#   include <osg/LOD>
#   include <osg/MatrixTransform>
#   include <osg/Program>

#   include <sim/cgi/sim_Textures.h>
#endif

#include <sim/cgi/sim_Effects.h>

#include <sim/entities/sim_Explosion.h>
#include <sim/entities/sim_WreckageSurface.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_DESKTOP
const char UnitMarine::_frag[] =
    "uniform sampler2D reflection;\n"
    "uniform sampler2D refraction;\n"
    "uniform sampler2D normalTex;\n"
    "varying vec4 projCoords;\n"
    "varying vec3 lightDir, eyeDir;\n"
    "varying vec2 flowCoords, rippleCoords;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec2 rippleEffect = 0.02 * texture2D(refraction, rippleCoords * 0.1).xy;\n"
    "   vec4 N = texture2D(reflection, flowCoords + rippleEffect);\n"
    "   N = N * 2.0 - vec4(1.0);\n"
    "   N.a = 1.0; N = normalize(N);\n"
    "\n"
    "   vec3 refVec = normalize(reflect(-lightDir, vec3(N) * 0.6));\n"
    "   float refAngle = clamp(dot(eyeDir, refVec), 0.0, 1.0);\n"
    "   vec4 specular = vec4(pow(refAngle, 40.0));\n"
    "\n"
    "   vec2 dist = texture2D(refraction, flowCoords + rippleEffect).xy;\n"
    "   float dist_inv = 1.0 / sqrt( dist.x*dist.x + dist.y*dist.y );\n"
    "   dist = (dist * 2.0 - vec2(1.0)) * dist_inv * 0.02;\n"
    "   vec2 uv = projCoords.xy / projCoords.w;\n"
    "   uv = clamp((uv + 1.0) * 0.5 + dist, 0.0, 1.0);\n"
    "\n"
    "   vec4 refl = texture2D(reflection, uv);\n"
    "   refl.w = refl.w * 0.4;\n"
    "   gl_FragColor = mix(refl, refl + specular, 0.6);\n"
    "}\n"
    "\n";

const char UnitMarine::_vert[] =
    "uniform float osg_FrameTime;\n"
    "varying vec4 projCoords;\n"
    "varying vec3 lightDir, eyeDir;\n"
    "varying vec2 flowCoords, rippleCoords;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec3 T = vec3(0.0, 1.0, 0.0);\n"
    "   vec3 N = vec3(0.0, 0.0, 1.0);\n"
    "   vec3 B = vec3(1.0, 0.0, 0.0);\n"
    "   T = normalize(gl_NormalMatrix * T);\n"
    "   B = normalize(gl_NormalMatrix * B);\n"
    "   N = normalize(gl_NormalMatrix * N);\n"
    "\n"
    "   mat3 TBNmat;\n"
    "   TBNmat[0][0] = T[0]; TBNmat[1][0] = T[1]; TBNmat[2][0] = T[2];\n"
    "   TBNmat[0][1] = B[0]; TBNmat[1][1] = B[1]; TBNmat[2][1] = B[2];\n"
    "   TBNmat[0][2] = N[0]; TBNmat[1][2] = N[1]; TBNmat[2][2] = N[2];\n"
    "\n"
    "   vec3 vertexInEye = vec3(gl_ModelViewMatrix * gl_Vertex);\n"
    "   lightDir =  gl_LightSource[0].position.xyz - vertexInEye;\n"
    "   lightDir = normalize(TBNmat * lightDir);\n"
    "   eyeDir = normalize(TBNmat * (-vertexInEye));\n"
    "\n"
    "   vec2 t1 = vec2(osg_FrameTime*0.02, osg_FrameTime*0.02);\n"
    "   vec2 t2 = vec2(osg_FrameTime*0.05, osg_FrameTime*0.05);\n"
    "   flowCoords = gl_MultiTexCoord0.xy * 5.0 + t1;\n"
    "   rippleCoords = gl_MultiTexCoord0.xy * 10.0 + t2;\n"
    "\n"
    "   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
    "   gl_Position = ftransform();\n"
    "   projCoords = gl_Position;\n"
    "}\n"
    "\n";
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef SIM_TEST
void UnitMarine::createReflection( osg::Node *model, osg::Group *parent )
{
    const float z = 0.1f;

    osg::ref_ptr<osg::LOD> lodReflection = new osg::LOD();
    osg::ref_ptr<osg::Group> groupReflection = new osg::Group();

    lodReflection->addChild( groupReflection.get(), 0.0f, 10000.0f );

    osg::ref_ptr<osg::MatrixTransform> reverse = new osg::MatrixTransform;
    reverse->preMult(osg::Matrix::translate(0.0f, 0.0f, -z) *
                     osg::Matrix::scale(1.0f, 1.0f, -1.0f) *
                     osg::Matrix::translate(0.0f, 0.0f, z) );
    reverse->addChild( model );

    osg::ref_ptr<osg::ClipPlane> clipPlane = new osg::ClipPlane;
    clipPlane->setClipPlane( 0.0, 0.0, -1.0, z );
    clipPlane->setClipPlaneNum( 0 );

    osg::ref_ptr<osg::ClipNode> clipNode = new osg::ClipNode;
    clipNode->addClipPlane( clipPlane.get() );
    clipNode->addChild( reverse.get() );

    ////////////////////////////////

    // The RTT camera
    osg::ref_ptr<osg::Texture2D> tex2D = new osg::Texture2D();
    tex2D->setTextureSize( 1024, 1024 );
    tex2D->setInternalFormat( GL_RGBA );
    tex2D->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
    tex2D->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );

    osg::ref_ptr<osg::Camera> cameraRTT = new osg::Camera();

    cameraRTT->setClearColor( osg::Vec4() );
    cameraRTT->setClearMask( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
    cameraRTT->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    cameraRTT->setRenderOrder( osg::Camera::PRE_RENDER );

    cameraRTT->setViewport( 0, 0, tex2D->getTextureWidth(), tex2D->getTextureHeight() );
    cameraRTT->attach( osg::Camera::COLOR_BUFFER, tex2D.get() );

    cameraRTT->addChild( clipNode.get() );

    // The water plane
    const osg::Vec3& center = model->getBound().center();
    //float planeSize = 4.0f * model->getBound().radius();
    float planeSize = 500.0f;

    osg::Vec3 planeCorner( center.x() - 0.5f*planeSize, center.y() - 0.5f*planeSize, z );

    osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry(
                planeCorner,
                osg::Vec3( planeSize, 0.0f, 0.0f ),
                osg::Vec3( 0.0f, planeSize, 0.0f ) );

    osg::ref_ptr<osg::Geode> geodeQuad = new osg::Geode;
    geodeQuad->addDrawable( quad.get() );

    osg::ref_ptr<osg::Texture2D> texWaterDUDV = Textures::get( Path::get( "textures/water_DUDV.png" ) );
    texWaterDUDV->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
    texWaterDUDV->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
    texWaterDUDV->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
    texWaterDUDV->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );

    osg::ref_ptr<osg::Texture2D> texWaterNM = Textures::get( Path::get( "textures/water_NM.png" ) );
    texWaterNM->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
    texWaterNM->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
    texWaterNM->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
    texWaterNM->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );

    osg::ref_ptr<osg::StateSet> stateSet = geodeQuad->getOrCreateStateSet();
    stateSet->setTextureAttributeAndModes( 0, tex2D.get() );
    stateSet->setTextureAttributeAndModes( 1, texWaterDUDV.get() );
    stateSet->setTextureAttributeAndModes( 2, texWaterNM.get() );

    osg::ref_ptr<osg::Program> program = new osg::Program();
    program->addShader( new osg::Shader( osg::Shader::VERTEX   , _vert ) );
    program->addShader( new osg::Shader( osg::Shader::FRAGMENT , _frag ) );
    stateSet->setAttributeAndModes( program.get() );
    stateSet->addUniform( new osg::Uniform("defaultTex", 0) );
    stateSet->addUniform( new osg::Uniform("refraction", 1) );
    stateSet->addUniform( new osg::Uniform("normalTex", 2) );

    ////////////////////////////////

    groupReflection->addChild( model );
    groupReflection->addChild( cameraRTT.get() );
    groupReflection->addChild( geodeQuad.get() );

    parent->addChild( lodReflection.get() );
}
#endif

////////////////////////////////////////////////////////////////////////////////

UnitMarine::UnitMarine( Affiliation affiliation ) : UnitSurface( affiliation ) {}

////////////////////////////////////////////////////////////////////////////////

UnitMarine::~UnitMarine() {}

////////////////////////////////////////////////////////////////////////////////

void UnitMarine::destroy()
{
    if ( isActive() )
    {
        Explosion *explosion = new Explosion( 25.0f );
        explosion->setPos( m_pos );
        explosion->setAtt( m_att );

        WreckageSurface *wreckage = new WreckageSurface( m_model.get(), m_smoke.get() );
        wreckage->setPos( m_pos );
        wreckage->setAtt( m_att );

        if ( m_ownship )
        {
            wreckage->setName( "ownship" );
        }
    }

    ///////////////////////
    UnitSurface::destroy();
    ///////////////////////
}

////////////////////////////////////////////////////////////////////////////////

void UnitMarine::load()
{
    ////////////////////
    UnitSurface::load();
    ////////////////////

#   ifdef SIM_TEST
    if ( m_model.valid() )
    {
        createReflection( m_model.get(), m_switch.get() );
    }
#   endif
}

////////////////////////////////////////////////////////////////////////////////

void UnitMarine::setHP( UInt16 hp )
{
    /////////////////////////
    UnitSurface::setHP( hp );
    /////////////////////////

    if ( m_hp < 50 )
    {
        if ( !m_smoke.valid() )
        {
            m_smoke = Effects::createSmoke( 60.0f, 10.0f, 150.0f, 1.0f, 0.04f, 0.5f );
            m_switch->addChild( m_smoke.get() );
        }
    }
}
