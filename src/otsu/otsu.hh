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
typedef float (*t_sigmaComp)(float **h, int *threshold, int level);

class Otsu
{
    public:
        Otsu();
        Otsu(const cv::Mat& img, int nClass);
        template<typename T>
        Otsu(const std::vector<std::vector<T>>& img, int nClass);
        void operator()(const cv::Mat& img, int nClass);
        template<typename T>
        void operator()(const std::vector<std::vector<T>>& img, int nClass);
        cv::Mat getResult();
        std::vector<std::vector<int>> getMap();
private:
        void otsuProcess(const cv::Mat& img);

        float sigmaComputation(std::vector<int>& thresholds,std::vector<std::vector<float>> H);
        void sigmaCodeGeneration();
        void libGeneration();
        t_sigmaComp libLoad();
        void segmentImg(const std::vector<int>& thresholds);

        void buildLookUpTables(std::vector<std::vector<float>>& P, std::vector<std::vector<float>>& S, std::vector<std::vector<float>>& H, std::vector<float>& h);
        void buildHistogram(std::vector<float>& h, const std::vector<std::vector<float>>& Vmap);

        cv::Mat output;
        int nClass;
        std::vector<std::vector<int>> segMap;

};

#endif