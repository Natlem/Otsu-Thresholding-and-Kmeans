#include "otsu.hh"

float Otsu::sigmaComputation(std::vector<int>& thresholds)
{
    std::vector<std::vector<float>> H;
    float sigma = 0;
    sigmaCodeGeneration();
    this->libGeneration();
    t_sigmaComp sigmaFunc = this->libLoad();
    sigma = sigmaFunc(H, thresholds, this->nClass);
    return sigma;
}

void Otsu::sigmaCodeGeneration()
{
        std::string header("#include <vector>\n");
        std::string prototype("float "FUNC2GEN"(std::vector<std::vector<float>> H, std::vector<int>& threshold, int level)\n");
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

        std::ofstream output (FILE2GEN(".cc"), std::ofstream::out | std::ofstream::trunc);
        output << code;
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
        char *compilerPath = "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\bin\\cl.exe";
        TCHAR* compilerPathT= A2T(compilerPath);

        char *compilerArg = "cl.exe /c "FILE2GEN(".cc");
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

        char *linkerPath = "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\bin\\link.exe";
        TCHAR* linkerPathT= A2T(linkerPath);

        char *linkerArg = "link.exe /DLL /OUT:"FILE2GEN(".dll")" "FILE2GEN(".obj");
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
        std::system("g++ -shared -fPIC -o "FILE2GEN(".so")" "FILE2GEN(".cc"));
    #endif
}

t_sigmaComp Otsu::libLoad()
{
    #ifdef _WIN32
        HMODULE sigma_dll = LoadLibrary(FILE2GEN(".dll"));
        if (!sigma_dll)
        {
            std::cerr << "Problem loading shared lib " << std::endl;
            std::exit(1);
        }
        FARPROC tempProc = GetProcAddress(sigma_dll, FUNC2GEN);
        if (!tempProc)
        {
            std::cerr << "Problem loading function in lib " << std::endl;
            std::exit(1);
        }
        t_sigmaComp sigmaCompFunc = (t_sigmaComp)tempProc;
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
            h[Vmap[row][col]]++;
        }
    }
}

void Otsu::otsuProcess(cv::Mat& img)
{
    //We assume that img is in HSV colorspace
    
    std::vector<std::vector<float>> Vmap(img.row, std::vector<float>(img.col));

    for (auto i = 0; i < img.row; ++i)
        for (auto j = 0; j < img.col; ++j)
            Vmap[i][j] = img.at<cv::Vec3b>()[2];

    std::vector<float> histo(256,0);
    this->buildHistogram(histo, Vmap);

    std::vector<std::vector<float>> P(NGRAY,std::vector<float>(NGRAY));
    std::vector<std::vector<float>> S(NGRAY,std::vector<float>(NGRAY));
    std::vector<std::vector<float>> H(NGRAY,std::vector<float>(NGRAY));
    this->buildLookUpTables(P, S, H, histo);

    
    std::vector<int> thresholds(this->nClass);
    float maxSig = sigmaComputation(thresholds);

}