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
        // cout<<this->CMT["config"];
    }

    public: vector<Point> encryptByteArray(unsigned int bytes[],int size){
        // cout<<"encrypt";
        string blank = "";
        cout<<"filesize in encryptBYteArray: "<<size<<endl;
        vector<Point> points;
        for(int i=0;i<size;i++)
            points.push_back(Point());
        cout<<" points size in encrypt"<<points.size()<<endl; 
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
            unsigned int byte = bytes[i];    
            int intX = distrX(eng);
            int intY = distrY(eng);
            int floatX = distrFloat(eng);
            int floatY = distrFloat(eng);
            string baseX = to_string(intX - (intX % width));
            string baseY = to_string(intY- (intY % width));
            int key  =  this->CMT[blank+"data"][blank+baseX][blank+baseY];
            // int key = 140;
            //cout<<intX<<"."<<floatX<<" "<<key<<" "<<intY<<"."<<floatY;
            int floatXY = ( floatX << FPPB ) +floatY;
            int floatXYXor = floatXY ^ key;
            int beta = floatXYXor % 256;
            
            // cout<<(bitset<8>(floatX)).to_string()<<(bitset<8>(floatY)).to_string()<<"\n";
            // cout<<(bitset<16>(floatXY)).to_string()<<"\n";
            // cout<<(bitset<16>(key)).to_string()<<"\n";
            // cout<<(bitset<16>(beta)).to_string()<<"\n";
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
            // int newFloatY = (newFloatXY & (int)(pow(2,FPPB)-1));
            int newFloatY = (newFloatXY & 255);
            Point *p =new Point;
            p->intX = intX;
            p->floatX = newFloatX;
            p->intY = intY;
            p->floatY = newFloatY;
            // points.push_back(new Point(intX,newFloatX,intY,newFloatY));
            points[i].intX=intX;
            points[i].floatX=newFloatX;
            points[i].intY=intY;
            points[i].floatY=newFloatY;
            // cout<<(bitset<16>(newFloatXY)).to_string()<<"\n";
            // cout<<(bitset<8>(newFloatX)).to_string()<<"\n";
            // cout<<(bitset<8>(newFloatY)).to_string()<<"\n";
            // cout<<((int)(pow(2,FPPB)-1)<< FPPB)<<"\n";
            // const unsigned int temp = FPPB;
        // ostringstream outX;
        // outX << std::internal << std::setfill('0') << std::setw(FPPDigits) << newFloatX;
        // string stringX  = to_string(intX) + "." + outX.str();
        // ostringstream outY;
        // outY << std::internal << std::setfill('0') << std::setw(FPPDigits) << newFloatY;
        // string stringY  = to_string(intY) + "." + outY.str();
        // float x = ::atof(stringX.c_str());
        // float y = ::atof(stringY.c_str());
            // cout<<x<<" "<<y<<" "<<endl;
        // Point p = Point(x,y);
        // cout<<"encryption end"<<endl;
        }
        cout<<"points size: "<<points.size()<<endl;
        return points;
    }

    public: unsigned int* decryptToByteArray(vector<Point> encrypted,int size){
        
        int FPPB = (int)(this->CMT["config"]["FPPB"]);
        // cout<<"Decrypt"<<endl;
        int width = (int)(this->CMT["config"]["width"]);
        // cout<<x<<" "<<y<<endl;
        unsigned int *bytes = new unsigned int[encrypted.size()];
        int key;
        cout<<"size in decrypt"<<encrypted.size()<<endl;
        for(int i=0;i<encrypted.size();i++){
            
            int intX = encrypted[i].intX;
            int intY = encrypted[i].intY;
            // int intX = 142;
            // int intY = 145;
            string baseX = to_string(intX - (intX % width));
            string baseY = to_string(intY- (intY % width));
            key  =  (int)(this->CMT["data"][baseX][baseY]);
            // cout<<baseX<<" "<<baseY<<" "<<key<<endl;
            int floatX = (encrypted[i].floatX);
            int floatY = (encrypted[i].floatY);
            // int floatX = 245;
            // int floatY = 145;
            //cout<<floatX<<" "<<floatY<<" "<<endl;
            // cout<<this->CMT["config"]["FPPDecimal"]<<endl;
            int floatXY = ( floatX << FPPB ) +floatY;
            bytes[i] = (floatXY ^ key) % 256;
        }
        
        // cout<<byte<<endl;
        // cout<<"end-decryption"<<endl;
        return bytes;
    }
};

int main(){
    std::ifstream i("../CMT.json");
    json j;
    i >> j;
    cout<< "int size"<<sizeof(int);
    ifstream fstr;
    int fileSize = GetFileSize("../Data.txt");
    unsigned int *bytes = new unsigned int[fileSize];
    fstr.open("../Data.txt", std::ios::binary);
    for(int i=0;i<fileSize;i++){
        char byteChar;
        fstr.read(reinterpret_cast<char*>(&byteChar), 1);
        bytes[i] = byteChar;
    }
    double start, end;
    start = omp_get_wtime();
    unsigned int *decrypted;
    Encryption enc = Encryption(j);
    vector<Point> encrypted;
    cout<<"filesize: "<<fileSize<<endl;
    encrypted = enc.encryptByteArray(bytes,fileSize);
    cout<<"encyptedsize: "<<encrypted.size()<<endl;
        //cout<<"iteration: "<<i<<endl;
    decrypted = enc.decryptToByteArray(encrypted,fileSize);
        // // cout<<"iteration: "<<i<<endl;
        // if(byte != 25){
        //     cout<<"trouble"<<endl;
        //     cout<<p.x<<p.y<<byte<<i<<endl;
        // }
    end = omp_get_wtime();
    for(int i=0;i<fileSize;i++){
        if(bytes[i]!=decrypted[i])
            cout<<"trouble"<<i<<" "<<bytes[i]<<" "<<decrypted[i]<<endl;
    }
    double delta = end-start;
    cout<<delta;
    ofstream fout;
    cout<<"decrypted size"<<(sizeof(&decrypted));
    fout.open("file.bin", ios::binary | ios::out);
    fout.write((char*) &decrypted, sizeof(decrypted));
    fout.close();
    return 0;

}

