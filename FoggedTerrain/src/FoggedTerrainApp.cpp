#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/TriMesh.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Material.h"


using namespace ci;
using namespace ci::app;
using namespace std;


typedef std::shared_ptr<gl::Light> LightRef;

class FoggedTerrainApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );
    void prepareSettings( Settings* settings);
	void update();
	void draw();
    
    void buildMesh();
    
    gl::VboMeshRef mMesh;
    gl::GlslProgRef mTerrainFogShader;
    gl::TextureRef mHeightMap, mNormalMap;
    
    CameraPersp mCam;
    
    params::InterfaceGlRef mGui;
    
    float mHeightScale;
    
    gl::Material mMaterial;
    LightRef mLight;
    
    Quatf mRotation;
    
};

void FoggedTerrainApp::prepareSettings(cinder::app::AppBasic::Settings *settings){
    settings->setWindowSize(1024, 768);
}

void FoggedTerrainApp::setup()
{
    
    mHeightMap = gl::Texture::create( loadImage( loadAsset("noiseField_height.jpg") ) );
    mNormalMap = gl::Texture::create( loadImage( loadAsset("noiseField_NRM.png") ) );
    
    mTerrainFogShader = gl::GlslProg::create( loadAsset("vertex_blinPhong_height.glsl"),
                                              loadAsset("fragment_blinPhong_height.glsl")
                                             );
    
    
    mCam.setPerspective(50, getWindowAspectRatio(), .1, 100000);
    mCam.lookAt( Vec3f(0,50,500), Vec3f(0,0,0), Vec3f::yAxis() );
    
    buildMesh();
    
    mHeightScale = 20.f;
    
    mGui = params::InterfaceGl::create( "Parameters" , Vec2f(200,500) );
    mGui->addParam("Height Scale", &mHeightScale );
    mGui->addParam("Rotate Scene", &mRotation );
    
    mLight = LightRef( new gl::Light( gl::Light::Type::POINT, 0 ) );
    mLight->setPosition(Vec3f(5,5,5));
    mLight->setAmbient(Color(.1,.1,.1));
    mLight->setDiffuse(Color(1.,.7,.1));
    mLight->setSpecular(Color( 1.,1.,1. ));
    mLight->setAttenuation(1.,1., 1.);
    
    mMaterial = gl::Material( ColorA(.1,.1,.1,1.), ColorA(1.,.5,0.), ColorA(1.,1.,1.,1.), 50. );
 
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void FoggedTerrainApp::mouseDown( MouseEvent event )
{
}

void FoggedTerrainApp::buildMesh(){
    //build a high-res mesh to draw the texture to, later we can manipulate these vertices with the shader
    vector<Vec3f> positions;
    vector<Vec2f> texCoords;
    vector<Vec3f> normals;
    vector<uint32_t> indices;
    
    int width = 1024;
    int height = 1024;
    
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
            
            Vec3f normal = cross(Vec3f(x, y+1,0.f)-Vec3f(x, y,0.f), Vec3f(x+1, y,0.f)-Vec3f(x, y,0.f)).normalized();
            normals.push_back(normal);
            
        }
    }
    
    //create a layout that will include the types of data we are sending to the GPU
    
    gl::VboMesh::Layout layout;
    layout.setStaticIndices();
    layout.setStaticPositions();
    layout.setStaticTexCoords2d();
 //   layout.setStaticNormals();
    
    //create the mesh with the right number of verticies and indicies, set the layout and tell it how th draw
    mMesh = gl::VboMesh::create( positions.size(), indices.size(), layout, GL_TRIANGLES );
    
    //add the data to the mesh
    mMesh->bufferPositions( positions );
   // mMesh->bufferNormals( normals );
    mMesh->bufferIndices( indices );
    mMesh->bufferTexCoords2d( 0, texCoords );
    
    //unbind the mesh to clean up
    mMesh->unbindBuffers();

}

void FoggedTerrainApp::update()
{
    mLight->setPosition(Vec3f(500*cos((float)getElapsedFrames()/100), 25, 0));
    
}

void FoggedTerrainApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0.5, 0, 0 ) );
    
    gl::setMatrices( mCam );
    gl::rotate(mRotation);
    
    mMaterial.apply();
    
    mNormalMap->bind(0);
    mHeightMap->bind(1);
    
    mTerrainFogShader->bind();
    mTerrainFogShader->uniform("normalMap", 0);
    mTerrainFogShader->uniform("heightMap", 1);
    mTerrainFogShader->uniform("heightScale", mHeightScale );
    gl::draw(mMesh);
    gl::drawSphere(mLight->getPosition(), .1);

    mTerrainFogShader->unbind();
    
    mNormalMap->unbind();
    mHeightMap->unbind();
    
    
    
    mGui->draw();
    
}

CINDER_APP_NATIVE( FoggedTerrainApp, RendererGl )
