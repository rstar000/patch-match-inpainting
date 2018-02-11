#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "patchmatch.h"
#include "bds.h"
#include "utils.h"
#include "inpaint.h"

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
    QImage qimg1("img/img7.jpg");

    Image img1 = qimg_to_img(qimg1);
    MaskedImage mimg1(img1);
//    mimg1.createSquareMask(396, 414, 159, 177);
    mimg1.createSquareMask(11, 310, 414, 650);
    Image res = inpaint(mimg1);
    QImage nimg = img_to_qimg(res);
    nimg.save("res.png");
    return 0;
}
