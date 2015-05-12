#include <opencv2/opencv.hpp>
#include <string>
#include "../otsu/otsu.hh"

int main(int argc, char **argv)
{
    int numOfRegion = std::stoi(argv[2]); 
    cv::Mat img = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    cv::cvtColor(img,img,CV_BGR2HSV);

    Otsu o(img, numOfRegion);
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::Mat image = o.getResult();
    cv::imshow( "Display window", img );                   // Show our image inside it.
   
    cv::waitKey(0);
    cv::imwrite(std::string("result") + std::to_string(numOfRegion) + ".jpg", img);
    return 0;
}
