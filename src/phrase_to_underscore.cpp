//
// Created by Xueqing Li on 2019-04-11.
//
#include <iostream>
#include <cstdint>
#include <sstream>
#include <fstream>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

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
    auto j_ori = json::parse(output);
    ostringstream clean_html;
    auto s = j_ori["title"].dump();
    clean_html << s.substr(1, s.size() - 2) << "\n";
    for (auto& content : j_ori["contents"]) {
        auto uncleaned = content["content"];
        if (uncleaned != nullptr) {
            auto d = uncleaned.dump();
            if (d[0] != '\"') {
                continue;
            }
            //cout << d << endl;
            bool entered = false;
            for (int i = 1; i < d.size() - 1; i++) {
                if (d[i] == '<') {
                    entered = true;
                    continue;
                }
                if (d[i] == '>') {
                    entered = false;
                    continue;
                }
                if (!entered) {
                    clean_html << d[i];
                }
            }
            clean_html << " ";
        }
    }
    return clean_html.str() + '\n';
}

string generate_json(string& line, const string& start, const string& end) {
    uint64_t index = 0;
    json j_new = json::object();
    json phrases = json::array();
    ostringstream ss_cleaned;
    int space = 0;
    while (index < line.size()) {
        uint64_t index1 = line.find(start, index);
        if (index1 != string::npos) {
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
            int space0 = space;
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
            space++;
            string s = ss.str();
            if (s.find('/') == string::npos && s.find("kicker") == string::npos && s.find("post") == string::npos
&& s.find("Post") == string::npos && s.find("POST") == string::npos) {
                phrase["name"] = s;
                phrase["start"] = space0;
                phrase["end"] = space;
                //cout << phrase << endl;
                phrases.emplace_back(phrase);
            }
            index = index2 + end.size();
        } else {
            break;
        }
    }
    ss_cleaned << line.substr(index, line.size() - index);
    auto clstr = ss_cleaned.str();
    auto j_ori = json::parse(clstr);
    j_new["id"] = j_ori["id"];
    j_new["ner"] = phrases;
    j_new["title"] = j_ori["title"];
    ostringstream clean_html;
    for (auto& content : j_ori["contents"]) {
        auto uncleaned = content["content"];
        if (uncleaned != nullptr) {
            auto d = uncleaned.dump();
            if (d[0] != '\"') {
                continue;
            }
            //cout << d << endl;
            bool entered = false;
            for (int i = 1; i < d.size() - 1; i++) {
                if (d[i] == '<') {
                    entered = true;
                    continue;
                }
                if (d[i] == '>') {
                    entered = false;
                    continue;
                }
                if (!entered) {
                    clean_html << d[i];
                }
            }
            clean_html << "\n\n";
        }
    }
    //cout << clean_html.str() << endl;
    j_new["content"] = clean_html.str();
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
    while(getline(is, line)) {
        auto output = replace_phrase_tags(line, start, end);
        os.write(output.c_str(), output.size());
        i++;
        if (i % 1000 == 0) {
            cout << i << " done" << endl;
            auto t = std::chrono::high_resolution_clock::now();
            cout << (t - t0).count() << endl;
            t0 = t;
        }
    }
}
