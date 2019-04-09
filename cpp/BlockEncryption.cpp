#include<iostream>
#include <fstream>
#include<vector>
#include <sys/stat.h> 
#include<nlohmann/json.hpp>
#include <omp.h>
#include "Testing/TestSuite.cpp"

using json = nlohmann::json;
using namespace std;
class Point{
    public: int intX;
    public: int intY;
    public: unsigned int floatX;
    public: unsigned int floatY;
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
vector<Point> generateVector(){
    vector<Point> points;
    for(int i=0;i<100000;i++)
        points.push_back(Point(1,2,3,4));
    cout<<"size "<<points.size();
    return points;
}
class Encryption{
    json CMT;
    public: Encryption(json CMT){
        this->CMT = CMT;
    }
    public: vector<Point> encryptByteArray(unsigned int bytes[],int size){
            string blank = "";
            vector<Point> points;
            for(int i=0;i<size;i++)
                points.push_back(Point());
            int width = (int)(this->CMT[blank+"config"][blank+"width"]);
            int FPPB = 32;
            int FPPBDigits = 10;
            int xmax = (this->CMT[blank+"data"]).size() * width - width;
            int ymax = (this->CMT[blank+"data"][blank+"0"]).size() * width -width;
            int keySize = FPPB * 2;
            unsigned long int totalChars = pow(2,FPPB);
            random_device rd;
            mt19937 eng(rd());
            uniform_int_distribution<> distrX(0, xmax);
            uniform_int_distribution<> distrY(0, ymax); 
            uniform_int_distribution<> distrFloat(0 , pow(2,16));
            #pragma omp parallel for
            for(int i = 0 ;i<size;i++){
                unsigned int byte = bytes[i];    
                int intX = distrX(eng);
                int intY = distrY(eng);
                unsigned int floatX = distrFloat(eng);
                unsigned int floatY = distrFloat(eng);
                string baseX = to_string(intX - (intX % width));
                string baseY = to_string(intY- (intY % width));
                unsigned long int key  =  std::stoul((string)(this->CMT[blank+"data"][blank+baseX][blank+baseY]));
                //std::cout<<key<<std::endl;
                unsigned long int floatXY = ( ((unsigned long int)(floatX)) << FPPB ) +floatY;
                unsigned long int floatXYXor = floatXY ^ key;
                unsigned long int beta = (unsigned long int)(floatXYXor) % (totalChars);
                unsigned long int diff=0;
                if(beta > byte){
                    diff = totalChars - beta;
                    diff+= byte;
                }
                else{
                    diff = byte-beta;
                }
                floatXYXor = floatXYXor + diff;
                unsigned long int newFloatXY = floatXYXor ^ key;
                unsigned int newFloatX = (newFloatXY >> FPPB);
                unsigned int newFloatY = (newFloatXY & (totalChars-1));
                points[i].intX=intX;
                points[i].floatX=newFloatX;
                points[i].intY=intY;
                points[i].floatY=newFloatY;
            }
            return points;
    }
    public: unsigned int* decryptToByteArray(vector<Point> encrypted){
        int FPPB = 32;
        int width = (int)(this->CMT["config"]["width"]);
        unsigned long int totalChars = pow(2,FPPB);
        unsigned int *bytes = new unsigned int[encrypted.size()];
        std::cout<<encrypted.size()<<std::endl;
        #pragma omp parallel for
        for(int i=0;i<encrypted.size();i++){ 
            unsigned long int key,intX,intY,floatX,floatY;
            unsigned long int floatXY = 0;
            string baseX,baseY;      
            intX = encrypted[i].intX;
            intY = encrypted[i].intY;
            baseX = to_string(intX - (intX % width));
            baseY = to_string(intY- (intY % width));
            // std::cout<<baseX<<" "<<baseY<<std::endl;
            key  =  std::stoul((string)(this->CMT["data"][baseX][baseY]));
            floatX = (encrypted[i].floatX);
            floatY = (encrypted[i].floatY);
            floatXY = ( floatX << FPPB ) +floatY;
            bytes[i] = (floatXY ^ key) % totalChars;
        }
        return bytes;
    }
};
int main(){
    std::ifstream i("../CMT32.json");
    json j;
    i >> j;
    Encryption enc = Encryption(j);
    std::vector<Point> encrypted;
    //READ FILE INTO BYTES ARRAY
    std::ifstream fstr;
    std::string path="./";
    std::string filename = "Data.txt";
    int fileSize = GetFileSize(path+"Data/"+filename);
    int tailSize = fileSize%4;
    unsigned long int blockCount = fileSize/4;
    if(tailSize>0)
        blockCount++;
    unsigned int *bytes = new unsigned int[blockCount];
    fstr.open(path+"Data/"+filename, std::ios::binary);
    unsigned int block = 0;
    
    for(int i=0;i<fileSize/4;i++){
        fstr.read(reinterpret_cast<char*>(&block), 4);
        bytes[i] = 0;
        bytes[i] = block;
    }
    block=0;
    if(tailSize>0){
        fstr.read(reinterpret_cast<char*>(&block), tailSize);
        bytes[blockCount-1] = 0;
        bytes[blockCount-1] = block;
    }
    fstr.close();
    double start, end;
    start = omp_get_wtime();
    encrypted = enc.encryptByteArray(bytes,blockCount);
    end = omp_get_wtime();
    double delta = end-start;
    cout<<"Time to encrypt "<<fileSize<<" Bytes: "<<delta<<endl;

    std::ofstream file;
    file.open(path+"Encrypted/"+filename+".encrypted", std::ios_base::binary);
    assert(file.is_open());
    file.write((char*)(&tailSize),4);
    std::cout<<"encrypt  tail size"<<tailSize<<std::endl;
    for(int i = 0; i < encrypted.size(); ++i){
        file.write((char*)(&encrypted[i].intX), sizeof(int));
        file.write((char*)(&encrypted[i].floatX), sizeof(unsigned int));
        file.write((char*)(&encrypted[i].intY), sizeof(int));
        file.write((char*)(&encrypted[i].floatY), sizeof(unsigned int));
    }
    file.close();

    fstr.open(path+"Encrypted/"+filename+".encrypted", std::ios_base::binary);
    assert(fstr.is_open());
    std::vector<Point> encryptedFromFile;
    for(int i=0;i<blockCount;i++)
        encryptedFromFile.push_back(Point());
    int decryptTailSize = 0;
    fstr.read(reinterpret_cast<char*>(&decryptTailSize), 4);
    //std::cout<<"decrypt  tail size"<<decryptTailSize<<std::endl;
    for(int i=0;i<blockCount;i++){
        fstr.read(reinterpret_cast<char*>(&encryptedFromFile[i].intX), 4);
        fstr.read(reinterpret_cast<char*>(&encryptedFromFile[i].floatX), 4);
        fstr.read(reinterpret_cast<char*>(&encryptedFromFile[i].intY), 4);
        fstr.read(reinterpret_cast<char*>(&encryptedFromFile[i].floatY), 4);
    }
    unsigned int *decrypted;
    start = omp_get_wtime();
    decrypted = enc.decryptToByteArray(encryptedFromFile);
    end = omp_get_wtime();
    delta = end-start;
    cout<<"Time to decrypt to "<<fileSize<<" Bytes: "<<delta<<endl;
    file.open(path+"Decrypted/"+filename, std::ios_base::binary);
    assert(file.is_open());
    for(int i = 0; i < fileSize/4; ++i)
        file.write((char*)(decrypted + i), sizeof(unsigned int));
    if(decryptTailSize!=0)
        file.write((char*)(decrypted+blockCount-1),decryptTailSize);
    else
        file.write((char*)(decrypted+blockCount-1),sizeof(unsigned int));
    file.close();
    return 0;
 }