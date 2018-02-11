#ifndef BDS_H
#define BDS_H

#include "patchmatch.h"

class BDS
{
    Image &img1;
    Image &img2;

    NNF &nnf1;
    NNF &nnf2;

    Image result;
public:
    BDS(Image &img1, Image &img2, NNF &nnf1, NNF &nnf2);

    Image &Restore();

    Image &getResult();
};

#endif // BDS_H
