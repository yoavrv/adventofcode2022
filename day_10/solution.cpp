#include <iostream>
#include <fstream>
#include <string>
#include <numeric>
#include <vector>
using namespace std;

void read_to_vec(std::istream& fin, vector<int>& vec);

int main(int argc, char* argv[]) {
    ifstream inputstream;
    vector<int> vec;
    if (argc==2) {
        inputstream.open(argv[1]);
        read_to_vec(inputstream,vec);
        inputstream.close();
    } else {
        read_to_vec(std::cin,vec);
    }
    int accX=1;
    int lit=0;
    for (int i=0; i<vec.size(); i++) {
        if (i%40==0){
             std::cout << "[";
        }
        lit = accX-(i%40);
        if (-1<=lit && lit<=1){
            std::cout << "#";
        } else {
            std::cout << ".";
        }
        accX+=vec[i];
        if (i%40==39){
             std::cout << "] \n";
        }
    }
    std::cout << std::endl;
    std::cout << "signals" << std::endl;
    int signal_sum=0;
    for (int i=20; i<vec.size() && i<240; i+=40) {
        accX = (std::accumulate(vec.begin(), vec.begin()+i-1, 1));
        std::cout << "Cycle " << i << " is " << accX*i << " (Accumulator: "<< accX <<")"<< endl;
        signal_sum+=i*accX;
    }
    std::cout << "signals sum is " << signal_sum << std::endl;
    return 0;
}

void read_to_vec(std::istream& fin, vector<int>& vec){
    string line;
    while (getline(fin,line)){
        // std::cout << line << std::endl;
        if (line.find("addx")!=std::string::npos){
            vec.push_back(0);
            vec.push_back(std::stoi(line.substr(5,line.length())));
        } else if (line.find("noop")!=std::string::npos) {
            vec.push_back(0);
        } else {
            break;
        }
    }
}