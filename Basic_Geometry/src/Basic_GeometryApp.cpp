#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/TriMesh.h"
#include "cinder/gl/Vbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Basic_GeometryApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    gl::VboMeshRef mMesh;
    
};

void Basic_GeometryApp::setup()
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
    
    mMesh = gl::VboMesh::create( m );

    
}

void Basic_GeometryApp::mouseDown( MouseEvent event )
{
}

void Basic_GeometryApp::update()
{
}

void Basic_GeometryApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::draw( mMesh );
}

CINDER_APP_NATIVE( Basic_GeometryApp, RendererGl )
