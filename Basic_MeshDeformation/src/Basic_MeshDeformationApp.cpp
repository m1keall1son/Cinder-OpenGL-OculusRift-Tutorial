#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Camera.h"

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
    "gl_FragColor.a = .01;  \n"
    "}"
};

class Basic_MeshDeformationApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    gl::TextureRef mTexture;
    gl::GlslProgRef mGlsl;
    gl::VboMeshRef mMesh;
    
    CameraPersp mCam;
};

void Basic_MeshDeformationApp::setup()
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
    mMesh = gl::VboMesh::create( positions.size(), indices.size(), layout, GL_POINTS );
    
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
    
    mCam.setPerspective(45, getWindowAspectRatio(), .1, 10000);
    mCam.lookAt( Vec3f(0,150,600), Vec3f(0,0,0), Vec3f::yAxis() );
    
    
    //we only want to write depth, we don't want to depth test by reading it
    gl::enableDepthWrite();
    
    //enable the alpha channel
    gl::enableAdditiveBlending();

}

void Basic_MeshDeformationApp::mouseDown( MouseEvent event )
{
}

void Basic_MeshDeformationApp::update()
{
}

void Basic_MeshDeformationApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatrices(mCam);
    
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

}

CINDER_APP_NATIVE( Basic_MeshDeformationApp, RendererGl )
