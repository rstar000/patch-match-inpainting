#ifndef UTILS_H
#define UTILS_H

#include <QImage>
#include <tuple>

#include "patchmatch.h"

Image qimg_to_img(QImage &qimg);
QImage img_to_qimg(Image &img);

Image resize(Image &input, int new_width, int new_height);
Mask resize(Mask &input, int new_width, int new_height);

Image rescale(Image &input, float scale);
Mask rescale(Mask &input, float scale);
Mask img_to_mask(Image &img);
#endif // UTILS_H
