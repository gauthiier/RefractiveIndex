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

#define WORK_G_SIZE         64
#define HALF_WORK_G_SIZE    (WORK_G_SIZE / 2)

const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void scan(read_only image2d_t src, write_only image2d_t dst, int rows, int cols)
{
    __local float data0[WORK_G_SIZE + HALF_WORK_G_SIZE];
    __local float data1[WORK_G_SIZE + HALF_WORK_G_SIZE];
    
    int2 coords = (int2) (get_global_id(0), get_global_id(1));
    int X = coords.x;
    int Y = coords.x;
    
    if(coords.x < HALF_WORK_G_SIZE) {
        data0[coords.x] = 0.0f;
        data1[coords.x] = 0.0f;
    }
    
    X += HALF_WORK_G_SIZE;    
    float max_val = 0.0f;    
    
    int it = cols / WORK_G_SIZE;    
    if(cols % WORK_G_SIZE != 0) {
        it++;
    }
    
    for(int i = 0; i < it; i++) {
        
        int col_offset = i * WORK_G_SIZE + coords.x;
        
        data0[X] = read_imagef(src, smp, (int2)(col_offset, Y)).x;        
        barrier(CLK_LOCAL_MEM_FENCE);
        
        // 1
        data1[X] = data0[X] + data0[X-1];
        barrier(CLK_LOCAL_MEM_FENCE);
        
        // 2
        data0[X] = data1[X] + data1[X-2];
        barrier(CLK_LOCAL_MEM_FENCE);
        
        // 4
        data1[X] = data0[X] + data0[X-4];
        barrier(CLK_LOCAL_MEM_FENCE);
        
        // 8
        data0[X] = data1[X] + data1[X-8];
        barrier(CLK_LOCAL_MEM_FENCE);
        
        // 16
        data1[X] = data0[X] + data0[X-16];
        barrier(CLK_LOCAL_MEM_FENCE);
        
        // 32
        data0[X] = data1[X] + data1[X-32];
        barrier(CLK_LOCAL_MEM_FENCE);
        
        if(col_offset < cols) {
            write_imagef(dst, (int2)(col_offset, Y), (float4)(data0[X] + max_val, 0.0f, 0.0f, 0.0f));
        }
        
        max_val += data0[WORK_G_SIZE + HALF_WORK_G_SIZE - 1];
    }
    
}

__kernel void transpose(read_only image2d_t src, write_only image2d_t dst, int rows, int cols)
{
    
    __local float buff[256];
    
    int2 coords = (int2) (get_global_id(0), get_global_id(1));
    
    int inX = coords.x;
    int inY = coords.y;
    
    int lX = coords.x;
    int lY = coords.y;
    
    int ginX = coords.x * 16 + lX;
    int ginY = coords.y * 16 + lY;
    
    buff[lY * 16 + lX] = read_imagef(src, smp, (int2)(ginX, ginY)).x;
    barrier(CLK_LOCAL_MEM_FENCE);
    
    int outRows = rows;
    int outCols = cols;
    
    int outX = inX;
    int outY = inY;
    
    int goutX = coords.x * 16 + lX;
    int goutY = coords.y * 16 + lY;
    
    if(goutX >= 0 && goutX < outCols && goutY >=0 && goutY < outRows) {
        write_imagef(dst, (int2)(goutX, goutY), (float4)(buff[lX * 16 + lY], 0.0f, 0.0f, 0.0f));
    }
}

