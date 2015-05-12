#include "otsu.hh"


Otsu::Otsu()
{

}

Otsu::Otsu(const cv::Mat& img, int nClass)
    :output(img),nClass(nClass)
{
    this->otsuProcess(img);
}

void Otsu::operator()(const cv::Mat& img, int nClass)
{
    this->nClass = nClass;
    this->otsuProcess(img);
}

float sigmaComp1(std::vector<std::vector<float>> H, std::vector<int>& threshold, int level)
{
    float maxSig = 0;
    for (int a = 1; a < 256-2; a++)
    {
        float Sq = H[1][a] + H[a + 1][255];
        if (maxSig < Sq)
        {
            threshold[0] = a;
            maxSig = Sq;
        }
    }
    return maxSig;
}


float Otsu::sigmaComputation(std::vector<int>& thresholds,std::vector<std::vector<float>> H)
{
    float sigma = 0;
    sigmaCodeGeneration();
    this->libGeneration();
    t_sigmaComp sigmaFunc = this->libLoad();
    sigma = sigmaFunc(H, thresholds, this->nClass);
    return sigma;
}

void Otsu::sigmaCodeGeneration()
{
        std::string header("#ifdef _WIN32\n #define EXTERN_DLL_EXPORT extern \"C\" __declspec(dllexport)\n#endif\n#include <vector>\n");
        std::string prototype("EXTERN_DLL_EXPORT float "FUNC2GEN"(std::vector<std::vector<float>> H, std::vector<int>& threshold, int level)\n");
        std::string code(header + prototype);
        code.append("{\n");
        code.append("float maxSig = 0;\n");

        std::vector<char> index;
        int j = 0;
        for (char c = 'a';c < 'z' && j < nClass - 1; ++c, ++j)
            index.push_back(c);

        // For loop generation
        for (int i = 0; i < nClass - 1; ++i)
        {
            if (i == 0)
                code = code + "for (int " + index[i] + " = 1; " + index[i] + " < " +  std::to_string(NGRAY) + "-" + std::to_string(nClass) + "; " + index[i] + "++)\n";
            else
                code = code + "for (int " + index[i] + " = " + index[i - 1] + " + 1; "  + index[i] + " < " +  std::to_string(NGRAY) + "-" + std::to_string(nClass) + " + " + std::to_string(i) + "; " + index[i] + "++)\n";
        }
        code.append("{\n");
        code.append("float Sq = H[1][a]" );
        for (int i = 0; i < nClass - 2; ++i)
        {
            code = code + " + H[" + index[i] + " + 1]["  + index[i + 1] + "]"; 
        }
        code = code + " + H[" + index[index.size() - 1] + " + 1][255];\n";

        code = code + "if (maxSig < Sq)\n";
        code.append("{\n");

        for (int i = 0; i < nClass - 1 ; ++i)
        {
            code = code + "threshold[" + std::to_string(i) + "] = " + index[i] + ";\n"; 
        }
        code.append("maxSig = Sq;\n");

        code.append("}\n");

        code.append("}\n");
        code.append("return maxSig;\n");
        code.append("}\n");

        std::ofstream output;
        output.open(FILE2GEN(.cc), std::ofstream::out | std::ofstream::trunc);
        if (output.is_open())
        {
            output << code;
        }
        else
        {
            std::cerr << "Problem creating sigma computation file" << std::endl;
            std::exit(1);
        }

        output.close();
}

void Otsu::libGeneration()
{
    #ifdef _WIN32
        STARTUPINFO compilerInfo;
        PROCESS_INFORMATION compilerProcessInfo;

        ZeroMemory( &compilerInfo, sizeof(compilerInfo) );
        compilerInfo.cb = sizeof(compilerInfo);
        ZeroMemory( &compilerProcessInfo, sizeof(compilerProcessInfo) );

        USES_CONVERSION;
        #ifdef _WIN64
            char *compilerPath = "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\bin\\amd64\\cl.exe";
        #else
            char *compilerPath = "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\bin\\cl.exe";
        #endif
        TCHAR* compilerPathT= A2T(compilerPath);

        char *compilerArg = "cl.exe /c "FILE2GEN(.cc);
        TCHAR* compilerArgT= A2T(compilerArg);

        // Start the child process. 
        if( !CreateProcess( compilerPathT,
            compilerArgT,        
            NULL,           
            NULL,           
            FALSE,          
            0,              
            NULL,           
            NULL,           
            &compilerInfo,            
            &compilerProcessInfo )           
        ) 
        {
            printf( "CreateProcess failed (%d).\n", GetLastError() );
            std::exit(1);
        }

        WaitForSingleObject( compilerProcessInfo.hProcess, INFINITE );
        CloseHandle( compilerProcessInfo.hProcess );
        CloseHandle( compilerProcessInfo.hThread );

        STARTUPINFO linkerInfo;
        PROCESS_INFORMATION processLinkerInfo;

        ZeroMemory( &linkerInfo, sizeof(linkerInfo) );
        linkerInfo.cb = sizeof(linkerInfo);
        ZeroMemory( &processLinkerInfo, sizeof(processLinkerInfo) );

        #ifdef _WIN64
            char *linkerPath = "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\bin\\amd64\\link.exe";
        #else
            char *linkerPath = "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\bin\\link.exe";
        #endif
        TCHAR* linkerPathT= A2T(linkerPath);

        char *linkerArg = "link.exe /DLL /OUT:"FILE2GEN(.dll)" "FILE2GEN(.obj);
        TCHAR* linkerArgT= A2T(linkerArg);

        // Start the child process. 
        if( !CreateProcess( linkerPathT,   // No module name (use command line)
            linkerArgT,        // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block   
            NULL,           // Use parent's starting directory 
            &linkerInfo,            // Pointer to STARTUPINFO structure
            &processLinkerInfo )           // Pointer to PROCESS_INFORMATION structure
            ) 
        {
            printf( "CreateProcess failed (%d).\n", GetLastError() );
            std::exit(1);
        }

        WaitForSingleObject( processLinkerInfo.hProcess, INFINITE );



        // Close process and thread handles. 
        CloseHandle( processLinkerInfo.hProcess );
        CloseHandle( processLinkerInfo.hThread );

    #else
        std::system("g++ -shared -fPIC -o "FILE2GEN(.so)" "FILE2GEN(.cc));
    #endif
}

t_sigmaComp Otsu::libLoad()
{
    #ifdef _WIN32
    char *test = FILE2GEN(.dll); 
        HMODULE sigma_dll = LoadLibrary(FILE2GEN(.dll));
        if (!sigma_dll)
        {
            int error = GetLastError();
            std::cerr << "Problem loading shared lib " << std::endl;
            std::cerr << "Exiting with error " << error << std::endl;
            std::exit(error);
        }
        FARPROC tempProc = GetProcAddress(sigma_dll, FUNC2GEN);
        if (!tempProc)
        {
            int error = GetLastError();
            std::cerr << "Problem loading function in lib " << std::endl;
            std::cerr << "Exiting with error " << error << std::endl;
            std::exit(error);
        }
        t_sigmaComp sigmaCompFunc = reinterpret_cast<t_sigmaComp>(tempProc);
        return sigmaCompFunc;
    #else
        void* sigma_so = dlopen(FILE2GEN(".so"), RTLD_LAZY);
        if (!sigma_so)
        {
            std::cerr << "Problem loading shared lib " << std::endl;
            std::exit(1);
        }
        void* tempSigma = dlsym(sigma_so, FUNC2GEN);
        if (!tempSigma)
        {
            std::cerr << "Problem loading function in lib " << std::endl;
            std::exit(1);
        }
        t_sigmaComp sigmaCompFunc = (t_sigmaComp)tempSigma;
    #endif
}

void Otsu::buildLookUpTables(std::vector<std::vector<float>>& P, std::vector<std::vector<float>>& S, std::vector<std::vector<float>>& H, std::vector<float>& h)
{

    for (int i=1; i < NGRAY; ++i)
    {
        P[i][i] = h[i];
        S[i][i] = ((float) i)*h[i];
    }
    
    for (int i=1; i < NGRAY-1; ++i)
    {
        P[1][i+1] = P[1][i] + h[i+1];
        S[1][i+1] = S[1][i] + ((float) (i+1))*h[i+1];
    }
    
    for (int i=2; i < NGRAY; i++)
        for (int j=i+1; j < NGRAY; j++)
        {
            P[i][j] = P[1][j] - P[1][i-1];
            S[i][j] = S[1][j] - S[1][i-1];
        }
        
    for (int i=1; i < NGRAY; ++i)
        for (int j=i+1; j < NGRAY; j++)
        {
            if (P[i][j] != 0)
                H[i][j] = (S[i][j]*S[i][j])/P[i][j];
            else
                H[i][j] = 0.f;
        }
}

void Otsu::buildHistogram(std::vector<float>& h, const std::vector<std::vector<float>>& Vmap)
{
    for (auto row = 0; row < Vmap.size(); ++row)
    {
        for (auto col = 0; col < Vmap[0].size(); ++col)
        {
            h[static_cast<int>(Vmap[row][col])] += 1.0;
        }
    }
}

void Otsu::otsuProcess(const cv::Mat& img)
{
    //We assume that img is in HSV colorspace
    
    std::vector<std::vector<float>> Vmap(img.rows, std::vector<float>(img.cols));

    for (auto i = 0; i < img.rows; ++i)
        for (auto j = 0; j < img.cols; ++j)
            Vmap[i][j] = img.at<cv::Vec3b>(i,j)[2];

    std::vector<float> histo(256,0);
    this->buildHistogram(histo, Vmap);

    std::vector<std::vector<float>> P(NGRAY,std::vector<float>(NGRAY));
    std::vector<std::vector<float>> S(NGRAY,std::vector<float>(NGRAY));
    std::vector<std::vector<float>> H(NGRAY,std::vector<float>(NGRAY));
    this->buildLookUpTables(P, S, H, histo);

    
    std::vector<int> thresholds(this->nClass);
    sigmaComputation(thresholds, H);
    segmentImg(thresholds);

}

void Otsu::segmentImg(const std::vector<int>& thresholds)
{
    segMap = std::vector<std::vector<int>>(output.rows, std::vector<int>(output.cols));
    for (auto i = 0; i < output.rows; ++i)
    {
        for (auto j = 0; j < output.cols; ++j)
        {
            int vValue = static_cast<int>(output.at<cv::Vec3b>(i,j)[2]);
            for (auto k = 0; k < this->nClass; ++ i)
            {
                if (k < this->nClass)
                {
                    if (vValue < thresholds[k + 1] && vValue > thresholds[k])
                    {
                        segMap[i][j] = k;
                        output.at<cv::Vec3b>(i,j)[2] = vValue;
                    }
                }
                else
                {
                    if (vValue > thresholds[k])
                    {
                        segMap[i][j] = k;
                        output.at<cv::Vec3b>(i,j)[2] = vValue;
                    }
                }
            }
        }
    }
}

cv::Mat Otsu::getResult()
{
    return this->output;
}

std::vector<std::vector<int>> Otsu::getMap()
{
    return this->segMap;
}