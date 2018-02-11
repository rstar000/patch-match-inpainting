#include "patchmatch.h"

int clamp(int x, int min, int max) {
    if(x < min) return min;
    if(x > max) return max;
    return x;
}

NNF::NNF(int width, int height) :
    ssd({height, width}),
    offset({height, width, 2})
{ }

NNF::NNF()
{ }

NNF NNF::resize(int new_width, int new_height)
{
    NNF output(new_width, new_height);
    output.radius = radius;

    auto sh = ssd.Shape();
    int width = sh[1];
    int height = sh[0];

    float scale_x = float(new_width) / width;
    float scale_y = float(new_height) / height;
    for(int i = 0; i < new_height; i++) {
        for(int j = 0; j < new_width; j++) {
            int n = (float(j) / new_height) * height;
            int m = (float(i) / new_width) * width;

            output.offset(i,j,0) = scale_y * offset(n,m,0);
            output.offset(i,j,1) = scale_x * offset(n,m,1);
            output.ssd(i,j) = ssd(n,m);
        }
    }
    return output;
}

float PatchMatch::SSD(int y1, int x1, int y2, int x2)
{
    Image &img1 = mimg1.img;
    Image &img2 = mimg1.img;
    Mask &mask1 = mimg1.mask;
    Mask &mask2 = mimg2.mask;
    float sum = 0;
    for(int i = 0; i < 2*radius+1; i++) {
        for(int j = 0; j < 2*radius+1; j++) {
            if(mask1(y1+i,x1+j) and mask2(y2+i,x2+j)) {
                sum += 100;
                continue;
            }

            float r = img1(y1+i,x1+j,0) - img2(y2+i,x2+j,0);
            float g = img1(y1+i,x1+j,1) - img2(y2+i,x2+j,1);
            float b = img1(y1+i,x1+j,2) - img2(y2+i,x2+j,2);
            sum += r*r + g*g + b*b;
        }
    }
    return sum;
}

void PatchMatch::propagate(int dir, int iter)
{
    int h1, w1, c1;
    int h2, w2, c2;
    std::tie(h1, w1, c1) = mimg1.img.shapeTuple();
    std::tie(h2, w2, c2) = mimg2.img.shapeTuple();

    int nnfw = w1 - 2*radius - 1;
    int nnfh = h1 - 2*radius - 1;

    w2 -= 2*radius + 1;
    h2 -= 2*radius + 1;

    std::uniform_real_distribution<float> dist(-1, 1);
    int search_radius = std::max(1, int(nnfw * pow(0.5,iter)));
    for(int i = 0; i < nnfh; i++) {
        for(int j = 0; j < nnfw; j++) {
            /* Propagate */
            int cur_y = i;
            int cur_x = j;
            if(dir < 0) {
                cur_y = nnfh - i - 1;
                cur_x = nnfw - j - 1;
            }

            float top_error = INF;
            float left_error = INF;

            int best_y = nnf.offset({cur_y,cur_x,0});
            int best_x = nnf.offset({cur_y,cur_x,1});
            float best_error = nnf.ssd({cur_y,cur_x});

            int top_x = 0, top_y = 0;
            int left_x = 0, left_y = 0;

            if(dir > 0) {
                if(cur_y - 1 > 1) {
                    top_y = nnf.offset({cur_y-1,cur_x,0}) + 1;
                    top_x = nnf.offset({cur_y-1,cur_x,1});
                    if(top_y < h2) {
                        top_error = SSD(cur_y,cur_x,top_y,top_x);
                    } else {
                        top_error = INF;
                    }
                }

                if(cur_x - 1 > 1) {
                    left_y = nnf.offset({cur_y,cur_x-1,0});
                    left_x = nnf.offset({cur_y,cur_x-1,1}) + 1;
                    if(left_x < w2) {
                        left_error = SSD(cur_y,cur_x,left_y,left_x);
                    } else {
                        left_error = INF;
                    }
                }
            } else {
                if(cur_y - dir < nnfh) {
                    top_y = nnf.offset({cur_y+1,cur_x,0}) - 1;
                    top_x = nnf.offset({cur_y+1,cur_x,1});
                    if(top_y >= 0) {
                        top_error = SSD(cur_y,cur_x,top_y,top_x);
                    } else {
                        top_error = INF;
                    }
                }

                if(cur_x - dir < nnfw) {
                    left_y = nnf.offset({cur_y,cur_x+1,0});
                    left_x = nnf.offset({cur_y,cur_x+1,1}) -1;
                    if(left_x >= 0) {
                        left_error = SSD(cur_y,cur_x,left_y,left_x);
                    } else {
                        left_error = INF;
                    }
                }
            }

            if(top_error < best_error) {
                best_error = top_error;
                best_x = top_x;
                best_y = top_y;
            }

            if(left_error < best_error) {
                best_error = left_error;
                best_x = left_x;
                best_y = left_y;
            }

            nnf.offset({cur_y, cur_x, 0}) = best_y;
            nnf.offset({cur_y, cur_x, 1}) = best_x;
            nnf.ssd({cur_y,cur_x}) = best_error;

            //if(mimg1.mask({cur_y, cur_x})) continue;
            /* Random search */
            for(int k = 0; k < num_points; k++) {
                float dx = dist(mt);
                float dy = dist(mt);
                int off_y = nnf.offset({cur_y, cur_x, 0});
                int off_x = nnf.offset({cur_y, cur_x, 1});
                int new_y = off_y + dy * search_radius;
                int new_x = off_x + dx * search_radius;
                new_y = clamp(new_y, 0, nnfh);
                new_x = clamp(new_x, 0, nnfw);

                float cur_error = nnf.ssd({cur_y, cur_x});
                float new_error = SSD(cur_y, cur_x, new_y, new_x);
                if(new_error < cur_error) {
                    nnf.offset({cur_y, cur_x, 0}) = new_y;
                    nnf.offset({cur_y, cur_x, 1}) = new_x;
                    nnf.ssd({cur_y, cur_x}) = new_error;
                }
            }
        }
    }
}


PatchMatch::PatchMatch(MaskedImage &img1, MaskedImage &img2, int radius, int num_iter) :
    mimg1(img1), mimg2(img2), radius(radius), mt(rd()), num_iterations(num_iter)
{
    auto sh = img1.img.Shape();
    int h = sh[0];
    int w = sh[1];

    nnf.offset.Resize({h-2*radius,w-2*radius,2});
    nnf.ssd.Resize({h-2*radius,w-2*radius});
    nnf.radius = radius;

    initNNF();
}

void PatchMatch::initNNF()
{
    int h1, w1, c1;
    int h2, w2, c2;
    std::tie(h1, w1, c1) = mimg1.img.shapeTuple();
    std::tie(h2, w2, c2) = mimg2.img.shapeTuple();

    int nnfw = w1 - 2*radius - 1;
    int nnfh = h1 - 2*radius - 1;

    std::uniform_int_distribution<> rx(0, w2-2*radius - 1);
    std::uniform_int_distribution<> ry(1, h2-2*radius - 1);

    for(int i = 0; i < nnfh; i++) {
        for(int j = 0; j < nnfw; j++) {
            int new_y = ry(mt);
            int new_x = rx(mt);
            nnf.offset({i,j,0}) = new_y;
            nnf.offset({i,j,1}) = new_x;
            nnf.ssd({i,j}) = SSD(i,j,new_y,new_x);
        }
    }
}

void PatchMatch::setNNF(NNF &new_nnf)
{
    nnf = new_nnf;
}

void PatchMatch::Match()
{
    for(int i = 0; i < num_iterations; i++) {
        //std::cout << "Iteration " << i+1 << std::endl;
        int dir = 1;
        if(i % 2) dir = -1;

        propagate(dir, i);
    }
}

NNF &PatchMatch::getNNF()
{
    return nnf;
}

MaskedImage::MaskedImage(Image img) :
    img(img)
{
    auto sh = img.Shape();
    int h = sh[0];
    int w = sh[1];
    mask.Resize({h,w});
}

void MaskedImage::createSquareMask(int radius)
{
    int h, w;
    auto sh = mask.Shape();
    h = sh[0];
    w = sh[1];
    int cx = w/2;
    int cy = h/2;
    for(int i = -radius; i <= radius; i++) {
        for(int j = -radius; j <= radius; j++) {
            mask({cy+i, cx+j}) = 1;
        }
    }
}

void MaskedImage::createSquareMask(int x1, int x2, int y1, int y2)
{
    int h, w;
    auto sh = mask.Shape();
    h = sh[0];
    w = sh[1];
    int cx = w/2;
    int cy = h/2;
    for(int i = y1; i <= y2; i++) {
        for(int j = x1; j <= x2; j++) {
            mask({i, j}) = 1;
        }
    }
}
