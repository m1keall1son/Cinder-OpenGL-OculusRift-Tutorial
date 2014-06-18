#version 120
uniform sampler2D heightMap;
uniform float heightScale; 
varying vec4 wcPosition; 
varying vec3 wc3Position; 
varying vec3 wcNormal;

void main(){
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    wcNormal = gl_NormalMatrix * gl_Normal;
    vec4 pos = vec4(gl_Vertex.x, gl_Vertex.y + (heightMap.r * heightScale), gl_Vertex.z, gl_Vertex.w);
    wcPosition = gl_ModelViewMatrix * pos;
    wc3Position = vec3(wcPosition) / wcPosition.w;
    gl_Position = gl_ModelViewProjectionMatrix * pos;
    
}