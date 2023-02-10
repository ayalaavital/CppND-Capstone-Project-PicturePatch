
#include "parser.h"

Parser::Parser() {
    config = YAML::LoadFile("../config/exp.yaml");
}

std::string Parser::getStrArgFromConfig(const std::string arg) const {

    std::string val;

    try {
        val = config[arg].as<std::string>();
    } catch(const YAML::BadFile& e) {
        std::cerr << e.msg << std::endl;
    } catch(const YAML::ParserException& e) {
        std::cerr << e.msg << std::endl;
    } catch(...) {}
    return val;
}

int Parser::getIntArgFromConfig(const std::string arg) const {

    int val = -1;

    try {
        val = config[arg].as<int>();
    } catch(const YAML::BadFile& e) {
        std::cerr << e.msg << std::endl;
    } catch(const YAML::ParserException& e) {
        std::cerr << e.msg << std::endl;
    } catch(...) {}
    return val;
}

cv::Rect Parser::getFgPatchRectFromConfig() const {

    int x = getIntArgFromConfig("fgX");
    int y = getIntArgFromConfig("fgY");
    int width = getIntArgFromConfig("fgWidth");
    int height = getIntArgFromConfig("fgHeight");
    if (x == -1 || y == -1 || width == -1 || height == -1) {
        return cv::Rect();
    }
    return cv::Rect(x, y, width, height);
}

cv::Mat Parser::getPicFromUser(const ImgType imgType) const {

    std::string imgName = Image::imgTypeToString(imgType);
    std::string path;
    if (imgType == ImgType::bg_img) {
        path = getStrArgFromConfig("backgroundPath");
    } else {
        path = getStrArgFromConfig("foregroundPath");
    }

    if (path.empty()) {
        std::cout << "Enter the path to your " << imgName << " picture:" << std::endl;
        std::cin >> path;
    } else {
        std::cout << "\nThe config file defines the path to the " << imgName << " image" << std::endl;
    }

    cv::Mat image = cv::imread(path);
    while (image.empty()) {
        std::cin.clear();
        std::cout << "\nIncorrect format entered, please re-enter image path:" << std::endl;
        std::cin >> path;
        image = cv::imread(path);
    }

    cv::Size s = image.size();
    std::cout << "\nNote your " << imgName << " size is height: " << s.height << ", width: " << s.width << std::endl;

    return image;
}

std::pair<int, int> Parser::getFgLocationFromUser(const cv::Size bgSize) const {

    int row = getIntArgFromConfig("fgLocationY");
    int col = getIntArgFromConfig("fgLocationX");
    if (row == -1 || col == -1) {
        std::cout << "\nEnter two numbers that indicate the desired position for placing the image in the background: \n"
                     "For example 0 0  represent the Top left corner. "
                     "The width should be in range [0, " << bgSize.width << "]\n"
                     "The height should be in range [0, " << bgSize.height << "]"
                  << std::endl;
        std::cin >> col;
        std::cin >> row;
    } else {
        std::cout << "\nThe config file determines the location of the foreground image." << std::endl;
    }

    while (row < 0 || row > bgSize.height || col < 0 || col > bgSize.width) {
        std::cin.clear();
        std::cout << "\nThe given point is outside the image boundaries please try"
                     " again" << std::endl;
        std::cin >> col;
        std::cin >> row;
    }
    return std::pair<int, int>(col, row);
}

LocationType Parser::getFgLocationTypeFromUser() const {

    std::string input;
    input = getStrArgFromConfig("fgLocationType");

    if (input.empty()) {
        std::cout << "\nEnter the desired type of foreground location from the following options: \n"
                     "TL: top left \n"
                     "BL: button left \n"
                     "TR: top right \n"
                     "BR: button right \n"
                     "C: center \n"
                     "For example if the foreground location 0 0 then the type will "
                     "be TL - top left corner." << std::endl;
        std::cin >> input;
    } else {
        std::cout << "\nThe type of foreground location is specified by the value in the config file." << std::endl;
    }

    while (input != "TL" && input != "BL" && input != "TR" && input != "BR" && input != "C"){
        std::cout << "Incorrect input, please try again" << std::endl;
        std::cin >> input;
    }

    if (input == "TL") {
        return LocationType::top_left;
    } else if (input == "BL") {
        return LocationType::button_left;
    } else if (input == "TR") {
        return LocationType::top_right;
    } else if (input == "BR") {
        return LocationType::button_right;
    }
    return LocationType::center;
}

cv::Size Parser::getNewImgSizeFromUser(const ImgType imgType) const {

    int height, width;
    std::cout << "Enter the desired dimensions for the " << Image::imgTypeToString(imgType)
              << " image: height width" << std::endl;
    std::cin >> height;
    std::cin >> width;
    while ( 0 > height || 0 > width) {
        std::cout << "Incorrect input, please try again:" << std::endl;
        std::cin >> height;
        std::cin >> width;
    }

    return cv::Size(width, height);
}

int Parser::getScalePercentForNewImgSizeFromUser(const ImgType imgType) const {

    std::string imgName = Image::imgTypeToString(imgType);
    int input = -1;
    if (imgType == ImgType::bg_img) {
        input = getIntArgFromConfig("bgScalePercent");
    } else {
        input = getIntArgFromConfig("fgScalePercent");
    }
    if (input == -1) {
        std::cout << "\nEnter the scale percentage for the " << imgName << " image: \nFor example, entering 200 will double the size." << std::endl;
        std::cin >> input;
    } else {
        std::cout << "\nThe config file defines the scale percentage for the " << imgName << " image" << std::endl;
    }

    while ( 0 > input) {
        std::cout << "Incorrect input, please try again:" << std::endl;
        std::cin >> input;
    }

    return input;
}

int Parser::getTheDesiredChangeFromUser() {

    int input;
    std::cout << "\nEnter a number that corresponds to the desired change, according to the following table: \n"
                 "0: Change the background image \n"
                 "1: Resize the background image \n"
                 "2: Change the foreground image \n"
                 "3: Resize the foreground image \n"
                 "4: Change the position of the foreground image by entering coordinates \n"
                 "5: Change the position of the foreground image by selecting a position on the background image \n"
              << std::endl;
    std::cin >> input;
    while ( 0 > input || input > 5) {
        std::cout << "Incorrect input, please try again:" << std::endl;
        std::cin >> input;
    }

    return input;
}

std::string Parser::getImgNameFromUser() const {

    std::string input;
    input = getStrArgFromConfig("nameImg");

    if (input.empty()) {
        std::cout << "\nEnter the desired name for the merged image:" << std::endl;
        std::cin >> input;
    } else {
        std::cout << "\nThe config file determines the name of the image." << std::endl;
    }

    return input;
}

bool Parser::checkIfUserWantsToResizeImage(const ImgType imgType) const {

    std::string imgName = Image::imgTypeToString(imgType);
    std::string input;
    if (imgType == ImgType::bg_img) {
        input = getStrArgFromConfig("resizeBgImg");
    } else {
        input = getStrArgFromConfig("resizeFgImg");
    }

    if (input.empty()) {
        std::cout << "Would you like to resize the " << imgName << " image? Y/N" << std::endl;
        std::cin >> input;
    } else {
        std::cout << "\nThe size adjustment for the " << imgName << " image was determined based on the information in the config file.." << std::endl;
    }

    while (input != "N" && input != "Y") {
        std::cout << "Incorrect input, please enter Y for yes or N for no:" << std::endl;
        std::cin >> input;
    }

    return input == "Y";
}

bool Parser::checkIfUserWantsToSaveTheImage() const {

    std::string input;
    input = getStrArgFromConfig("saveImg");

    if (input.empty()) {
        std::cout << "\nWould you like to save the merged image? Y/N" << std::endl;
        std::cin >> input;
    } else {
        std::cout << "\nThe config file defines whether the image will be saved or not." << std::endl;
    }

    while (input != "N" && input != "Y") {
        std::cout << "Incorrect input, please enter Y for yes or N for no:" << std::endl;
        std::cin >> input;
    }
    return input == "Y";
}

bool Parser::checkIfUserWantsToMakeChangeToTheImage() const {

    std::string input;
    input = getStrArgFromConfig("makeChange");

    if (input.empty()) {
        std::cout << "\nWould you like to make a change to the image? Y/N" << std::endl;
        std::cin >> input;
    } else {
        std::cout << "\nThe config file determines whether changes will be made to the image." << std::endl;
    }

    while (input != "N" && input != "Y") {
        std::cout << "Incorrect input, please enter Y for yes or N for no:" << std::endl;
        std::cin >> input;
    }
    return input == "Y";
}
