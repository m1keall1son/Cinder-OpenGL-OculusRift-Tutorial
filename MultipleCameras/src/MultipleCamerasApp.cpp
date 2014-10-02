#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/TriMesh.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Material.h"
#include "cinder/ObjLoader.h"

using namespace ci;
using namespace ci::app;
using namespace std;

//flat shaded vs and fs

static std::string vertex_flat = {
    "#version 120 \n "
    "varying vec4 vColor; \n"
    "void main(){  \n"
        " gl_TexCoord[0] = gl_MultiTexCoord0;  \n"
        " vec3 wcNormal = gl_NormalMatrix * gl_Normal;  \n"
        " vec4 wcPosition = gl_ModelViewMatrix * gl_Vertex;"
        " vec3 s = normalize(vec3(gl_LightSource[0].position - wcPosition)); \n"
        " gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n"
        " vColor =  gl_LightSource[0].ambient * gl_FrontMaterial.ambient;"
        " vColor += gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * max( dot( s , wcNormal ), 0.0 );"
    "}"
};

static std::string fragment_flat = {
    "#version 120  \n"
    "varying vec4 vColor; \n"
    "void main(){  \n"
        "gl_FragColor.rgb = vColor.rgb;  \n"
        "gl_FragColor.a = 1.0;  \n"
    "}"
};


typedef std::shared_ptr<gl::Light> LightRef;

class MultipleCamerasApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );
    void prepareSettings( Settings* settings ){ settings->setFullScreen(); }
	void update();
	void draw();
    void drawCube();
    
    CameraPersp mCamera, mCamera2;
    gl::Fbo mFbo;
    
    gl::Material mMaterial, mNoMat;
    TriMesh mMesh;
    
    gl::GlslProgRef mFlatShading;
    LightRef mLight;
    
};

void MultipleCamerasApp::setup()
{
    
    //load the geometry
    ObjLoader loader = ObjLoader( loadAsset("../../primitives/cube.obj") );
    //fill the mesh with the data
    loader.load(&mMesh);
    
    //create the shaders from the above strings
    mFlatShading = gl::GlslProg::create( vertex_flat.c_str(), fragment_flat.c_str() );
    
    
    //set up cameras and seperate rendering fbo
    mCamera.setPerspective(45, getWindowAspectRatio(), .1, 10000);
    mCamera.lookAt(Vec3f(5,5,5), Vec3f(0,0,0), Vec3f::yAxis() );

    mCamera2.setPerspective(45, getWindowAspectRatio(), .1, 10000);
    mCamera2.lookAt(Vec3f(5,5,5), Vec3f(0,0,0), Vec3f::yAxis() );

    //create fbo format with depth, RGBA 32 float color and 4 samples of antialiasing
    gl::Fbo::Format format;
    format.enableDepthBuffer();
    format.setColorInternalFormat(GL_RGBA32F_ARB);
    format.setSamples( 4 ); //set antialiasing sample level
    
    //create fbo with width height and format
    mFbo = gl::Fbo(getWindowWidth()/3,getWindowHeight()/3, format);
    
    //set material with ambient and diffuse and apply it
    mMaterial = gl::Material( ColorA(.1,.1,.1,1.), ColorA(1.,0.5,0.,1.) );

    //create a shared ptr to the light
    mLight = LightRef( new gl::Light( gl::Light::Type::POINT, 0) );
    mLight->enable();
    mLight->setDiffuse( Color(.9,.1,.1) );
    mLight->setSpecular( Color(1.,1.,1.) );
    mLight->setAmbient( Color(.1,.1,.1) );
    mLight->setPosition( Vec3f(0,3,1) );
    mLight->setAttenuation(1.,1.,1.);

    //enable depth
    gl::enableDepthWrite();
    gl::enableDepthRead();

}

void MultipleCamerasApp::mouseDown( MouseEvent event )
{
}

void MultipleCamerasApp::drawCube(){
    
    mFlatShading->bind();
    gl::draw( mMesh );
    mFlatShading->unbind();
    
}

void MultipleCamerasApp::update()
{
    
    gl::enable(GL_LIGHTING);
    mMaterial.apply();
    
    //make the camera go in a circle
    mCamera2.setEyePoint(Vec3f( 3.0*cos( getElapsedSeconds() ), 3, 3.0*sin( getElapsedSeconds() ) ));
    mCamera2.setCenterOfInterestPoint(Vec3f(0,0,0));
    
    //bind the fbo and draw the cube in there from the persepective of camera 2
    mFbo.bindFramebuffer();
    gl::clear( Color(0.,0.,0.) );
    gl::color(1.,1.,1.);
    gl::pushMatrices();
    gl::setMatrices(mCamera2);
    gl::setViewport(mFbo.getBounds());
    
    mLight->update(mCamera2);
    drawCube();
    
    gl::popMatrices();
    mFbo.unbindFramebuffer();

    
}

void MultipleCamerasApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

    //set the matrices to camera 1 and draw the cube to the main screen
    gl::pushMatrices();
    gl::setMatrices(mCamera);
    gl::setViewport(getWindowBounds());
    
    mLight->update(mCamera);
    drawCube();
    
    //disable lighting to draw debugs and 2D stuff
    gl::disable(GL_LIGHTING);
    gl::drawFrustum(mCamera2);
    
    gl::popMatrices();
    
    //set matrices for 2D drawing
    gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
    
    Rectf camera2Area = Rectf( Vec2f( getWindowWidth()-mFbo.getWidth(), getWindowHeight()-mFbo.getHeight() ),
                               Vec2f( getWindowWidth(), getWindowHeight() )
                              );
    //draw the other camera texture from the other render fbo
    gl::Texture tex = mFbo.getTexture();
    tex.setFlipped();
    gl::draw( tex , camera2Area );
    //draw a border
    gl::drawStrokedRect(camera2Area);
}

CINDER_APP_NATIVE( MultipleCamerasApp, RendererGl )
