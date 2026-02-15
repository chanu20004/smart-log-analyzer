#ifndef CORE_H
#define CORE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

// DO NOT use using namespace std in headers

bool isValidTime(const std::string &time,
                 int &hour,
                 int &minute,
                 int &second);

bool isValidLogLine(const std::string &line,
                    int &hour,
                    int &minute,
                    int &second,
                    std::string &level,
                    std::string &message);

void processLog(
    std::ifstream &inputfile,
    std::unordered_map<int,int> &errors,
    std::unordered_map<std::string,int> &levelCount,
    std::unordered_map<std::string,std::string> &keywordcategory,
    std::unordered_map<std::string,int> &categoryCount,
    std::vector<std::string> &spikeEvents,
    int windowSize,
    int threshold,
    int &skippedlines
);

int findPeakHour(const std::unordered_map<int,int> &errors,
                 int &maxerrors);

void loadKeywordCategories(const std::string& filename,
                           std::unordered_map<std::string,std::string> &keywordcategory);

#endif
