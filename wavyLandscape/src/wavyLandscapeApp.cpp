#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"

#include "OculusVR.h"
#include "CameraStereoHMD.h"

using namespace ci;
using namespace ci::app;
using namespace std;

//here we assign the built-in texcoord variable to access the texCoords we passed up to the GPU in texCoord position 0
//we also apply some math to the position of the verticies by passing in a time uniform
static string vertex = {
    "#version 120  \n"
    "uniform float time;  \n"
    "void main() {   \n"
        "gl_TexCoord[0] = gl_MultiTexCoord0;  \n"
        "vec4 position = gl_Vertex;  \n"
        "position.y += 40.0 * sin( time*gl_TexCoord[0].s ) * sin( time*gl_TexCoord[0].t ) ;  \n"
        "gl_Position = gl_ModelViewProjectionMatrix * position;  \n"
    "}"
};

//we are going to use the texture in the shader by passing it as a uniform and using our texture coordinates as a lookup number to find out what color to make the fragment.
static string fragment = {
    "#version 120  \n"
    "uniform sampler2D tex; \n"
    "void main() {  \n"
    //sample the texture at the texcoord by using the built in GLSL function
    "gl_FragColor = texture2D( tex, gl_TexCoord[0].st );  \n"
    //add some alpha
    "gl_FragColor.a = .5;  \n"
    "}"
};

class wavyLandscapeApp : public AppNative {
  public:
	void setup();
    void prepareSettings( Settings* settings );
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    void render();
    
    //declare that we will have an oculus attached
    ovr::DeviceRef mOculusVR;
    //declare the distortion helper and stereo camera
    ovr::DistortionHelperRef mDistortionHelper;
    CameraStereoHMD mCamera;
    
    //the FBO we will render the 3D scene to through th stereo camera
    gl::Fbo mFbo;
    
    //the actual 3D stuff we will use
    gl::TextureRef mTexture;
    gl::GlslProgRef mGlsl;
    gl::VboMeshRef mMesh;
    
    
};

void wavyLandscapeApp::prepareSettings(cinder::app::AppBasic::Settings *settings){
    //set the window to full screen
    settings->setFullScreen();
}

void wavyLandscapeApp::setup()
{
    
    //build a high-res mesh to draw the texture to, later we can manipulate these vertices with the shader
    vector<Vec3f> positions;
    vector<Vec2f> texCoords;
    vector<uint32_t> indices;
    
    int width = 512;
    int height = 512;
    
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            
            //position these to make a flat plane that is centered around 0
            positions.push_back( Vec3f( x - (width / 2) , 0, y - (height / 2 ) ) );
            //Texcoords are normalized from 0-1, so divide the positions by the max. Cast the ints as floats to get a more precise division.
            texCoords.push_back( Vec2f( (float)x / ( (float)width-1.), (float)y / ( (float)height-1. ) ) );
            //add the triangles, except for edge cases
            if(x < width-1 && y < height - 1){
                indices.push_back(  x    +  y    * width );
                indices.push_back(  x    + (y+1) * width );
                indices.push_back( (x+1) + (y+1) * width );
                indices.push_back(  x    +  y    * width );
                indices.push_back( (x+1) + (y+1) * width );
                indices.push_back( (x+1) +  y    * width );
                
            }
        }
    }
    
    //create a layout that will include the types of data we are sending to the GPU
    
    gl::VboMesh::Layout layout;
    layout.setStaticIndices();
    layout.setStaticPositions();
    layout.setStaticTexCoords2d();
    
    //create the mesh with the right number of verticies and indicies, set the layout and tell it how th draw
    mMesh = gl::VboMesh::create( positions.size(), indices.size(), layout, GL_TRIANGLES );
    
    //add the data to the mesh
    mMesh->bufferPositions( positions );
    mMesh->bufferIndices( indices );
    mMesh->bufferTexCoords2d( 0, texCoords );
    
    //unbind the mesh to clean up
    mMesh->unbindBuffers();
    
    //load the image from the assets folder
    mTexture = gl::Texture::create( loadImage( loadAsset("rainbow.png") ) );
    
    //load the GLSL
    mGlsl = gl::GlslProg::create( vertex.c_str(), fragment.c_str() );
    
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
    mCamera.setEyePoint( Vec3f(0,50,0) );
    mCamera.setWorldUp( Vec3f( 0, 1, 0 ) );
    
    // Make the stereo a bit stronger
    mCamera.setEyeSeparation( 1.5f );

}

void wavyLandscapeApp::mouseDown( MouseEvent event )
{
}

void wavyLandscapeApp::update()
{
    //get oculus orientation and update the cameras
    Quatf orientation;
    
    if( mOculusVR ){
        orientation = mOculusVR->getOrientation();
    }
    
    mCamera.setOrientation( orientation * Quatf( Vec3f( 0, 1, 0 ), M_PI ) );
}

void wavyLandscapeApp::render(){
    
    //set matrices to the currently enabled eye
    gl::setMatrices( mCamera );

    //set our alpha and depth settings
    gl::pushMatrices();
    gl::enableAdditiveBlending();
    gl::enableDepthWrite();
    
    //bind the texture we want to use at an attachement point 0-16
    mTexture->bind(0);
    
    mGlsl->bind();
    
    //pass the texture as a uniform jsut by specificying which attachement to send
    mGlsl->uniform("tex", 0);
    //send up the elapsed time to the shader, be explict about casting the time as a float since the getElapsedSeconds returns a double
    mGlsl->uniform("time", (float)getElapsedSeconds() );
    gl::draw( mMesh );
    
    mGlsl->unbind();
    mTexture->unbind();
    
    //clean up after ourselves
    gl::disableAlphaBlending();
    gl::disableDepthWrite();
    
    gl::popMatrices();

}

void wavyLandscapeApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
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
    
    // Send the Side by Side texture to our lens distortion correction shader
    mDistortionHelper->render( mFbo.getTexture(), getWindowBounds() );
    
    // Draw FPS
    gl::setMatricesWindow( getWindowSize() );
    gl::drawString( to_string( (int) getAverageFps() ), Vec2f( 10, 10 ) );
    
}

CINDER_APP_NATIVE( wavyLandscapeApp, RendererGl )
