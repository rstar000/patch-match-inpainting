#ifndef PATCHMATCH_H
#define PATCHMATCH_H

#include <vector>
#include <random>
#include <iostream>

#include <QImage>

#include "ndarray.h"

const double INF = 999999.9;

typedef ndarray<float> Image;
typedef ndarray<int> Mask;

struct MaskedImage
{
    Image img;
    Mask mask;

    MaskedImage(Image img);
    MaskedImage() {}
    void createSquareMask(int radius);
    void createSquareMask(int x1, int x2, int y1, int y2);
};

struct NNF
{
public:
    NNF(int width, int height);
    NNF();
    ndarray<float> ssd;
    ndarray<int> offset;
    int radius;

    NNF resize(int new_width, int new_height);
};

class PatchMatch
{
    MaskedImage &mimg1;
    MaskedImage &mimg2;
    int radius;

    NNF nnf;

    std::random_device rd;
    std::mt19937 mt;
    int num_points = 10;
    int num_iterations;
    float SSD(int y1, int x1, int y2, int x2);
    void propagate(int dir, int iter);
    void random_search(int iter);
public:
    PatchMatch(MaskedImage &mimg1, MaskedImage &mimg2, int radius, int num_iter);

    void initNNF();

    void setNNF(NNF &new_nnf);

    void Match();

    NNF& getNNF();

};

#endif // PATCHMATCH_H
