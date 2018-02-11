#include "bds.h"

BDS::BDS(Image &img1, Image &img2, NNF &nnf1, NNF &nnf2) :
    img1(img1), img2(img2), nnf1(nnf1), nnf2(nnf2)
{
    auto shape = img1.Shape();
    int h1 = shape[0];
    int w1 = shape[1];

    result.Resize({h1,w1,3});
}

Image &BDS::Restore()
{
    auto nnf_sh = nnf1.ssd.Shape();
    int nnfh = nnf_sh[0];
    int nnfw = nnf_sh[1];
    int h,w,c;
    std::tie(h,w,c) = img1.shapeTuple();
    ndarray<float> weights({h,w});
    int diam = 2 * nnf1.radius + 1;
    auto put_patch = [this, &weights, diam](int y, int x)
    {
        int off_y = nnf1.offset({y,x,0});
        int off_x = nnf1.offset({y,x,1});

        int off_y2 = nnf2.offset(y,x,0);
        int off_x2 = nnf2.offset(y,x,1);
        float weight = 1.0 / (nnf1.ssd(y, x) + 0.0001);
        float weight2 = 1.0 / (nnf2.ssd(y,x) + 0.0001);
        //std::cout << weight << " " << img2({y,x,0}) << std::endl;
        for(int i = 0; i < diam; i++) {
            for(int j = 0; j < diam; j++) {
                result({y+i,x+j,0}) += weight * img2({off_y+i, off_x+j, 0});
                result({y+i,x+j,1}) += weight * img2({off_y+i, off_x+j, 1});
                result({y+i,x+j,2}) += weight * img2({off_y+i, off_x+j, 2});

                result({y+i,x+j,0}) += weight2 * img1({off_y2+i, off_x2+j, 0});
                result({y+i,x+j,1}) += weight2 * img1({off_y2+i, off_x2+j, 1});
                result({y+i,x+j,2}) += weight2 * img1({off_y2+i, off_x2+j, 2});
                weights({y+i,x+j}) += weight + weight2;
            }
        }
    };

    for(int i = 0; i < nnfh; i++) {
        for(int j = 0; j < nnfw; j++) {
            put_patch(i,j);
        }
    }

    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            float weight = weights({i,j});
            result({i,j,0}) /= weight;
            result({i,j,1}) /= weight;
            result({i,j,2}) /= weight;
        }
    }

    std::cout << result.Shape()[0];
    return result;
}

Image &BDS::getResult()
{
    return result;
}
