#include<iostream>
#include<nlohmann/json.hpp>
#include <ctime>
#include<math.h>
#include<iomanip>
#include<sstream>
#include<random>
#include <typeinfo>
#include<map>
#include<vector>
#include <bitset>
#include <omp.h>
#include <fstream>
#include<vector>
#include <sys/stat.h> 
using namespace std;

using json = nlohmann::json;
using namespace std;
long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}
class Point{
    public: int intX;
    public: int intY;
    public: int floatX;
    public: int floatY;
    public: Point(){

    }
    public: Point(int intX,int  floatX,int intY,int floatY){
        this->intX = intX;
        this->floatX = floatX;
        this->intY = intY;
        this->floatY = floatY;
    }
};
class Encryption{
    json CMT;
    public: Encryption(json CMT){
        this->CMT = CMT;
    }

    public: vector<Point> encryptByteArray(unsigned char bytes[],int size){
        string blank = "";
        vector<Point> points;
        for(int i=0;i<size;i++)
            points.push_back(Point());
        int width = (int)(this->CMT[blank+"config"][blank+"width"]);
        int FPPB = (int)(this->CMT[blank+"config"][blank+"FPPB"]);
        int FPPDigits = (int)(this->CMT[blank+"config"][blank+"FPPDigits"]);
        int xmax = (this->CMT[blank+"data"]).size() * width - width;
        int ymax = (this->CMT[blank+"data"][blank+"0"]).size() * width -width;
        int keySize = FPPB * 2;
        random_device rd;
        mt19937 eng(rd());
        uniform_int_distribution<> distrX(0, xmax);
        uniform_int_distribution<> distrY(0, ymax); 
        uniform_int_distribution<> distrFloat(0 , 100);
        #pragma omp parallel for
        for(int i = 0 ;i<size;i++){
            unsigned char byte = bytes[i];    
            int intX = distrX(eng);
            int intY = distrY(eng);
            int floatX = distrFloat(eng);
            int floatY = distrFloat(eng);
            string baseX = to_string(intX - (intX % width));
            string baseY = to_string(intY- (intY % width));
            int key  =  this->CMT[blank+"data"][blank+baseX][blank+baseY];
            int floatXY = ( floatX << FPPB ) +floatY;
            int floatXYXor = floatXY ^ key;
            int beta = floatXYXor % 256;
            int diff=0;
            
            if(beta > byte){
                diff = 256 - beta;
                diff+= byte;
            }
            else{
                diff = byte-beta;
            }
            floatXYXor = beta + diff;
            int newFloatXY = floatXYXor ^ key;
            int newFloatX = (newFloatXY >> FPPB);
            int newFloatY = (newFloatXY & 255);
            points[i].intX=intX;
            points[i].floatX=newFloatX;
            points[i].intY=intY;
            points[i].floatY=newFloatY;
        }
        return points;
    }

    public: unsigned char* decryptToByteArray(vector<Point> encrypted,int size){
        
        int FPPB = (int)(this->CMT["config"]["FPPB"]);
        int width = (int)(this->CMT["config"]["width"]);
        unsigned char *bytes = new unsigned char[encrypted.size()];
        #pragma omp parallel for
        for(int i=0;i<encrypted.size();i++){ 
            int key,intX,intY,floatX,floatY,floatXY;
            string baseX,baseY;      
            intX = encrypted[i].intX;
            intY = encrypted[i].intY;
            baseX = to_string(intX - (intX % width));
            baseY = to_string(intY- (intY % width));
            key  =  (int)(this->CMT["data"][baseX][baseY]);
            floatX = (encrypted[i].floatX);
            floatY = (encrypted[i].floatY);
            floatXY = ( floatX << FPPB ) +floatY;
            bytes[i] = (floatXY ^ key) % 256;
        }
        return bytes;
    }
};

int main(){
    std::ifstream i("../CMT.json");
    json j;
    i >> j;
    ifstream fstr;
    string path="../";
    string filename = "Data.mp3";
    int fileSize = GetFileSize(path+filename);
    unsigned char *bytes = new unsigned char[fileSize];
    fstr.open(path+filename, std::ios::binary);
    for(int i=0;i<fileSize;i++){
        char byteChar;
        fstr.read(reinterpret_cast<char*>(&byteChar), 1);
        bytes[i] = byteChar;
    }
    double start, end;
    start = omp_get_wtime();
    unsigned char *decrypted;
    Encryption enc = Encryption(j);
    vector<Point> encrypted;
    encrypted = enc.encryptByteArray(bytes,fileSize);
    end = omp_get_wtime();
    double delta = end-start;
    cout<<"Time to encrypt "<<fileSize<<" Bytes: "<<delta<<endl;
    start = omp_get_wtime();
    decrypted = enc.decryptToByteArray(encrypted,fileSize);
    end = omp_get_wtime();
    delta = end-start;
    cout<<"Time to decrypt to "<<fileSize<<" Bytes: "<<delta<<endl;
    for(int i=0;i<fileSize;i++){
        if(bytes[i]!=decrypted[i])
            cout<<"trouble"<<i<<" "<<bytes[i]<<" "<<decrypted[i]<<endl;
    }
    ofstream fout;
    std::ofstream file;
    file.open(filename, std::ios_base::binary);
    assert(file.is_open());
    for(int i = 0; i < fileSize; ++i)
       file.write((char*)(decrypted + i * sizeof(decrypted[0])), sizeof(decrypted[0]));
    file.close();
    return 0;

}

