#version 120
#extension GL_EXT_geometry_shader4 : enable

#define ALGO_1  1
#define ALGO_2  2
#define ALGO_3  3
#define ALGO_4  4

uniform int             algo;
uniform float           scale;
uniform sampler2DRect   tex0;

float max_color(vec3 rgb)
{
    float max = rgb.r;
    if(rgb.g > max) { max = rgb.g; }
    if(rgb.b > max) { max = rgb.b; }  
    return max;
} 

float min_color(vec3 rgb)
{
    float min = rgb.r;
    if(rgb.g < min) { min = rgb.g; }
    if(rgb.b < min) { min = rgb.b; }  
    return min;
} 

void hue(in vec4 color, out float h)
{
	float max = max_color(color.rgb);	
	float min = min_color(color.rgb);
    if(max == min) { h = 0.f; }
    
	float hueSixth;
	if(color.r == max) {
		hueSixth = (color.g - color.b) / (max - min);
		if(hueSixth < 0.f)
			hueSixth += 6.f;
	} else if (color.g == max) {
		hueSixth = 2.f + (color.b - color.r) / (max - min);
	} else {
		hueSixth = 4.f + (color.r - color.g) / (max - min);
	}
    
	h = 255.f * hueSixth / 6.f;    
}

void saturation(in vec4 color, out float s)
{
	float max = max_color(color.rgb);	
	float min = min_color(color.rgb);
    if(max == min) { s = 0.f; }
    
    s = 255.f * (max - min) / max;
}

void brightness(in vec4 color, out float b)
{
    b = max_color(color.rgb);
}

void toHSB(in vec3 rgb, out vec3 hsb) 
{
	float max = max_color(rgb);	
	float min = min_color(rgb);
	
	if(max == min) {
		hsb.x = 0.f;
		hsb.y = 0.f;
		hsb.z = 255.f * max;
		return;
	}
	
	float hueSixth;
	if(rgb.r == max) {
		hueSixth = (rgb.g - rgb.b) / (max - min);
		if(hueSixth < 0.f)
			hueSixth += 6.f;
	} else if (rgb.g == max) {
		hueSixth = 2.f + (rgb.b - rgb.r) / (max - min);
	} else {
		hueSixth = 4.f + (rgb.r - rgb.g) / (max - min);
	}
	hsb.x = 255.f * hueSixth / 6.f;
	hsb.y = 255.f * (max - min) / max;
	hsb.z = max;
}

void main()
{

    if(algo == ALGO_4 && mod(gl_PositionIn[0].y, 2.f) != 0) return;

	for (int i = 0; i < gl_VerticesIn; i++)
	{
 
		gl_Position = gl_PositionIn[i];
        
        vec4 color0 = texture2DRect(tex0, gl_TexCoordIn[i][0].st);
        
        float depth = 0.f;
        
        if(algo == ALGO_1) {
            brightness(color0, depth);
        } 
        else if(algo == ALGO_2) {
            hue(color0, depth);
        }
        else if(algo == ALGO_3) {
            saturation(color0, depth);
        }
        else if(algo == ALGO_4) {
            brightness(color0, depth);
        }
        else {
            brightness(color0, depth);
        }                
                
        gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Position.x, gl_Position.y, depth * scale, 1.0);
        gl_TexCoord[0] = gl_TexCoordIn[i][0];
                
		EmitVertex();        
	}    
    
    
}
