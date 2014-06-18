#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/TriMesh.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class FoggedTerrainApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    gl::VboMeshRef mMesh;
    gl::GlslProgRef terrainFogShader;
    
    
    
};

void FoggedTerrainApp::setup()
{
}

void FoggedTerrainApp::mouseDown( MouseEvent event )
{
}

void FoggedTerrainApp::update()
{
}

void FoggedTerrainApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( FoggedTerrainApp, RendererGl )
