
#ifndef CPPND_CAPSTONE_PROJECT_IMAGE_H
#define CPPND_CAPSTONE_PROJECT_IMAGE_H

#include <opencv2/opencv.hpp>
#include <string>
#include <utility>

enum LocationType {
    top_left,
    button_left,
    top_right,
    button_right,
    center
};

enum ImgType {
    bg_img,   // background img
    fg_img,   // foreground img
    fg_p      // foreground patch
};

class Image {

    cv::Mat fg_orig_img;
    cv::Mat bg_orig_img;
    cv::Mat fg_patch;
    cv::Mat img;        // merged image
    cv::Rect fg_p_r;
    LocationType locationType;
    std::pair<int, int> fgLocation;

public:

    Image() = default;
    ~Image() = default;

    void setImage(const ImgType imgType, const cv::Mat image);
    void setFgLocationType(const LocationType locType);
    void setFgLocation(const std::pair<int, int> newLocation);
    void setFgPatchRect(const cv::Rect rect);

    cv::Size getImgSize(const ImgType imgType) const;
    cv::Mat getImg(const ImgType imgType) const;
    std::string getFgLocationType() const;
    std::pair<int, int> getFgLocation() const;

    bool generateImg();
    void resizeImg(cv::Mat& image, cv::Size newSize);
    void resizeImg(const ImgType imgType, const cv::Size size);
    void scaleImg(const ImgType imgType, const int scalePercent);
    void saveImg(const std::string name) const;

    static std::string imgTypeToString(const ImgType imgType);
    cv::Rect calcFgRectFromLocation() const;
    bool isValidFgLocation() const;

private:
    static cv::Mat bgrToBgra(cv::Mat image);
    static cv::Mat mergeBGRAIntoBGR(cv::Mat small_fg, cv::Mat background, int top, int left);
    void generatePatch();

};

#endif //CPPND_CAPSTONE_PROJECT_IMAGE_H
