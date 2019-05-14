//
// Created by Xueqing Li on 2019-04-11.
//
#include <iostream>
#include <cstdint>
#include <sstream>
#include <fstream>
#include <chrono>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

string phrase_to_underscore_and_ner(string& line, json& phrases, uint64_t& space, const string& start, const string& end) {
    ostringstream ss_cleaned;
    uint64_t index = 0;
    while (index < line.size()) {
        uint64_t index1 = line.find(start, index);
        if (index1 != string::npos) {
            space++;
            auto before = line.substr(index, index1 - index);
            bool spaceSeen = false;
            for (const auto& c : before) {
                if (c == 32) {
                    if (!spaceSeen) {
                        space++;
                        spaceSeen = true;
                    }
                } else {
                    spaceSeen = false;
                }
            }
            if (!spaceSeen) {
                space++;
            }
            ss_cleaned << before;
            index = index1 + start.size();
            uint64_t index2 = line.find(end, index);
            auto toReplace = line.substr(index, index2 - index);
            json phrase;
            uint64_t space0 = space;
            ostringstream ss;
            for (auto& c : toReplace) {
                if (c == 32) {
                    if (!spaceSeen) {
                        space++;
                        spaceSeen = true;
                    }
                    ss << "_";
                } else {
                    ss << c;
                    spaceSeen = false;
                }
                ss_cleaned << c;
            }
            string s = ss.str();
            phrase["name"] = s;
            phrase["start"] = space0;
            phrase["end"] = space;
            phrases.emplace_back(phrase);
            index = index2 + end.size();
        } else {
            break;
        }
    }
    ss_cleaned << line.substr(index, line.size() - index);
    return ss_cleaned.str();
}

string generate_json(string& title, string& content, string& id, const string& start, const string& end) {
    json j_new = json::object();
    json phrases = json::array();
    uint64_t space = 0;
    auto cleaned_title = phrase_to_underscore_and_ner(title, phrases, space, start, end);
    auto cleaned_content = phrase_to_underscore_and_ner(content, phrases, space, start, end);
    j_new["id"] = id;
    j_new["ner"] = phrases;
    j_new["title"] = cleaned_title;
    j_new["content"] = {cleaned_content};
    return j_new.dump() + '\n';
}

int main(int argc, char** argv) {
    string filename = argv[1];
    ifstream is(filename);
    ofstream os(filename + "_AutoPhraseNER.jl");
    string line;
    string start = "<phrase>";
    string end = "</phrase>";
    int i = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    string title;
    string content;
    string id;
    while(getline(is, line)) {
        if (i % 3 == 0) {
            title = line;
        }
        if (i % 3 == 1) {
            content = line;
        }
        if (i % 3 == 2) {
            id = line;
            auto output = generate_json(title, content, id, start, end);
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
