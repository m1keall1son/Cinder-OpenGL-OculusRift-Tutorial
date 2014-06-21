#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Material.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Vbo.h"
#include "cinder/TriMesh.h"
#include "cinder/ObjLoader.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

//type def for eas of writing
typedef std::shared_ptr<gl::Light> LightRef;

class Basic_LightApp : public AppNative {
public:
	void setup();
	void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event );
	void update();
	void draw();
    
    CameraPersp mCamera;
    
    TriMesh mMesh;
    
    gl::Material mMaterial;
    
    vector<Vec3f> rotations;
    vector<Vec3f> translations;
    
    Vec3f mLightPos;
    
    
};

void Basic_LightApp::setup()
{
    
    //set up camera
    mCamera.setPerspective(45, getWindowAspectRatio(), .1, 10000);
    mCamera.lookAt(Vec3f(5,5,15), Vec3f(0,0,0), Vec3f::yAxis() );
    
    //set material with ambient and diffuse and apply it
    mMaterial = gl::Material( ColorA(.3,.2,.0,1.), ColorA(1.,0.5,0.,1.), ColorA(1.,1.,1.,1.), 10., ColorA( 0,0,0,0) );
    
    //enable depth
    gl::enableDepthWrite();
    gl::enableDepthRead();
    
    
    for(int i=0; i<20; i++){
        rotations.push_back( Vec3f(randFloat(360),randFloat(360),randFloat(360)) );
        translations.push_back( Vec3f( randFloat(-5.,5), randFloat(-5.,5), randFloat(-5.,5) ) );
    }
    
}

void Basic_LightApp::keyDown(cinder::app::KeyEvent event){
    if(event.getChar() == 's'){
        glShadeModel(GL_SMOOTH);
    }
    if (event.getChar() == 'f') {
        glShadeModel(GL_FLAT);
    }
}

void Basic_LightApp::mouseDown( MouseEvent event )
{
}

void Basic_LightApp::update()
{
    //make the light go in a circle
    mLightPos = Vec3f( 5 * cos( toRadians( (float)getElapsedFrames() ) ),
                      5 * cos( toRadians( (float)getElapsedFrames()*2 ) ),
                      5 * sin( toRadians( (float)getElapsedFrames() ) )
                     );
}

void Basic_LightApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    gl::setMatrices( mCamera );
    
    gl::enable(GL_LIGHTING);
    
    //make a light!
    glEnable(GL_LIGHT0);
    //openGL likes its own datatypes
    GLfloat light_position[] = { mLightPos.x, mLightPos.y, mLightPos.z, 1. };
    glLightfv( GL_LIGHT0, GL_POSITION, light_position );

    mMaterial.apply();
    
    for(int i=0;i<20;i++){
        gl::pushMatrices();
        gl::rotate( rotations[i] );
        gl::translate( translations[i] );
        
        gl::drawSphere(Vec3f(0,0,0), .5);
        gl::popMatrices();
        
    }
    
    //light debug
    gl::disable(GL_LIGHTING);
    gl::drawSphere(mLightPos, .1);
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    gl::drawString("press 's' for smooth shading", Vec2f(10,getWindowHeight()-30) );
    gl::drawString("press 'f' for flat shading", Vec2f(10,getWindowHeight()-10) );
    
    
    
}

CINDER_APP_NATIVE( Basic_LightApp, RendererGl )
