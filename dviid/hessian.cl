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

const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

const sampler_t smp_adrs = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;


float box_integral(read_only image2d_t src, int width, int height, int row, int col, int nbrrows, int nbrcols)
{
    float A = 0.0f;
    float B = 0.0f;
    float C = 0.0f;
    float D = 0.0f;
    
    int r0 = min(row, height) - 1;
    int c0 = min(col, width) - 1;
    int r1 = min(row + nbrrows, height) - 1;
    int c1 = min(col + nbrcols, width) - 1;
    
    A = read_imagef(src, smp, (int2)(c0, r0)).x;
    B = read_imagef(src, smp, (int2)(c1, r0)).x;
    C = read_imagef(src, smp, (int2)(c0, r1)).x;
    D = read_imagef(src, smp, (int2)(c1, r1)).x;
    
    return max(0.0f, A - B - C + D);
    
}


__kernel void hessian_det(
                          read_only image2d_t src,
                          int width,
                          int height,
                          write_only image2d_t determinant,
                          write_only image2d_t laplacians,
                          int layer_width,
                          int layer_height,
                          int step,
                          int filter)
{
    int l, w, b;
    float Dxx, Dxy, Dyy, inverse;
    
    int idx = get_global_id(0);
    int idy = get_global_id(1);
    
    w = filter;
    l = w / 3;
    b = (w - 1) / 2 + 1
    inverse = 1.0f / (w * w);
    
    int c = idx * step;
    int r = idy * step;
    
    if(r >= height || c >= width) return;
    
    Dxx = box_integral(src, width, height, r - l + 1, c - b, 2 * l - 1, w) -
          box_integral(src, width, height, r - l + 1, c - l / 2, 2 * l - 1, l) * 3;
    
    Dxy = box_integral(src, width, height, r - l, c + 1, l, l) +
          box_integral(src, width, height, r + 1, c - l, l, l) -
          box_integral(src, width, height, r - 1, c - l, l, l) -
          box_integral(src, widht, height, r + 1, c + 1, l, l);
    
    DYY = box_integral(src, width, height, r - b, c - l + 1, w, 2 * l - 1) -
          box_integral(src, width, height, r - l / 2, c - l + 1, l, 2 * l -1) * 3;
    
    Dxx += inverse; 
    Dxy += inverse; 
    Dyy += inverse; 
    
    float4 det = {0.0f, 0.0f, 0.0f, 0.0f};
    det.x = (Dxx * Dyy - 0.81f * Dxy * Dxy);
    
    int4 lap = {0, 0, 0, 0};
    lap.x = (Dxx + Dyy >= 0 ? 1 : 0);
    
    write_imagef(determinant, (int2)(idx, idy), det);
    write_imagef(laplacians, (int2)(idx, idy), lap);
    
}

int pop_laplacian(read_only image2d_t layer, int c, int r, int width)
{
    int lap;
    lap = read_imagei(layer, smp_adrs, (int2)(c,r)).x;
    return lap;
}

float pop_response(read_only image2d_t layer, int c, int r, int width, int scale)
{
    float resp;
    resp = read_imagef(layer, smp_adrs, (int2)(c*scale, r*scale)).x;
    return resp;
}

bool interpolate_extremum(
                          int r,
                          int c,
                __global int* pts_cnt;
                      float2* pos,
                       float* det_scale
                         int* laplacian,
          read_only image2d_t t,
                          int t_width,
                          int t_height,
                          int t_step,
          read_only image2d_t m,
          read_only image2d_t mlaplacian,
                          int m_width,
                          int m_height,
                          int m_filter,
          read_only image2d_t b,
                          int b_width,
                          int b_height,
                          int b_filter
                          )
{
    
    // 3D derivatives
    
    int mscale = (m_width / m_height);
    int bscale = (b_width / b_height);
    
    float Dx, Dy, Dz;
    
    Dx = (pop_response(m, c+1, r,   m_width, mscale) -
          pop_response(m, c-1, r,   m_width, mscale)) / 2.0f;
    Dy = (pop_response(m, c,   r+1, m_width, mscale) -
          pop_response(m, c,   r-1, m_width, mscale)) / 2.0f;
    
    Dz = (pop_response(t, c, r, t_width, 1) - 
          pop_response(b, c, r, b_width, bscale)) / 2.0f;
          
    // inverse hessian
    
    float v, Dxx, Dyy, Dzz, Dxy, Dxz, Dyz;
    
    v = pop_response(m, r, c, m_width, mscale);
    
    Dxx = pop_response(m, c+1, r,   m_width, mscale) + 
          pop_response(m, c-1, r,   m_width, mscale) - 2.0f * v;
    
    Dyy = pop_response(m, c,   r+1, m_width, mscale) +
          pop_response(m, c,   r-1, m_width, mscale) - 2.0f * v;
        
    Dxy = (pop_response(m, c+1, r+1, m_width, mscale) -
           pop_response(m, c-1, r+1, m_width, mscale) -
           pop_response(m, c+1, r-1, m_width, mscale) +
           pop_response(m, c-1, r-1, m_width, mscale)) / 4.0f;
    
    Dzz = pop_response(t, c, r, t_width, 1) - 
          pop_response(b, c, r, b_width, bscale) - 2.0f * v;
    
    Dxz = (pop_response(t, c+1, r, t_width, 1) -
           pop_response(t, c-1, r, t_width, 1) -
           pop_response(b, c+1, r, b_width, bscale) +
           pop_response(b, c-1, r, b_width, bscale)) / 4.0f;
    
    Dyz = (pop_response(t, c, r+1, t_width, 1) -
           pop_response(t, c, r-1, t_width, 1) -
           pop_response(b, c, r+1, b_width, bscale) +
           pop_response(b, c, r-1, b_width, bscale)) / 4.0f;
    
    float det = Dxx * (Dyy*Dzz - Dyz*Dyz) -
                Dxy * (Dxy*Dzz - Dyz*Dxz) +
                Dxz * (Dxy*Dyz - Dyy*Dxz);

    float invdet = 1.0f / det;
    
    float invDxx =  (Dyy*Dzz-Dyz*Dyz) * invdet;
    float invDxy = -(Dxy*Dzz-Dyz*Dxz) * invdet;
    float invDxz =  (Dxy*Dyz-Dyy*Dxz) * invdet;
    float invDyx = -(Dxy*Dzz-Dxz*Dyz) * invdet;
    float invDyy =  (Dxx*Dzz-Dxz*Dxz) * invdet;
    float invDyz = -(Dxx*Dyz-Dxy*Dxz) * invdet;
    float invDzx =  (Dxy*Dyz-Dxz*Dyy) * invdet;
    float invDzy = -(Dxx*Dyz-Dxz*Dxy) * invdet;
    float invDzz =  (Dxx*Dyy-Dxy*Dxy) * invdet;
    
    // derivative * hessian
    
    float xi = 0.0f, xr = 0.0f, xc = 0.0f;
    
    xc -= invDxx * Dx;
    xc -= invDxy * Dy;
    xc -= invDxz * Dz;
    
    xr -= invDyx * Dx;
    xr -= invDyy * Dy;
    xr -= invDyz * Dz;
    
    xc -= invDzx * Dx;
    xc -= invDzy * Dy;
    xc -= invDzz * Dz;
    
    // extremum??
    if(fabs(xi) < 0.5f && fabs(xr) < 0.5f && fabs(xc) < 0.5f) {
        
        int fstep = m_filter - b_filter;
        
        (*pos).x = (float)((c + xc) * fstep);
        (*pos).y = (float)((c + xr) * fstep);
        *det_scale = (float)(0.1333f) * (m_filter + (xi * fstep));
        
        int s = m_width / t_width;
        *laplacian = pop_laplacian(mlaplacian, c * s, r * s, m_width);
        
        return true;
    }
    
    return false;
        
}

bool is_extremum(
                 int r,
                 int c,
                 read_only image2d_t t,
                 int t_width,
                 int t_height,
                 int t_step,
                 int t_filter,
                 read_only image2d_t m,
                 int m_width,
                 int m_height,
                 read_only image2d_t b,
                 int b_width,
                 int b_height,
                 float tresh
                 )
{
    int border = (t_filter + 1) / (2 * t_step);
    
    if(r <= border || r >= t_height - border || c <= border || c >= t_width - border) {
        return false;
    }
 
    int mscale = m_width / t_width;
    
    float candidate = pop_response(m, c, r, m_width, mscale);
    if(candidate < tresh) {
        return false;
    }
    
    // If any response in 3x3x3 is greater candidate not maximum
    float localMax =          getResponse(t, c-1, r-1, t_width, 1);
    localMax = fmax(localMax, getResponse(t, c,   r-1, t_width, 1));
    localMax = fmax(localMax, getResponse(t, c+1, r-1, t_width, 1));
    localMax = fmax(localMax, getResponse(t, c-1, r,   t_width, 1));
    localMax = fmax(localMax, getResponse(t, c,   r,   t_width, 1));
    localMax = fmax(localMax, getResponse(t, c+1, r,   t_width, 1));
    localMax = fmax(localMax, getResponse(t, c-1, r+1, t_width, 1));
    localMax = fmax(localMax, getResponse(t, c,   r+1, t_width, 1));
    localMax = fmax(localMax, getResponse(t, c+1, r+1, t_width, 1));
    
    int bScale = b_width/t_width;
    
    localMax = fmax(localMax, getResponse(b, c-1, r-1, b_width, bScale));
    localMax = fmax(localMax, getResponse(b, c,   r-1, b_width, bScale));
    localMax = fmax(localMax, getResponse(b, c+1, r-1, b_width, bScale));
    localMax = fmax(localMax, getResponse(b, c-1, r,   b_width, bScale));
    localMax = fmax(localMax, getResponse(b, c,   r,   b_width, bScale));
    localMax = fmax(localMax, getResponse(b, c+1, r,   b_width, bScale));
    localMax = fmax(localMax, getResponse(b, c-1, r+1, b_width, bScale));
    localMax = fmax(localMax, getResponse(b, c,   r+1, b_width, bScale));
    localMax = fmax(localMax, getResponse(b, c+1, r+1, b_width, bScale));
    
    //int mScale = m_width/t_width;
    
    localMax = fmax(localMax, getResponse(m, c-1, r-1, m_width, mScale));
    localMax = fmax(localMax, getResponse(m, c,   r-1, m_width, mScale));
    localMax = fmax(localMax, getResponse(m, c+1, r-1, m_width, mScale));
    localMax = fmax(localMax, getResponse(m, c-1, r,   m_width, mScale));
    // This is the candidate pixel
    localMax = fmax(localMax, getResponse(m, c+1, r,   m_width, mScale));
    localMax = fmax(localMax, getResponse(m, c-1, r+1, m_width, mScale));
    localMax = fmax(localMax, getResponse(m, c,   r+1, m_width, mScale));
    localMax = fmax(localMax, getResponse(m, c+1, r+1, m_width, mScale));
    
    // If localMax > candidate, candidate is not the local maxima
    if(localMax > candidate) {
        return false;
    }
    
    return true;   
    
}

__kernel void suppress_non_max(                               
               read_only image2d_t tResponse,
                               int t_width,
                               int t_height,
                               int t_filter,
                               int t_step,
               read_only image2d_t mResponse,                
               read_only image2d_t mLaplacian,
                               int m_width,
                               int m_height,
                               int m_filter,
               read_only image2d_t bResponse,           
                               int b_width;
                               int b_height,
                               int b_filter,
                      __global int* pts_cnt,
                   __global float2* pix_pos,
                    __global float* scale,
                      __global int* laplacian,
                               int max_pts,
                             float tresh                               
                               )
{
    int r = get_global_id(0);
    int c = get_global_id(1);
    
    float2 pixpos;
    float s;
    int lap;
    
    
    if(is_extremum(r, c, tResponse, t_width, t_height, t_step, t_filter, mResponse, m_width, m_height, bResponse, b_width, b_height, tresh)) {
        
        if(interpolate_extremum(r, c, pts_cnt, &pixpos, &s, &lap, tResponse, t_width, t_height, t_step, mResponse, mLaplacian, m_width, m_height, m_filter, bResponse, b_width, b_height, b_filter)) {
            
            int indx = atom_add(&pts_cnt[0],1);
            if(indx < max_pts) {
                pix_pos[indx] = pix_pos;
                scale[indx] = s;
                laplacian[indx] = lap;
            }
            
        }
        
    }
}
                               
                               
                          
                          
                          




