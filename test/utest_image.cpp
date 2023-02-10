
#include <gtest/gtest.h>
#include <iostream>

#include "image.h"


Image createTestImage() {

    std::string bg_img_path = "../data/bg.jpeg";      // width: 275, height: 183
    std::string fg_path = "../data/small_whitney.jpg";  // 71 93

    cv::Mat bg = cv::imread(bg_img_path);
    cv::Mat fg = cv::imread(fg_path);

    Image img;
    img.setImage(ImgType::bg_img, bg);
    img.setImage(ImgType::fg_p, fg);
    cv::Size bgSize = img.getImgSize(ImgType::bg_img);

    return img;
}


class ImageTest : public ::testing::Test {
protected:
    Image img = createTestImage();
    cv::Size bgSize = img.getImgSize(ImgType::bg_img);
    cv::Size fgSize = img.getImgSize(ImgType::fg_p);

};


TEST_F(ImageTest, TestCalcFgRectFromLocationCenter) {

    img.setFgLocationType(LocationType::center);
    img.setFgLocation(std::pair<int, int>(bgSize.width / 2, bgSize.height / 2));

    int x = (bgSize.width / 2) - (fgSize.width / 2);
    int y = (bgSize.height / 2) - (fgSize.height / 2);
    cv::Rect expected = cv::Rect(x, y, fgSize.width, fgSize.height);
    cv::Rect res = img.calcFgRectFromLocation();

    EXPECT_EQ(res, expected);
}