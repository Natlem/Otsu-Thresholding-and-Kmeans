#include <opencv2\opencv.hpp>
#include "../otsu/otsu.hh"

int main(int argc, char **argv)
{
    cv::Mat img = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    cv::cvtColor(img,img,CV_BGR2HSV);
    Otsu o(img, 2);
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::Mat image = o.getResult();
    cv::imshow( "Display window", image );                   // Show our image inside it.

    cv::waitKey(0);

    return 0;
}