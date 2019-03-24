#include<iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <stdio.h>
// using namespace std;

class TestSuite{
    void createBinFile(std::string filename, int size){
        std::ofstream ofs(filename, std::ios::binary | std::ios::out);
        ofs.seekp((size) - 1);
        ofs.write("", 1);
    }
    void createFiles(int maxFileSize){
        for(int i = 1; i<=maxFileSize*1024; i*=10){
            createBinFile(std::string("./Data/Data") + std::to_string(i) + ".bin", i*1024);
        }
    }
    bool compareFile(std::string filename1, std::string filename2) {
        FILE *fp1, *fp2;
        char ch1, ch2;
        fp1 = fopen(filename1.c_str(),  "r");
        fp2 = fopen(filename2.c_str(), "r");
        if ((fp1 != NULL) && (fp2 != NULL))
        {
            int flag = 0;
            while (((ch1 = fgetc(fp1)) != EOF) &&((ch2 = fgetc(fp2)) != EOF))
            {
                if (ch1 == ch2)
                {
                    flag = 1;
                    continue;
                }
                else
                {
                    fseek(fp1, -1, SEEK_CUR);        
                    flag = 0;
                    break;
                }
            }
            if (flag == 0)
                return false;
            else
                return true;
        }
    }
    int numberOfIterations(std::string filename){
        filename.erase(filename.begin(), filename.begin()+4);
        filename.erase(filename.end()-4, filename.end());
        return(1000000/stoi(filename));
    }
};
int main(){
    std::cout<<numberOfIterations("Data1.bin");
}
