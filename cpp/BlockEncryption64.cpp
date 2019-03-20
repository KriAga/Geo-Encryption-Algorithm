#include<iostream>
#include <fstream>
#include<vector>
#include <sys/stat.h> 
#include<nlohmann/json.hpp>
#include <omp.h>
using json = nlohmann::json;
using namespace std;
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
    public: vector<Point> encryptByteArray(unsigned long int bytes[],int size){
            vector<Point> points;
            for(int i=0;i<size;i++)
                points.push_back(Point());
            int width = (int)(this->CMT["config"]["width"]);
            int FPPB = (int)(this->CMT["config"]["FPPB"]);
            //int FPPDigits = (int)(this->CMT["config"]["FPPDigits"]);
            int xmax = (this->CMT["data"]).size() * width - width;
            int ymax = (this->CMT["data"]["0"]).size() * width -width;
            int keySize = FPPB * 2;
            unsigned long long int totalChars = pow(2,FPPB-1);
            random_device rd;
            mt19937 eng(rd());
            uniform_int_distribution<> distrX(0, xmax);
            uniform_int_distribution<> distrY(0, ymax); 
            uniform_int_distribution<> distrFloat(0 , pow(2,16));
            #pragma omp parallel for
            for(int i = 0 ;i<size;i++){
                unsigned long int byte = bytes[i];    
                int intX = distrX(eng);
                int intY = distrY(eng);
                unsigned long int floatX = distrFloat(eng);
                unsigned long int floatY = distrFloat(eng);
                string baseX = to_string(intX - (intX % width));
                string baseY = to_string(intY- (intY % width));
                unsigned long long int key  =  this->CMT["data"][baseX][baseY];
                unsigned long long int floatXY = ( ((unsigned long long int)(floatX)) << FPPB ) +floatY;
                unsigned long long int floatXYXor = floatXY ^ key;
                unsigned long long int beta = (unsigned long long int)(floatXYXor) % (totalChars);
                unsigned long long int diff=0;
                if(beta > byte){
                    diff = totalChars - beta;
                    diff+= byte;
                }
                else{
                    diff = byte-beta;
                }
                floatXYXor = beta + diff;
                unsigned long long int newFloatXY = floatXYXor ^ key;
                unsigned long int newFloatX = (newFloatXY >> FPPB);
                unsigned long int newFloatY = (newFloatXY & (totalChars-1));
                points[i].intX=intX;
                points[i].floatX=newFloatX;
                points[i].intY=intY;
                points[i].floatY=newFloatY;
            }
            return points;
    }
    public: unsigned long int* decryptToByteArray(vector<Point> encrypted){
        int FPPB = 32;
        int width = (int)(this->CMT["config"]["width"]);
        unsigned long long int totalChars = pow(2,FPPB);
        unsigned long int *bytes = new unsigned long int[encrypted.size()];
        #pragma omp parallel for
        for(int i=0;i<encrypted.size();i++){ 
            unsigned long long int key,intX,intY,floatX,floatY;
            unsigned long long int floatXY = 0;
            string baseX,baseY;      
            intX = encrypted[i].intX;
            intY = encrypted[i].intY;
            baseX = to_string(intX - (intX % width));
            baseY = to_string(intY- (intY % width));
            key  =  (int)(this->CMT["data"][baseX][baseY]);
            floatX = (encrypted[i].floatX);
            floatY = (encrypted[i].floatY);
            floatXY = ( floatX << FPPB ) +floatY;
            bytes[i] = (floatXY ^ key) % totalChars;
        }
        return bytes;
    }
};
int main(){
    std::ifstream i("../CMT64.json");
    json j;
    i >> j;
    Encryption enc = Encryption(j);
    vector<Point> encrypted;
    ifstream fstr;
    string path="../";
    string filename = "Data256.bin";
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
    cout<<"Time to encrypt "<<fileSize<<" Bytes: "<<delta<<endl;
    unsigned long int *decrypted;
    start = omp_get_wtime();
    decrypted = enc.decryptToByteArray(encrypted);
    end = omp_get_wtime();
    delta = end-start;
    cout<<"Time to decrypt to "<<fileSize<<" Bytes: "<<delta<<endl;
    std::ofstream file;
    file.open(filename, std::ios_base::binary);
    assert(file.is_open());
    for(int i = 0; i < fileSize/8; ++i)
        file.write((char*)(decrypted + i), sizeof(unsigned long int));
    file.close();
    cout<<"size of unsigned long int"<<sizeof(unsigned long int);
    return 0;
 }