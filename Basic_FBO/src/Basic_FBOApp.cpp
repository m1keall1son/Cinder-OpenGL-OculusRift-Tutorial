#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Basic_FBOApp : public AppNative {
  public:
	void setup();
	void mouseDrag( MouseEvent event );
	void update();
	void draw();
    
    gl::Fbo mFbo;
    Vec2f mMousePos;
    
    bool bDrawInFBO;
    
};

void Basic_FBOApp::setup()
{
    //an FBO is like another screen, it stands for Framebuffer Object, which means it is a special type of object that contains texture attachements and can be drawn into
    //to create one, we need to give it a size and an optional format
    //we will make it with a default format
    
    gl::Fbo::Format fmt;
    fmt.setColorInternalFormat(GL_RGBA8);
    fmt.setSamples(8);
    fmt.enableDepthBuffer();
    
    mFbo = gl::Fbo( getWindowWidth()/2, getWindowHeight(), fmt);
    
    //FBOs will be allocated and the pixels will be the color of whatever was on the GPU, so we need to clean it before using it
    mFbo.bindFramebuffer();
    gl::clear();
    mFbo.unbindFramebuffer();
    
    //initialize variables
    mMousePos = Vec2f::zero();
    bDrawInFBO = false;
    
    
}

void Basic_FBOApp::mouseDrag( MouseEvent event )
{
    mMousePos = event.getPos();
    
    if( mMousePos.x < getWindowWidth()/2 ){
        bDrawInFBO = true;
    }else{
        bDrawInFBO = false;
    }

}

void Basic_FBOApp::update()
{
}

void Basic_FBOApp::draw()
{
	// clear out the window with black every frame
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
    
    gl::color(1., 1., 1.);
    
    if( !bDrawInFBO ){
        //we are not in the FBO
        gl::drawSolidCircle( mMousePos, 10 );
        
    }else{
        //draw into the FBO
        mFbo.bindFramebuffer();
        //don't clear the background so we accumulate colors
        gl::drawSolidCircle( mMousePos, 10 );
        
        mFbo.unbindFramebuffer();
        
        
    }

    //to draw what's in the FBO just get the texture, but we need to flip it over
    gl::Texture tex = mFbo.getTexture();
    tex.setFlipped();
    gl::draw( tex );
    
    //debug line and text
    gl::color( 1., 0., 0. );
    gl::drawLine( Vec2f(getWindowWidth()/2, 0), Vec2f(getWindowWidth()/2, getWindowHeight()) );
    gl::drawString("User FBO - no clearing", Vec2i(10,10) );
    gl::drawStringRight("Screen FBO - clearing to black", Vec2i(getWindowWidth()-10,10) );


    
}

CINDER_APP_NATIVE( Basic_FBOApp, RendererGl )
