/*
 ~ copyright (c) 2011 dviid
 ~ contact: dviid@labs.ciid.dk 
 
 + redistribution and use in source and binary forms, with or without
 + modification, are permitted provided that the following conditions
 + are met:
 +  > redistributions of source code must retain the above copyright
 +    notice, this list of conditions and the following disclaimer.
 +  > redistributions in binary form must reproduce the above copyright
 +    notice, this list of conditions and the following disclaimer in
 +    the documentation and/or other materials provided with the
 +    distribution.
 
 + THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 + "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 + LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 + FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 + COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 + INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 + BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 + OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 + AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 + OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 + OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 + SUCH DAMAGE.
 
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

const sampler_t smp = CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void sobel(read_only image2d_t src, write_only image2d_t dst)
{
    int2 coords = (int2) (get_global_id(0), get_global_id(1));
    int2 dx = (int2)(1,0);
    int2 dy = (int2)(0,1);
    float Gx, Gy, G, theta, p;
    
    if(coords.x > get_image_width(dst) || coords.y > get_image_height(dst))
        return;
    
    p = read_imagef(src, smp, coords - dx + dy).s0;
    Gx = p;
    Gy = p;
    
    p = read_imagef(src, smp, coords + dy).s0;
    Gy += 2 * p;

    p = read_imagef(src, smp, coords + dx + dy).s0;
    Gx -= p;
    Gy += p;
    
    p = read_imagef(src, smp, coords - dx).s0;
    Gx += 2 * p;
    
    p = read_imagef(src, smp, coords + dx).s0;
    Gx -= 2 * p;

    p = read_imagef(src, smp, coords - dx - dy).s0;
    Gx += p;
    Gy -= p;
    
    p = read_imagef(src, smp, coords - dy).s0;
    Gy -= 2 * p;
    
    p = read_imagef(src, smp, coords + dx - dy).s0;
    Gx -= p;
    Gy -= p;
    
    G = sqrt(Gx * Gx + Gy * Gy);
    theta = atan(Gx / Gy);
    
    write_imagef(dst, coords, G);                
    
}

__kernel void hgauss(read_only image2d_t src, write_only image2d_t dst, 
                     global read_only float* weights, global float* offsets, const int nbr_weights)
{
    int2    coords = (int2) (get_global_id(0), get_global_id(1));
    float2  src_coords = (float2) (get_global_id(0), get_global_id(1));
    float4  pix = (float4)(0,0,0,0);

    int i;
    for(i = 0; i < nbr_weights; i++) {
        pix += read_imagef(src, smp, src_coords + (float2) (offsets[i], 0.0f)) * weights[i];
    }
    
    write_imagef(dst, coords, clamp(pix, 0.0f, 1.0f));
}

__kernel void vgauss(read_only image2d_t src, write_only image2d_t dst, 
                     global read_only float* weights, global float* offsets, const int nbr_weights)
{
    int2    coords = (int2) (get_global_id(0), get_global_id(1));
    float2  src_coords = (float2) (get_global_id(0), get_global_id(1));
    float4  pix = (float4)(0,0,0,0);
    
    int i;
    for(i = 0; i < nbr_weights; i++) {
        pix += read_imagef(src, smp, src_coords + (float2) (0.0f, offsets[i])) * weights[i];
    }
    
    write_imagef(dst, coords, clamp(pix, 0.0f, 1.0f));
}



__kernel void grey(read_only image2d_t src, write_only image2d_t dst)
{
    int2 coords = (int2) (get_global_id(0), get_global_id(1));
    float4 color = read_imagef(src, smp, coords);
	float luminance = 0.3f * color.x + 0.59 * color.y + 0.11 * color.z;
	color = (float4)(luminance, luminance, luminance, 1.0f);
	write_imagef(dst, coords, color);
}

__kernel void brightness(read_only image2d_t src, write_only image2d_t dst)
{
    float max = 0;
    int2 coords = (int2) (get_global_id(0), get_global_id(1));
    float4 color = read_imagef(src, smp, coords);

    max = color.x;
    if(color.y > max) { max = color.y; }
    if(color.z > max) { max = color.z; }    
    
	write_imagef(dst, coords, max);
}

__kernel void lightness(read_only image2d_t src, write_only image2d_t dst)
{
    float l = 0;
    int2 coords = (int2) (get_global_id(0), get_global_id(1));
    float4 color = read_imagef(src, smp, coords);
    
    l = (color.x + color.y + color.z) / 3.0f;
    
	write_imagef(dst, coords, l);
}



__kernel void hist()
{
    
}