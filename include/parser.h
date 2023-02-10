
#ifndef CPPND_CAPSTONE_PROJECT_PARSER_H
#define CPPND_CAPSTONE_PROJECT_PARSER_H

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#include "image.h"

class Parser {

    YAML::Node config;

public:
    Parser();

    cv::Rect getFgPatchRectFromConfig() const;
    cv::Mat getPicFromUser(const ImgType imgType) const;
    std::pair<int, int> getFgLocationFromUser(const cv::Size bgSize) const;     // specified (x, y)
    LocationType getFgLocationTypeFromUser() const;
    cv::Size getNewImgSizeFromUser(const ImgType imgType) const;
    int getScalePercentForNewImgSizeFromUser(const ImgType imgType) const;
    static int getTheDesiredChangeFromUser();
    std::string getImgNameFromUser() const;

    bool checkIfUserWantsToResizeImage(const ImgType imgType) const;
    bool checkIfUserWantsToSaveTheImage() const;
    bool checkIfUserWantsToMakeChangeToTheImage() const;

private:
    std::string getStrArgFromConfig(const std::string arg) const;
    int getIntArgFromConfig(const std::string arg) const;

};

#endif //CPPND_CAPSTONE_PROJECT_PARSER_H
