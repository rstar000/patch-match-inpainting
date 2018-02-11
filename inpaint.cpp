#include "inpaint.h"

Image inpaint(MaskedImage &img)
{
    int num_scales = 6;
    MaskedImage source, target;
    MaskedImage current;
    for(int i = num_scales - 1; i >= 0; i--) {
        float scale = 1.0 / std::pow(2,i);
        std::cout << "Scale: " <<  scale << std::endl;
        Image img_scaled = rescale(img.img, scale);
        Mask mask_scaled = rescale(img.mask, scale);

        int num_em = 3;
        if(i == 0 or i == num_scales - 1) {
            num_em = 1;
        }
        for(int j = 0; j < num_em; j++) {
            if(i == num_scales - 1) {
                source = MaskedImage(img_scaled);
                target = MaskedImage(img_scaled);
            } else {
                int h,w;
                auto sh = img_scaled.Shape();
                h = sh[0];
                w = sh[1];

                source.img = img_scaled;
                source.mask = mask_scaled;

                target.img = resize(current.img, w,h);
                target.mask = mask_scaled;
            }

            PatchMatch source_to_target(source, target, 2, 4);
            PatchMatch target_to_source(target, source, 2, 4);

            source_to_target.Match();
            target_to_source.Match();

            BDS bds(source.img, target.img, source_to_target.getNNF(), target_to_source.getNNF());
            Image res = bds.Restore();
            current.img = res;
            current.mask = source.mask;
        }

    }
    return current.img;
}
