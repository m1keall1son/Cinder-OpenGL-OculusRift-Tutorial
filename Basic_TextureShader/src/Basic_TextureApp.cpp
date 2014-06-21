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
static string vertex = {
    "#version 120  \n"
    "void main() {   \n"
    "gl_TexCoord[0] = gl_MultiTexCoord0;  \n"
    "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n"
    "}"
};

//we are going to use the texture in the shader by passing it as a uniform and using our texture coordinates as a lookup number to find out what color to make the fragment.
static string fragment = {
    "#version 120  \n"
    "uniform sampler2D tex; \n"
    "void main() {  \n"
    //sample the texture at the texcoord by using the built in GLSL function
    "gl_FragColor = texture2D( tex, gl_TexCoord[0].st );  \n"
    "}"
};

class Basic_TextureShaderApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    gl::TextureRef mTexture;
    gl::GlslProgRef mGlsl;
    gl::VboMeshRef mMesh;
    
    
};

void Basic_TextureShaderApp::setup()
{
    //build a high-res mesh to draw the texture to, later we can manipulate these vertices with the shader
    vector<Vec3f> positions;
    vector<Vec2f> texCoords;
    vector<uint32_t> indices;
    
    int width = 320;
    int height = 240;
    
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            
            positions.push_back( Vec3f(x*2,y*2,0) );
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
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void Basic_TextureShaderApp::mouseDown( MouseEvent event )
{
}

void Basic_TextureShaderApp::update()
{
}

void Basic_TextureShaderApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    //bind the texture we want to use at an attachement point 0-16
    mTexture->bind(0);
    
    mGlsl->bind();
    
    //pass the texture as a uniform jsut by specificying which attachement to send
    mGlsl->uniform("tex", 0);
    
    gl::draw( mMesh );
    
    mGlsl->unbind();
    mTexture->unbind();
}

CINDER_APP_NATIVE( Basic_TextureShaderApp, RendererGl )
