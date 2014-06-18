#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/TriMesh.h"
#include "cinder/params/Params.h"
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
    //we will use the uniform variable in the frag shader to se the color
    "uniform vec4 color;  \n"
    "void main() {  \n"
    "gl_FragColor = color;  \n"
    "}"
};


class Basic_ParamsApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    
    gl::VboMeshRef mMesh;
    gl::GlslProgRef mGlsl;
    
    ColorA mColor;
    params::InterfaceGlRef mGui;
    
};

void Basic_ParamsApp::setup()
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
    
    //initialize variables
    mColor = ColorA(1.,0.,0.,1.);
    
    //create the GUI
    mGui = params::InterfaceGl::create( "Set Color", Vec2f(200,130) );
    mGui->addParam("Color", &mColor );

}

void Basic_ParamsApp::mouseDown( MouseEvent event )
{
}

void Basic_ParamsApp::update()
{
}

void Basic_ParamsApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    //bind the shader to the gl::draw call and Cinder will draw the geometry using your shader
    mGlsl->bind();
    //this time send a uniform that is accessible by your shder program to determine the color
    //dont forget that we had to update the GLSL at the top to accomidate a uniform variable!
    mGlsl->uniform("color", mColor );
    
    gl::draw( mMesh );
    
    mGlsl->unbind();
    
    //lastly draw the GUI
    mGui->draw();
}

CINDER_APP_NATIVE( Basic_ParamsApp, RendererGl )
