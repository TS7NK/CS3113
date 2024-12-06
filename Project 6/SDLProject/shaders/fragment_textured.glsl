// fragment_lit.glsl
uniform sampler2D diffuse;
uniform vec2 lightPosition;

varying vec2 texCoordVar;
varying vec2 varPosition;
uniform int onlyLevel_D;

float attenuate(float dist, float a, float b)
{
     return 1.0 / (1.0 + (a * dist) + (b * dist  * dist));
}

void main()
{
    if(onlyLevel_D == 1){
        float brightness = attenuate(distance(lightPosition, varPosition), 0.5, 0.1);
        vec4 color = texture2D(diffuse, texCoordVar);
        gl_FragColor = vec4(color.rgb * brightness, color.a);
    } else
    {
        gl_FragColor = texture2D(diffuse, texCoordVar);
    }
    
    // Lighting effect
    // float brightness = attenuate(distance(lightPosition, varPosition), 0.2, 0.01);
    // Normal
//    float brightness = attenuate(distance(lightPosition, varPosition), 0.01, 0.0);
//
//    vec4 color = texture2D(diffuse, texCoordVar);
//    gl_FragColor = vec4(color.rgb * brightness, color.a);
}
