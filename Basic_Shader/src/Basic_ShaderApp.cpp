#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/TriMesh.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;
using namespace std;


static string vertex = {
    "#version 120  \n"
    "void main() {   \n"
        "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n"
    "}"
};

static string fragment = {
    "#version 120  \n"
    "void main() {  \n"
        "gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );  \n"
    "}"
};


class Basic_ShaderApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    gl::VboMeshRef mMesh;
    gl::GlslProgRef mGlsl;
    
};

void Basic_ShaderApp::setup()
{
    
    float startX = getWindowWidth()/4;
    float width = startX*3;
    float startY = getWindowHeight()/4;
    float height = startY*3;
    
    
    vector<Vec3f> points;
    vector<uint32_t> indices;
    
    points.push_back( Vec3f( startX, startY, 0) );
    points.push_back( Vec3f( startX, height, 0) );
    points.push_back( Vec3f( width, height, 0) );
    points.push_back( Vec3f( width, startY, 0) );
    
    indices.push_back( 0 );  // everything is made of triangles, even rectangels, three indexes per triangle
    indices.push_back( 1 );  // 0 __   3
    indices.push_back( 3 );  //  |  //|
    indices.push_back( 1 );  //  | // |
    indices.push_back( 2 );  //  |//__|
    indices.push_back( 3 );  // 1      2
    
    TriMesh m;
    m.appendVertices( points.data(), points.size() );
    m.appendIndices( indices.data(), indices.size() );
    
    //load vbo with geometry
    mMesh = gl::VboMesh::create( m );
    
    //load shaders
    //pass in the shader programs as Char* strings
    mGlsl = gl::GlslProg::create( vertex.c_str(), fragment.c_str() );
    
    
}

void Basic_ShaderApp::mouseDown( MouseEvent event )
{
}

void Basic_ShaderApp::update()
{
}

void Basic_ShaderApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    //bind the shader to the gl::draw call and Cinder will draw the geometry using your shader
    mGlsl->bind();
    
    gl::draw( mMesh );
    
    mGlsl->unbind();
    
}

CINDER_APP_NATIVE( Basic_ShaderApp, RendererGl )





