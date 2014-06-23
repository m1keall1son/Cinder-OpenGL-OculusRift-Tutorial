#version 120
uniform sampler2D heightMap;
uniform float heightScale; 
varying vec4 wcPosition; 
varying vec3 wc3Position; 
varying vec3 wcNormal;

void main(){
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    
    float height = texture2D(heightMap, gl_TexCoord[0].st).r * heightScale;
    
    vec3 norm = gl_Normal;
    norm.y += height;
    wcNormal = gl_NormalMatrix * normalize(norm);
    
    vec4 pos = vec4(gl_Vertex.x, gl_Vertex.y + height, gl_Vertex.z, gl_Vertex.w);
    wcPosition = gl_ModelViewMatrix * pos;
    
    wc3Position = vec3(wcPosition);
    
    gl_Position = gl_ModelViewProjectionMatrix * pos;
    
}