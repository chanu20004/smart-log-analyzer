#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "core.h"

using namespace std;

int main(int argc, char *argv[]) {

    if (argc != 3 && argc != 4) {
        cout << "Usage: " << argv[0]
             << " <input_file> <output_file> [--json]\n";
        return 0;
    }

    bool jsonMode = false;

    if(argc == 4 && string(argv[3]) == "--json")
        jsonMode = true;

    ifstream inputfile(argv[1]);
    ofstream outputfile(argv[2]);

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

        outputfile << "{\n";

        outputfile << "  \"errors_per_hour\": {\n";

        size_t count = 0;
        for(const auto &p : errors){
            outputfile << "    \"" << p.first << "\": " << p.second;
            count++;
            if(count < errors.size())
                outputfile << ",";
            outputfile << "\n";
        }

        outputfile << "  },\n";

        outputfile << "  \"log_levels\": {\n";

        count = 0;
        for(const auto &p : levelCount){
            outputfile << "    \"" << p.first << "\": " << p.second;
            count++;
            if(count < levelCount.size())
                outputfile << ",";
            outputfile << "\n";
        }

        outputfile << "  },\n";

        outputfile << "  \"categories\": {\n";

        count = 0;
        for(const auto &p : categoryCount){
            outputfile << "    \"" << p.first << "\": " << p.second;
            count++;
            if(count < categoryCount.size())
                outputfile << ",";
            outputfile << "\n";
        }

        outputfile << "  }\n";
        outputfile << "}\n";
    }

    inputfile.close();
    outputfile.close();

    return 0;
}
