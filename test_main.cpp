
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "core.h"

using namespace std;


int main(int argc, char *argv[]) {
   cout << "DOCKER VERSION RUNNING" << endl;

    



    if (argc != 3 && argc != 4) {
        cout << "Usage: " << argv[0]
             << " <input_file> <output_file> [--json]\n";
        return 0;
    }

    bool jsonMode = false;

    if(argc == 4 && string(argv[3]) == "--json")
        jsonMode = true;

    ifstream inputfile(argv[1]);
    cout << "Reading file..." << endl;

    ofstream outputfile(argv[2],ios::out|ios::trunc);

    if (!inputfile.is_open() || !outputfile.is_open()) {
        cout << "Could not open files\n";
        return 0;
    }

    unordered_map<int,int> errors;
    unordered_map<string,int> levelCount;
    unordered_map<string,string> keywordcategory;
    unordered_map<string,int> categoryCount;

    loadKeywordCategories("categories.conf", keywordcategory);

    vector<string> spikeEvents;

    int windowSize=300;
    int threshold=3;
    int skippedlines = 0;

    processLog(
        inputfile,
        errors,
        levelCount,
        keywordcategory,
        categoryCount,
        spikeEvents,
        windowSize,
        threshold,
        skippedlines
    );

    if(jsonMode){

    json j;

    j["errors_per_hour"] = errors;
    j["log_levels"] = levelCount;
    j["categories"] = categoryCount;
   j["alerts"] = spikeEvents;
    j["skipped_lines"] = skippedlines;


    outputfile << j.dump(4);
}


    inputfile.close();
    outputfile.close();

    return 0;
}
