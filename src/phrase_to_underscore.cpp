//
// Created by Xueqing Li on 2019-04-11.
//
#include <iostream>
#include <cstdint>
#include <sstream>
#include <fstream>
using namespace std;

string replace_phrase_tags(string& line, const string& start, const string& end) {
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
    string output = ss.str();
    return output;
}

int main(int argc, char** argv) {
    string filename = argv[1];
    ifstream is(filename);
    ofstream os(filename + "_replaced");
    string line;
    string start = "<phrase>";
    string end = "</phrase>";
    while(getline(is, line)) {
        auto output = replace_phrase_tags(line, start, end);
        os.write(output.c_str(), output.size());
    }
}
