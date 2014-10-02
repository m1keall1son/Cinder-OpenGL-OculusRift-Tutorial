#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Matrix44.h"
#include "cinder/Rand.h"

#include "CameraStereoHMD.h"
#include "OculusVR.h"



using namespace ci;
using namespace ci::app;
using namespace std;

class oculusBaseApp : public AppNative {
public:
	void setup();
	void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event );
    void prepareSettings( Settings* settings );
	void update();
    void render();
	void draw();
    
    ovr::DeviceRef              mOculusVR;
    ovr::DistortionHelperRef    mDistortionHelper;
    CameraStereoHMD             mCamera;
    gl::Fbo                     mFbo;
    
    
};

void oculusBaseApp::prepareSettings( Settings* settings )
{
    // If more than one Display use the second
    // one for the Oculus
    if( Display::getDisplays().size() > 1 ){
        settings->setDisplay( Display::getDisplays()[1] );
    }
    
    // We need a fullscreen window for the Oculus
    settings->setFullScreen();
}

void oculusBaseApp::setup()
{
    mOculusVR = ovr::Device::create();
    mDistortionHelper = ovr::DistortionHelper::create();
    
    
    //set up Oculus screen video
    gl::Fbo::Format format;
    format.enableColorBuffer();
    format.enableDepthBuffer();
    format.setSamples( 8 );
    
    mFbo = gl::Fbo( 1600, 1000, format );
    
    
    //set up eye cameras
    mCamera = CameraStereoHMD( 640, 800, mOculusVR ? mOculusVR->getFov() : 125, mOculusVR ? mOculusVR->getEyeToScreenDistance() : 10, 10000.0f );
    mCamera.setEyePoint( Vec3f::zero() );
    mCamera.setWorldUp( Vec3f( 0, 1, 0 ) );
    
    // Make the stereo a bit stronger
    mCamera.setEyeSeparation( 1.5f );
    
    
}

void oculusBaseApp::keyDown(cinder::app::KeyEvent event){
    
    if( event.getChar() =='a' )
        mCamera.setEyeSeparation( mCamera.getEyeSeparation() - 0.1f );
    else if( event.getChar() =='s' )
        mCamera.setEyeSeparation( mCamera.getEyeSeparation() + 0.1f );
    else if( event.getChar() =='z' )
        mCamera.setProjectionCenterOffset( mCamera.getProjectionCenterOffset() - 1.1f );
    else if( event.getChar() =='x' )
        mCamera.setProjectionCenterOffset( mCamera.getProjectionCenterOffset() + 1.1f );
}

void oculusBaseApp::mouseDown( MouseEvent event )
{
}

void oculusBaseApp::update()
{
    //get oculus orientation and update the cameras
    Quatf orientation;
    
    if( mOculusVR ){
        orientation = mOculusVR->getOrientation();
    }
    
    mCamera.setOrientation( orientation * Quatf( Vec3f( 0, 1, 0 ), M_PI ) );
    
    
}

//Render is the new draw function
void oculusBaseApp::render(){
    static int i=0;
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::pushMatrices();
    gl::translate(Vec3f(0,0,500));
    gl::multModelView( Matrix44f::createRotation( Vec3f(0,1,0), toRadians((float)i++)) );
    gl::drawColorCube(Vec3f(0,0,0), Vec3f(100,100,100));
    gl::popMatrices();
    gl::disableDepthRead();
    gl::disableDepthWrite();
    
}

//in draw we will combine our eyes into one stereo image
void oculusBaseApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    // Start Rendering to Our Side by Side RenderTarget
    mFbo.bindFramebuffer();
    
    // Clear
    gl::clear( ColorA( 0.f, .0f, .0f, .0f ) );
    
    // Render Left Eye
    mCamera.enableStereoLeft();
    gl::setViewport( Area( Vec2f( 0.0f, 0.0f ), Vec2f( mFbo.getWidth() / 2.0f, mFbo.getHeight() ) ) );
    
    render();
    
    // Render Right Eye
    mCamera.enableStereoRight();
    gl::setViewport( Area( Vec2f( mFbo.getWidth() / 2.0f, 0.0f ), Vec2f( mFbo.getWidth(), mFbo.getHeight() ) ) );
    
    render();
    
    mFbo.unbindFramebuffer();
    
    
    // Back to 2d rendering
    gl::setMatricesWindow( getWindowSize(), false );
    gl::setViewport( getWindowBounds() );
    gl::disableDepthRead();
    gl::disableDepthWrite();
    
    // Send the Side by Side texture to our distortion correction shader
    mDistortionHelper->render( mFbo.getTexture(), getWindowBounds() );
    
    // Draw FPS
    gl::setMatricesWindow( getWindowSize() );
    gl::drawString( to_string( (int) getAverageFps() ), Vec2f( 10, 10 ) );
    
    
    
}

CINDER_APP_NATIVE( oculusBaseApp, RendererGl )
