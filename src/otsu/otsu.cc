#include "otsu.hh"

float Otsu::sigmaComputation(std::vector<float>& thresholds)
{
    if (this->nClass <= 5)
    {
        //Normal thing here
    }
    else
    {
        sigmaCodeGeneration();
    }
}

void Otsu::sigmaCodeGeneration()
{
        std::string NGRAY="256";
        std::string header("#include <vector>\n");
        std::string prototype("float sigmaComp(std::vector<std::vector<float>> H, std::vector<int>& threshold, int level)\n");
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
                code = code + "for (int " + index[i] + " = 1; " + index[i] + " < " +  NGRAY + "-" + std::to_string(nClass) + "; " + index[i] + "++)\n";
            else
                code = code + "for (int " + index[i] + " = " + index[i - 1] + " + 1; "  + index[i] + " < " +  NGRAY + "-" + std::to_string(nClass) + " + " + std::to_string(i) + "; " + index[i] + "++)\n";
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

        std::ofstream output ("./sigmaComputation.cc");
        output << code;
        output.close();
}