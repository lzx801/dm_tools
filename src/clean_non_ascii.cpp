#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <thread>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

string get_cleaned_text(string& line) {
    auto j_ori = json::parse(line);
    ostringstream clean_html;
    auto s = j_ori["title"].dump();
    for (uint64_t i = 1; i < s.size() - 1;i++) {
        if (s[i] >= 0) {
            clean_html << s[i];
        } else {
            if (s[i + 2] == -103) {
                clean_html << "'";
                i += 2;
            } else if (s[i + 2] == -100 || s[i + 2] == -99) {
                clean_html << "\"";
                i += 2;
            } else if (s[i + 2] == -108) {
                clean_html << "-";
                i += 2;
            } else {
                clean_html << " ";
                i += 2;
            }
        }
    }

    clean_html << "\n";
    for (auto& content : j_ori["contents"]) {
        auto uncleaned = content["content"];
        if (uncleaned != nullptr && content["type"].dump() == "\"sanitized_html\"") {
            auto d = uncleaned.dump();
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
                    if (d[i] >= 0) {
                        clean_html << d[i];
                    } else {
                        if (d[i + 2] == -103) {
                            clean_html << "'";
                            i += 2;
                        } else if (d[i + 2] == -100 || d[i + 2] == -99) {
                            clean_html << "\"";
                            i += 2;
                        } else if (d[i + 2] == -108) {
                            clean_html << "-";
                            i += 2;
                        } else {
                            clean_html << " ";
                            i += 2;
                        }
                    }
                }
            }
            clean_html << " ";
        }
    }
    clean_html << "\n";
    auto id = j_ori["id"].dump();
    for (uint64_t i = 1; i < id.size() - 1;i++) {
        clean_html << id[i];
    }
    return clean_html.str() + '\n';
}

int main(int argc, char** argv) {
    string filename = argv[1];
    ifstream is(filename);
    ofstream os(filename + "_sanitized_html.jl");
    string line;
    int i = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    while(getline(is, line)) {
        auto output = get_cleaned_text(line);
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