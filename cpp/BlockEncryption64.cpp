#include<iostream>
#include <fstream>
#include<vector>
#include <sys/stat.h> 
#include<nlohmann/json.hpp>
#include <omp.h>
#include <boost/multiprecision/cpp_int.hpp>

using json = nlohmann::json;
using namespace boost::multiprecision;
// using namespace std;
class Point{
    public: int intX;
    public: int intY;
    public: unsigned long int floatX;
    public: unsigned long int floatY;
    public: Point(){

    }
    public: Point(int intX,int  floatX,int intY,int floatY){
        this->intX = intX;
        this->floatX = floatX;
        this->intY = intY;
        this->floatY = floatY;
    }
};
long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}
std::vector<Point> generateVector(){
    std::vector<Point> points;
    for(int i=0;i<100000;i++)
        points.push_back(Point(1,2,3,4));
    std::cout<<"size "<<points.size();
    return points;
}
class Encryption{
    json CMT;
    public: Encryption(json CMT){
        this->CMT = CMT;
    }
    public: std::vector<Point> encryptByteArray(unsigned long int bytes[],int size){
            std::vector<Point> points;
            for(int i=0;i<size;i++)
                points.push_back(Point());
            int width = (int)(this->CMT["config"]["width"]);
            int FPPB = (int)(this->CMT["config"]["FPPB"]);
            //int FPPDigits = (int)(this->CMT["config"]["FPPDigits"]);
            int xmax = (this->CMT["data"]).size() * width - width;
            int ymax = (this->CMT["data"]["0"]).size() * width -width;
            int keySize = FPPB * 2;
            uint128_t totalChars = (uint128_t)pow(cpp_int(2),FPPB);
            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_int_distribution<> distrX(0, xmax);
            std::uniform_int_distribution<> distrY(0, ymax); 
            std::uniform_int_distribution<> distrFloat(0 , pow(2,16));
            #pragma omp parallel for
            for(int i = 0 ;i<size;i++){
                unsigned long int byte = bytes[i];    
                int intX = distrX(eng);
                int intY = distrY(eng);
                unsigned long int floatX = distrFloat(eng);
                unsigned long int floatY = distrFloat(eng);
                std::string baseX = std::to_string(intX - (intX % width));
                std::string baseY = std::to_string(intY- (intY % width));
                uint128_t key = (uint128_t)((this->CMT["data"][baseX][baseY]).get<std::string>());         
                uint128_t floatXY = ( ((uint128_t)(floatX)) << FPPB ) +floatY;
                uint128_t floatXYXor = ( key ^ floatXY);
                uint128_t beta = (uint128_t)((floatXYXor) % (totalChars));
                uint128_t diff=0;
                if(beta > byte){
                    diff = (uint128_t)(totalChars - beta);
                    diff+= byte;
                }
                else{
                    diff = byte-beta;
                }
                floatXYXor = beta + diff;
                uint128_t newFloatXY = (uint128_t)(floatXYXor ^ key);
                unsigned long int newFloatX = (unsigned long int)(newFloatXY >> FPPB);
                unsigned long int newFloatY = (unsigned long int)(newFloatXY & (unsigned long int)(totalChars-1));
                points[i].intX=intX;
                points[i].floatX=newFloatX;
                points[i].intY=intY;
                points[i].floatY=newFloatY;
            }
            return points;
    }
    public: unsigned long int* decryptToByteArray(std::vector<Point> encrypted){
        int FPPB = (int)(this->CMT["config"]["FPPB"]);
        int width = (int)(this->CMT["config"]["width"]);
        uint128_t totalChars = (uint128_t)pow(cpp_int(2),FPPB);
        unsigned long int *bytes = new unsigned long int[encrypted.size()];
        #pragma omp parallel for
        for(int i=0;i<encrypted.size();i++){ 
            unsigned int intX,intY;
            unsigned long int floatX,floatY;
            uint128_t floatXY = 0;
            uint128_t key;
            std::string baseX,baseY;      
            intX = encrypted[i].intX;
            intY = encrypted[i].intY;
            baseX = std::to_string(intX - (intX % width));
            baseY = std::to_string(intY- (intY % width));
            key = (uint128_t)((this->CMT["data"][baseX][baseY]).get<std::string>());
            floatX = (encrypted[i].floatX);
            floatY = (encrypted[i].floatY);
            floatXY = ( ((uint128_t)(floatX)) << FPPB ) +floatY;
            bytes[i] = (unsigned long int)((floatXY ^ key)  % totalChars) ;
        }
        return bytes;
    }
};
int main(){
    std::ifstream i("../CMT64.json");
    json j;
    i >> j;
    std::cout<<pow(2,64);
    Encryption enc = Encryption(j);
    std::vector<Point> encrypted;
    std::ifstream fstr;
    std::string path="../";
    std::string filename = "Data256.bin";
    int fileSize = GetFileSize(path+filename);
    unsigned long int *bytes = new unsigned long int[fileSize/8];
    fstr.open(path+filename, std::ios::binary);
    unsigned long int block = 0;
    for(int i=0;i<fileSize/8;i++){
        fstr.read(reinterpret_cast<char*>(&block), 8);
        bytes[i] = 0;
        bytes[i] = block;
    }
    double start, end;
    start = omp_get_wtime();
    encrypted = enc.encryptByteArray(bytes,fileSize/8);
    end = omp_get_wtime();
    double delta = end-start;
    std::cout<<"Time to encrypt "<<fileSize<<" Bytes: "<<delta<<std::endl;
    unsigned long int *decrypted;
    start = omp_get_wtime();
    decrypted = enc.decryptToByteArray(encrypted);
    end = omp_get_wtime();
    delta = end-start;
    std::cout<<"Time to decrypt to "<<fileSize<<" Bytes: "<<delta<<std::endl;
    std::ofstream file;
    file.open(filename, std::ios_base::binary);
    assert(file.is_open());
    for(int i = 0; i < fileSize/8; ++i)
        file.write((char*)(decrypted + i), sizeof(unsigned long int));
    file.close();
    return 0;
 }