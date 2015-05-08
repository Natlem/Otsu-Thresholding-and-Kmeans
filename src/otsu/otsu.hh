#ifndef OTSU_HH
# define OTSU_HH

#ifdef _WIN32
    #include <windows.h>
#elif __linux
    #include <dlfcn.h>
#elif __unix // all unices not caught above
    #include <dlfcn.h>
#elif __posix
    #include <dlfcn.h>
#endif

#include <opencv2\highgui\highgui.hpp>
#include <vector>
#include <iostream>
#include <fstream>

class Otsu
{
    public:
        Otsu();
        Otsu(const cv::Mat& grayMap, int nClass);
        template<typename T>
        Otsu(const std::vector<std::vector<T>>& grayMap, int nClass);
        void operator()(cv::Mat& grayMap, int nClass);
        template<typename T>
        void operator()(const std::vector<std::vector<T>>& grayMap, int nClass);
    private:
        float sigmaComputation(std::vector<float>& thresholds);
        void sigmaCodeGeneration();
        int nClass;
};

#endif