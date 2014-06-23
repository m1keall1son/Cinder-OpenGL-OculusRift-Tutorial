
uniform sampler2D normalMap;

varying vec3 wcNormal;
varying vec3 wc3Position;
vec2 normalScale = vec2(.5);
const float LOG2 = 1.442695;

void PointLight(in int i,
                in vec3 eye,
                in vec3 wc3Position,
                in vec3 normal,
                inout vec4 ambient,
                inout vec4 diffuse,
                inout vec4 specular,
				inout float attenuation )
{
    float nDotVP;         // normal . light direction
    float nDotHV;         // normal . light half vector
    float pf;             // power factor
    float d;              // distance from surface to light source
    vec3  VP;             // direction from surface to light position
    vec3  halfVector;     // direction of maximum highlights
    
    // Compute vector from surface to light position
    VP = vec3( gl_LightSource[i].position ) - wc3Position;
    
    // Compute distance between surface and light position
    d = length( VP );
    
    // Normalize the vector from surface to light position
    VP = normalize( VP );
    
    // Compute attenuation
    attenuation = 1.0 / ( gl_LightSource[i].constantAttenuation +
                         gl_LightSource[i].linearAttenuation * d +
                         gl_LightSource[i].quadraticAttenuation * d * d );
    
    halfVector = normalize( VP + eye );
	
	nDotVP = max( 0.0, dot( normal, VP ) );
    nDotHV = max( 0.0, dot( normal, halfVector ) );
    
    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow( nDotVP, gl_FrontMaterial.shininess );
    
    ambient += gl_LightSource[i].ambient * attenuation;
    diffuse += gl_LightSource[i].diffuse * nDotVP * attenuation;
	specular += gl_LightSource[i].specular * pf * attenuation;
}

// Per-Pixel Tangent Space Normal Mapping
// http://hacksoflife.blogspot.ch/2009/11/per-pixel-tangent-space-normal-mapping.html

vec3 perturbNormal2Arb( vec3 eye_pos, vec3 surf_norm ) {
    
    vec3 q0 = dFdx( eye_pos.xyz );
    vec3 q1 = dFdy( eye_pos.xyz );
    vec2 st0 = dFdx( gl_TexCoord[0].st );
    vec2 st1 = dFdy( gl_TexCoord[0].st );
    
    vec3 S = normalize(  q0 * st1.t - q1 * st0.t );
    vec3 T = normalize( -q0 * st1.s + q1 * st0.s );
    vec3 N = normalize( surf_norm );
    
    vec3 mapN = texture2D( normalMap, gl_TexCoord[0].st ).xyz * 2.0 - 1.0;
    mapN.xy = normalScale * mapN.xy;
    mat3 tsn = mat3( S, T, N );
    return normalize( tsn * mapN );
    
}


void main()
{
    
	vec4 color = vec4(0.);
	vec3 eye = normalize( -wc3Position );
	
	// Clear the light intensity accumulators
	vec4 amb  = vec4( 0.0 );
	vec4 diff = vec4( 0.0 );
	vec4 spec = vec4( 0.0 );
	
	float ppDiffuse			= abs( dot( wcNormal, eye ) );
	float ppFresnell		= 1.0 - ppDiffuse;
	
	float atten = 0.0;
	
    vec3 newNormal = perturbNormal2Arb(eye, wcNormal);
    
    PointLight( 0, eye, wc3Position, normalize( wcNormal ), amb, diff, spec, atten );
	
	color += amb * gl_FrontMaterial.ambient + diff * gl_FrontMaterial.diffuse;
    
    color += spec * gl_FrontMaterial.specular;
	
	color += atten * gl_FrontMaterial.emission * ppFresnell * 1.0;
    
	color += pow( atten, 3.0 ) * gl_FrontMaterial.emission;
	
	gl_FragColor.rgb = color.rgb;
	gl_FragColor.a = 1.0;
}



