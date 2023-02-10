
#include "image.h"


void Image::setFgLocationType(const LocationType locType) {
    locationType = locType;
}

void Image::setFgLocation(const std::pair<int, int> newLocation) {
    fgLocation.first = newLocation.first;
    fgLocation.second = newLocation.second;
}

void Image::setImage(const ImgType imgType, const cv::Mat image) {
    if (imgType == ImgType::bg_img) {
        bg_orig_img = std::move(image);
    } else if (imgType == ImgType::fg_img) {
        fg_orig_img = std::move(image);
    } else {
        fg_patch = std::move(image);
    }
}

void Image::setFgPatchRect(const cv::Rect rect) {
    fg_p_r = rect;
}

cv::Size Image::getImgSize(const ImgType imgType) const {
    if (imgType == ImgType::bg_img) {
        return bg_orig_img.size();
    } else if (imgType == ImgType::fg_img) {
        return fg_orig_img.size();
    }
    return fg_patch.size();
}

cv::Mat Image::getImg(const ImgType imgType) const {
    if (imgType == ImgType::bg_img) {
        return bg_orig_img;
    } else if (imgType == ImgType::fg_img) {
        return fg_orig_img;
    }
    return fg_patch;
}

std::string Image::getFgLocationType() const {
    if (locationType == LocationType::top_left) {
        return "TL";
    } else if (locationType == LocationType::button_left) {
        return "BL";
    } else if (locationType == LocationType::top_right) {
        return "TR";
    } else if (locationType == LocationType::button_right) {
        return "BR";
    }
    return "C";
}

std::pair<int, int> Image::getFgLocation() const {
    return fgLocation;
}

cv::Mat Image::bgrToBgra(cv::Mat image) {
    /*
     * Adds additional channel to the image, which will determine the
     * transparency of the image according to a threshold.
     * return: a copy of the image with the alpha channel added.
     */

    cv::Mat src = image.clone();
    cv::Mat res;
    cv::Mat tmp, alpha;
    int threshold  = 253;

    cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);
    cv::threshold(tmp, alpha, threshold, 255, cv::THRESH_BINARY_INV);

    cv::Mat rgb[3];
    cv::split(src, rgb);

    cv::Mat rgba[4] = {rgb[0], rgb[1], rgb[2], alpha};
    cv::merge(rgba, 4, res);

    return res;
}

cv::Mat Image::mergeBGRAIntoBGR(cv::Mat small_fg, cv::Mat background, int top, int left){
    /*
      * Add a small BGRA picture in front of a larger BGR bg_orig_img.
      * small_fg: The patch image. Must have 4 channels.
      * background: The background image. Must have 3 channels.
      * top: Y position where to put the patch.
      * left: X position where to put the patch.
      * return: a copy of the background with the patch added.
     */

    cv::Mat result = background.clone();

    cv::Mat rgba[4];
    cv::split(small_fg,rgba);

    rgba[3] = rgba[3] / 225.0;

    cv::Mat rgb[3] = {rgba[0].mul(rgba[3]), rgba[1].mul(rgba[3]), rgba[2].mul(rgba[3])};
    cv::Mat label_rgb;
    cv::merge(rgb, 3, label_rgb);

    int height = small_fg.rows;
    int width = small_fg.cols;

    cv::Mat part_of_bg = result(cv::Range(top,top + height), cv::Range(left,left + width));
    cv::Mat bg_rgb[3];
    cv::split(part_of_bg, bg_rgb);

    cv::Mat bg_rgb_a[3]={bg_rgb[0].mul(1 - rgba[3]), bg_rgb[1].mul(1 - rgba[3]), bg_rgb[2].mul(1 - rgba[3])};
    cv::merge(bg_rgb_a, 3, part_of_bg);

    cv::add(label_rgb, part_of_bg, part_of_bg);
    result(cv::Range(top,top + height), cv::Range(left,left + width)) = part_of_bg;

    return result;
}

void Image::generatePatch() {
    /*
     * Uses grabCut algorithm to cut the desired object from fg_orig_img and
     * saves it into the fg_patch field.
     */

    cv::Mat result;
    cv::Mat bgmodel, fgmodel;

    cv::grabCut(fg_orig_img, result, fg_p_r, bgmodel, fgmodel, 5, cv::GC_INIT_WITH_RECT);
    cv::compare(result, cv::GC_PR_FGD, result, cv::CMP_EQ);
    cv::Mat fg(fg_orig_img.size(), CV_8UC3, cv::Scalar(255, 255, 255));
    fg_orig_img.copyTo(fg, result);

    cv::Mat cropped_img = fg(fg_p_r);
    fg_patch = std::move(cropped_img);
}

bool Image::generateImg(){

    generatePatch();
    if (!isValidFgLocation()) {
        return false;
    }
    cv::Mat rgba_img = bgrToBgra(fg_patch);

    // The rect on the background were we are going to add the fg patch
    cv::Rect rect = calcFgRectFromLocation();
    img = mergeBGRAIntoBGR(rgba_img, bg_orig_img, rect.y, rect.x);

    cv::imshow("Merged image", img);
    return true;
}

void Image::resizeImg(cv::Mat& image, cv::Size newSize) {

    cv::Size currSize = image.size();
    const int interpolation =
            currSize.height < newSize.height && currSize.width < newSize.width ?
            cv::INTER_AREA : cv::INTER_LINEAR;
    cv::resize(image, image, newSize, 0, 0 , interpolation);
}

void Image::resizeImg(const ImgType imgType, const cv::Size size) {

    if (imgType == ImgType::bg_img) {
        resizeImg(bg_orig_img, size);
    } else {
        resizeImg(fg_orig_img, size);
    }
}

void Image::scaleImg(const ImgType imgType, const int scalePercent) {

    if (imgType == ImgType::bg_img) {
        cv::Size bgS = bg_orig_img.size();
        cv::Size size(bgS.width * scalePercent / 100, bgS.height * scalePercent / 100);
        resizeImg(bg_orig_img, size);
    } else {
        cv::Size fgS = fg_orig_img.size();
        cv::Size size(fgS.width * scalePercent / 100, fgS.height * scalePercent / 100);
        resizeImg(fg_orig_img, size);
    }
}

std::string Image::imgTypeToString(const ImgType imgType) {
    return imgType == ImgType::bg_img ? "background" : "foreground";
}

cv::Rect Image::calcFgRectFromLocation() const {

    int fgx = fgLocation.first;
    int fgy = fgLocation.second;
    cv::Size fgSize = fg_patch.size();

    if (locationType == LocationType::top_left) {
        return cv::Rect(fgx, fgy, fgSize.width, fgSize.height);
    } else if (locationType == LocationType::button_left) {
        return cv::Rect(fgx, fgy - fgSize.height, fgSize.width, fgSize.height);
    } else if (locationType == LocationType::top_right) {
        return cv::Rect(fgx - fgSize.width, fgy, fgSize.width, fgSize.height);
    } else if (locationType == LocationType::button_right) {
        return cv::Rect(fgx  - fgSize.width, fgy - fgSize.height, fgSize.width, fgSize.height);
    }
    return cv::Rect(fgx  - (fgSize.width / 2), fgy - (fgSize.height / 2), fgSize.width, fgSize.height);
}

bool Image::isValidFgLocation() const {

    cv::Rect rect = calcFgRectFromLocation();
    return (rect & cv::Rect(0, 0, bg_orig_img.cols, bg_orig_img.rows)) == rect;
}

void Image::saveImg(const std::string name) const {

    cv::imwrite("../data/" + name + ".jpg", img);
}