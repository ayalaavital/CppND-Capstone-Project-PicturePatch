
#ifndef CPPND_CAPSTONE_PROJECT_APP_H
#define CPPND_CAPSTONE_PROJECT_APP_H

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#include "parser.h"
#include "image.h"


enum ChangeType {
    background_img,      //Change the background image
    resize_bg,           //Resize the background image
    foreground_img,      //Change the foreground image
    resize_fg,           //Resize the foreground image
    fg_location_xy,      // Change fgLocation by entering coordinates
    fg_location_select   // Change fgLocation by cv::selectROI
}


class App {

    Image img;
    Parser parser;

public:
    App() = default;

    void run();

private:
    void getPicturesFromUser();
    std::pair<int, int> getFgLocationFromUser() const;  // using selectROI
    cv::Rect getFgPatchRectFromUser() const ;

    void printSizeReminder() const;
    void printLocation() const;
    void runChanges();
};

#endif //CPPND_CAPSTONE_PROJECT_APP_H
