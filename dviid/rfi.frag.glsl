#version 120

uniform int           algo;
uniform sampler2DRect tex0;

void main() {

    if(algo == 4) {
        vec4 color = texture2DRect(tex0, gl_TexCoord[0].st);
        float luminance = 0.3f * color.x + 0.59 * color.y + 0.11 * color.z;
        //gl_FragColor = vec4(luminance, luminance, luminance, 1.0f);
        gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 0.5f);   
        return;     
    }

    gl_FragColor = texture2DRect(tex0, gl_TexCoord[0].st);

}
