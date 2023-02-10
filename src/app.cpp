
#include "app.h"


std::pair<int, int> App::getFgLocationFromUser() const {

    const std::string windowName = "Background image";
    cv::Rect rect = cv::selectROI(windowName, img.getImg(ImgType::bg_img));
    cv::destroyWindow("Background image");

    int x = rect.x + (rect.width / 2);
    int y = rect.y + (rect.height / 2);

    return std::pair<int, int>(x, y);
}

cv::Rect App::getFgPatchRectFromUser() const {

    cv::Rect rect = parser.getFgPatchRectFromConfig();

    if (rect.empty()) {
        std::cout << "\nSelect the part of the foreground image you want to paste on the background image \n" << std::endl;
        const std::string windowName = "foreground image";
        rect = cv::selectROI(windowName, img.getImg(ImgType::fg_img));
        cv::destroyWindow("foreground image");
    } else {
        std::cout << "\nThe config file defines the the part of the foreground image you want to paste on the background image\n" << std::endl;
    }

    std::cout << "Selected Rect: " << rect << std::endl;
    return rect;
}

void App::getPicturesFromUser() {

    cv::Mat bg = std::move(parser.getPicFromUser(ImgType::bg_img));
    img.setImage(ImgType::bg_img, std::move(bg));
    if (parser.checkIfUserWantsToResizeImage(ImgType::bg_img)) {
        auto scalePercent = parser.getScalePercentForNewImgSizeFromUser(ImgType::bg_img);
        img.scaleImg(ImgType::bg_img, scalePercent);
    }

    cv::Mat fg = std::move(parser.getPicFromUser(ImgType::fg_img));
    img.setImage(ImgType::fg_img, std::move(fg));
    if (parser.checkIfUserWantsToResizeImage(ImgType::fg_img)) {
        auto scalePercent = parser.getScalePercentForNewImgSizeFromUser(ImgType::fg_img);
        img.scaleImg(ImgType::fg_img, scalePercent);
    }

    cv::Rect r = getFgPatchRectFromUser();
    img.setFgPatchRect(r);
}

void App::run() {


    getPicturesFromUser();

    img.setFgLocationType(parser.getFgLocationTypeFromUser());
    img.setFgLocation(parser.getFgLocationFromUser(img.getImgSize(ImgType::bg_img)));

    while (!img.generateImg()) {
        std::cout << "\nThe selected area is invalid, the patch goes out of the background, please try again \n" << std::endl;
        img.setFgLocationType(LocationType::center);
        img.setFgLocation(getFgLocationFromUser());
    }

    std::cout << "\nPress any key to continue" << std::endl;
    cv::waitKey(0);
    if (parser.checkIfUserWantsToSaveTheImage()) {
        img.saveImg(parser.getImgNameFromUser());
    }

    runChanges();
}

void App::runChanges() {

    while (parser.checkIfUserWantsToMakeChangeToTheImage()) {

        printSizeReminder();
        int change = parser.getTheDesiredChangeFromUser();

        switch (change) {
            case ChangeType::background_img: {
                cv::Mat bg = std::move(parser.getPicFromUser(ImgType::bg_img));
                img.setImage(ImgType::bg_img, std::move(bg));
                break;
            }
            case ChangeType::resize_bg: {
                printSizeReminder();
                auto scalePercent = parser.getScalePercentForNewImgSizeFromUser(ImgType::bg_img);
                img.scaleImg(ImgType::bg_img, scalePercent);
                break;
            }
            case ChangeType::foreround_img: {
                cv::Mat fg = std::move(parser.getPicFromUser(ImgType::fg_img));
                img.setImage(ImgType::fg_img, std::move(fg));
                cv::Rect r = getFgPatchRectFromUser();
                img.setFgPatchRect(r);
                break;
            }
            case ChangeType::resize_fg: {
                printSizeReminder();
                auto scalePercent = parser.getScalePercentForNewImgSizeFromUser(ImgType::fg_img);
                img.scaleImg(ImgType::fg_img, scalePercent);
                cv::Rect r = getFgPatchRectFromUser();
                img.setFgPatchRect(r);
                break;
            }
            case ChangeType::fg_location_xy: {
                printLocation();
                img.setFgLocationType(parser.getFgLocationTypeFromUser());
                img.setFgLocation(parser.getFgLocationFromUser(img.getImgSize(ImgType::bg_img)));
                break;
            }
            case ChangeType::fg_location_select: {
                img.setFgLocationType(LocationType::center);
                std::cout << "\nSelect the desired area on the background image where you would like to place the foreground image \n" << std::endl;
                img.setFgLocation(getFgLocationFromUser());
                break;
            }
            default:
                break;
        }

        while (!img.generateImg()) {
            std::cout << "\nThe selected area is invalid, the patch goes out of the background, please try again \n" << std::endl;
            img.setFgLocationType(LocationType::center);
            img.setFgLocation(getFgLocationFromUser());
        }

        std::cout << "\nPress any key to continue" << std::endl;
        cv::waitKey(0);
        if (parser.checkIfUserWantsToSaveTheImage()) {
            img.saveImg(parser.getImgNameFromUser());
        }
    }
}

void App::printSizeReminder() const {

    cv::Size bgSize = img.getImgSize(ImgType::bg_img);
    cv::Size fgSize = img.getImgSize(ImgType::fg_p);
    std::cout << "\nNote youre cuurent background image size is height: " << bgSize.height << ", width: " << bgSize.width << std::endl;
    std::cout << "youre cuurent foreground patch size is height: " << fgSize.height << ", width: " << fgSize.width << "\n" << std::endl;
}

void App::printLocation() const {

    std::string lt = img.getFgLocationType();
    std::pair<int, int> point = img.getFgLocation();
    std::cout << "\nThe " << lt << " current location is at (" << point.first << ", " << point.second << ")\n" << std::endl;
}
