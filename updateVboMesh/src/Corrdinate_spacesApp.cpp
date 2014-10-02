#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/TriMesh.h"
#include "cinder/Camera.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Perlin.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Corrdinate_spacesApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    TriMesh mMesh;
    
    gl::VboMeshRef mVboMesh;
    
    CameraPersp mCam;
    

};

void Corrdinate_spacesApp::setup()
{
    vector<Vec3f> positions;
    vector<uint32_t> indices;
    
    positions.push_back( Vec3f( -1,  1, 0) ); //0
    positions.push_back( Vec3f( -1, -1, 0) ); //1
    positions.push_back( Vec3f(  1, -1, 0) ); //2
    positions.push_back( Vec3f(  1,  1, 0) ); //3
    
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(0);
    
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

//    mMesh.appendVertices(positions.data(), positions.size());
//    mMesh.appendIndices(indices.data(), indices.size());
    
    gl::VboMesh::Layout layout;
    layout.setStaticPositions();
    layout.setStaticIndices();
    
    mVboMesh = gl::VboMesh::create( 4, 6, layout, GL_TRIANGLES );
    
    mVboMesh->bufferPositions(positions);
    mVboMesh->bufferIndices(indices);
    
    mVboMesh->unbindBuffers();
    
    mCam.setPerspective(60, getWindowAspectRatio(), .1, 10000);
    mCam.lookAt( Vec3f(0,0,5), Vec3f(0,0,0), Vec3f::yAxis() );
    
    
}

void Corrdinate_spacesApp::mouseDown( MouseEvent event )
{
}

void Corrdinate_spacesApp::update()
{
    vector<Vec3f> positions;
    positions.resize(4);
    for( int i=0;i<positions.size();i++ ){
        
        Perlin p;
        positions[i] = 3. * ( p.dnoise( getElapsedSeconds()+i,
                                       getElapsedSeconds()+i*2,
                                       getElapsedSeconds() ) * 2 - Vec3f(1.,1.,1.) );
        
    }
    
    mVboMesh->bufferPositions(positions);
    mVboMesh->unbindBuffers();
}

void Corrdinate_spacesApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatrices(mCam);
    gl::draw(mVboMesh);
    
}

CINDER_APP_NATIVE( Corrdinate_spacesApp, RendererGl )
