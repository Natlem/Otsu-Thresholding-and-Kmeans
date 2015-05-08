#ifndef OTSU_HH
# define OTSU_HH

#ifdef _WIN32
    #include <windows.h>
    #include <atlstr.h>
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
#include <cstdlib>

#define NGRAY 256
#define FILE2GEN(ext) "sigmaComputation"#ext
#define FUNC2GEN "sigmaComp"
typedef float (*t_sigmaComp)(std::vector<std::vector<float>> H, std::vector<int>& threshold, int level);

class Otsu
{
    public:
        Otsu();
        Otsu(const cv::Mat& img, int nClass);
        template<typename T>
        Otsu(const std::vector<std::vector<T>>& img, int nClass);
        void operator()(cv::Mat& img, int nClass);
        template<typename T>
        void operator()(const std::vector<std::vector<T>>& img, int nClass);
    private:
        void otsuProcess(cv::Mat& img);

        float sigmaComputation(std::vector<int>& thresholds);
        void sigmaCodeGeneration();
        void libGeneration();
        t_sigmaComp libLoad();

        void buildLookUpTables(std::vector<std::vector<float>>& P, std::vector<std::vector<float>>& S, std::vector<std::vector<float>>& H, std::vector<float>& h);
        void buildHistogram(std::vector<float>& h, const std::vector<std::vector<float>>& Vmap);

        int nClass;
};

#endif