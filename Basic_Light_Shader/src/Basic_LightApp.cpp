#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Material.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Vbo.h"
#include "cinder/TriMesh.h"
#include "cinder/ObjLoader.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

//flat shaded vs and fs, for one light only!

static std::string vertex_flat = {
    "#version 120 \n "
    "varying vec4 vColor; \n"
    "void main(){  \n"
    " gl_TexCoord[0] = gl_MultiTexCoord0;  \n"
    " vec3 wcNormal = gl_NormalMatrix * gl_Normal;  \n"
    " vec4 wcPosition = gl_ModelViewMatrix * gl_Vertex;"
    " vec3 s = normalize(vec3(gl_LightSource[0].position - wcPosition)); \n"
    " gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n"
    " vColor =  gl_LightSource[0].ambient * gl_FrontMaterial.ambient;"
    " vColor += gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * max( dot( s , wcNormal ), 0.0 );"
    "}"
};

static std::string fragment_flat = {
    "#version 120  \n"
    "varying vec4 vColor; \n"
    "void main(){  \n"
    "gl_FragColor.rgb = vColor.rgb;  \n"
    "gl_FragColor.a = 1.0;  \n"
    "}"
};


//type def for eas of writing
typedef std::shared_ptr<gl::Light> LightRef;

class Basic_LightApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    CameraPersp mCamera;
    
    TriMesh mMesh;
    
    gl::Material mMaterial;
    
    LightRef mLight;
    
    gl::GlslProgRef mFlatShading;
    
    vector<Vec3f> rotations;
    vector<Vec3f> translations;

};

void Basic_LightApp::setup()
{
    //load the geometry
    ObjLoader loader = ObjLoader( loadAsset("../../primitives/cube.obj") );
    //fill the mesh with the data
    loader.load(&mMesh);
    
    //load lighting shader
    mFlatShading = gl::GlslProg::create( vertex_flat.c_str(), fragment_flat.c_str() );
    
    //set up camera
    mCamera.setPerspective(45, getWindowAspectRatio(), .1, 10000);
    mCamera.lookAt(Vec3f(5,5,15), Vec3f(0,0,0), Vec3f::yAxis() );
    
    //set material with ambient and diffuse and apply it
    mMaterial = gl::Material( ColorA(.1,.1,.1,1.), ColorA(1.,0.5,0.,1.) );
    
    //create a shared ptr to the light
    mLight = LightRef( new gl::Light( gl::Light::Type::POINT, 0) );
    mLight->enable();
    mLight->setDiffuse( Color( .9, .1, .1) );
    mLight->setSpecular( Color( 1., 1., 1.) );
    mLight->setAmbient( Color( .1, .1, .1) );
    mLight->setPosition( Vec3f( 0, 1, 1) );
    //mLight->setAttenuation( 2., 1., .5 );
    
    //enable depth
    gl::enableDepthWrite();
    gl::enableDepthRead();
    
    
    for(int i=0; i<20; i++){
        rotations.push_back( Vec3f(randFloat(360),randFloat(360),randFloat(360)) );
        translations.push_back( Vec3f( randFloat(-5.,5), randFloat(-5.,5), randFloat(-5.,5) ) );
    }

}

void Basic_LightApp::mouseDown( MouseEvent event )
{
}

void Basic_LightApp::update()
{
    //make the light go in a circle
    mLight->setPosition(Vec3f( 5 * cos( toRadians( (float)getElapsedFrames() ) ),
                               5 * cos( toRadians( (float)getElapsedFrames()*2 ) ),
                               5 * sin( toRadians( (float)getElapsedFrames() ) )
                              )
                        );
    mLight->update(mCamera);
}

void Basic_LightApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    gl::setMatrices( mCamera );
    
    gl::enable(GL_LIGHTING);
    mMaterial.apply();
    
    mFlatShading->bind();
    
    for(int i=0;i<20;i++){
        gl::pushMatrices();
        gl::rotate( rotations[i] );
        gl::translate( translations[i] );

        gl::draw( mMesh );
        gl::popMatrices();

    }
    
    mFlatShading->unbind();
    
    //light debug
    
    gl::disable(GL_LIGHTING);
    gl::drawSphere(mLight->getPosition(), .1);
    
    
}

CINDER_APP_NATIVE( Basic_LightApp, RendererGl )
