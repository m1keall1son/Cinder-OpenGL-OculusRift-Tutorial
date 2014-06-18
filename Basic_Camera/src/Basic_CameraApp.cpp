#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Basic_CameraApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    CameraPersp mPerspCam;
    CameraOrtho mOrthoCam;
    bool bUsePersp;
    
};

void Basic_CameraApp::setup()
{
    
    //setup the camera's perspective with an Fov and aspect ratio
    //the last 2 args are the near clipping and far clipping planes
    mPerspCam.setPerspective(45, getWindowAspectRatio(), .1, 100000);
    
    //set the eyepoint, the center of interest and the up direction
    mPerspCam.lookAt(Vec3f(0,0,100), Vec3f(0,0,0), Vec3f::yAxis() );

    
    //set the ortho bounds (Left, Right, Down, Up) and the near and far clipping planes
    mOrthoCam.setOrtho( -50, 50, -50, 50, .1, 100000 );
    //set the eyepoint, the center of interest and the up direction
    mOrthoCam.lookAt(Vec3f(0,0,100), Vec3f(0,0,0), Vec3f::yAxis() );
    
    
    bUsePersp = false;
    
    //since we are now drawing 3D we must enable depth
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void Basic_CameraApp::mouseDown( MouseEvent event )
{
    bUsePersp = !bUsePersp;
}

void Basic_CameraApp::update()
{
}

void Basic_CameraApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    if(bUsePersp){
        gl::setMatrices( mPerspCam );
    }else{
        gl::setMatrices( mOrthoCam );
    }
    
    gl::pushMatrices();
    //cause the cube to rotate on all axies
    gl::multModelView( Matrix44f::createRotation(Vec3f(1,1,1), toRadians( (float)getElapsedFrames() ) ) );
    
    gl::drawColorCube(Vec3f(0,0,0), Vec3f(25,25,25) );
    
    gl::popMatrices();
    
    //draw 2D things without the 3D camera
    //set new matrices to the viewport of the window
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    if(bUsePersp){
        gl::drawString("Perspective Camera", Vec2i(10,10) );
    }else{
        gl::drawString("Orthogonal Camera", Vec2i(10,10) );
    }
    gl::drawStringRight("'mouse click' to toggle", Vec2i( getWindowWidth()-10, getWindowHeight()-10) );

    
}

CINDER_APP_NATIVE( Basic_CameraApp, RendererGl )
