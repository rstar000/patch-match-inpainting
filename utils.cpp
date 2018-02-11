#include "utils.h"
#include <QColor>
Image qimg_to_img(QImage &qimg)
{
    int w = qimg.width();
    int h = qimg.height();
    Image img({h,w,3});
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            QColor col(qimg.pixel(j,i));
            img({i,j,0}) = col.redF();
            img({i,j,1}) = col.greenF();
            img({i,j,2}) = col.blueF();
        }
    }

    return img;
}

QImage img_to_qimg(Image &img)
{
    auto shape = img.Shape();
    int h = shape[0];
    int w = shape[1];
    QImage qimg(w,h, QImage::Format_RGB888);
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            QColor col;
            col.setRedF(img({i,j,0}));
            col.setGreenF(img({i,j,1}));
            col.setBlueF(img({i,j,2}));
            qimg.setPixelColor(j,i,col);
        }
    }
    return qimg;
}

Image resize(Image &input, int new_width, int new_height)
{
    Image output({new_height, new_width, 3});
    int h,w,c;
    std::tie(h,w,c) = input.shapeTuple();

    for(int i = 0; i < new_height; i++) {
        for(int j = 0; j < new_width; j++) {
            int n = (float(i) / new_height) * h;
            int m = (float(j) / new_width) * w;

            output(i,j,0) = input(n,m,0);
            output(i,j,1) = input(n,m,1);
            output(i,j,2) = input(n,m,2);
        }
    }
    return output;
}

Mask resize(Mask &input, int new_width, int new_height)
{
    Mask output({new_height, new_width});
    int h,w;
    auto sh = input.Shape();
    h = sh[0];
    w = sh[1];

    for(int i = 0; i < new_height; i++) {
        for(int j = 0; j < new_width; j++) {
            int n = (float(i) / new_height) * h;
            int m = (float(j) / new_width) * w;

            output(i,j) = input(n,m);
        }
    }
    return output;
}

Mask img_to_mask(Image &img)
{
    int h,w;
    auto sh = img.Shape();
    h = sh[0];
    w = sh[1];
    Mask output({h,w});

    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            output(i,j) = img(i,j,0) > 0 ? 1 : 0;
        }
    }
    return output;
}

Mask rescale(Mask &input, float scale)
{
    auto sh = input.Shape();
    int w = sh[1];
    int h = sh[0];

    return resize(input, w*scale, h*scale);
}

Image rescale(Image &input, float scale)
{
    auto sh = input.Shape();
    int w = sh[1];
    int h = sh[0];

    return resize(input, w*scale, h*scale);
}
