#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <thread>
using namespace std;

//
// Created by Xueqing Li on 2019-05-13.
//
string underscore_text(string& line, const string& start, const string& end) {
    uint64_t index = 0;
    ostringstream ss;
    while (index < line.size()) {
        uint64_t index1 = line.find(start, index);
        if (index1 != string::npos) {
            ss << line.substr(index, index1 - index);
            index = index1 + start.size();
            uint64_t index2 = line.find(end, index);
            auto toReplace = line.substr(index, index2 - index);
            for (auto& c : toReplace) {
                if (c == 32) {
                    ss << "_";
                } else {
                    ss << c;
                }
            }
            index = index2 + end.size();
        } else {
            break;
        }
    }
    ss << line.substr(index, line.size() - index);
    ss << "\n";
    return ss.str();
}

int main(int argc, char** argv) {
    string filename = argv[1];
    ifstream is(filename);
    ofstream os(filename + "_underscored.jl");
    string line;
    string start = "<phrase>";
    string end = "</phrase>";
    int i = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    while(getline(is, line)) {
        if (i % 3 != 2) {
            auto output = underscore_text(line, start, end);
            os.write(output.c_str(), output.size());
        }
        i++;
        if (i % 1000 == 0) {
            cout << i << " done" << endl;
            auto t = std::chrono::high_resolution_clock::now();
            cout << (t - t0).count() << endl;
            t0 = t;
        }
    }
}