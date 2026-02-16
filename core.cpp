#include "core.h"

#include <sstream>
#include <algorithm>
#include<iostream>

#include <queue>
#include <cctype>

using namespace std;


/* =========================
   Helper Function (NEW)
   ========================= */

string trim(string s) {

    // Remove trailing characters
    while (!s.empty() &&
          (s.back() == '\r' ||
           s.back() == '\n' ||
           s.back() == ' '  ||
           s.back() == '\t')) {
        s.pop_back();
    }

    // Remove leading characters
    while (!s.empty() &&
          (s.front() == ' ' ||
           s.front() == '\t')) {
        s.erase(s.begin());
    }

    return s;
}


/* =========================
   Time Validation
   ========================= */

bool isValidTime(const string &time, int &hour,int &minute,int &second) {

    istringstream ts(time);
    char c1, c2;

    if (!(ts >> hour >> c1 >> minute >> c2 >> second) ||
        c1 != ':' || c2 != ':') {
        return false;
    }

    if (hour < 0 || hour > 23)
        return false;

    if (minute < 0 || minute > 59)
        return false;

    if (second < 0 || second > 59)
        return false;

    return true;
}


/* =========================
   Log Line Validation
   ========================= */

bool isValidLogLine(const string &line,
                    int &hour,
                    int &minute,
                    int &second,
                    string &level,
                    string &message) {

    if (line.find_first_not_of(" \t\r\n") == string::npos)
        return false;

    istringstream iss(line);
    string date, time;

    if (!(iss >> date >> time >> level))
        return false;

    level = trim(level);
     level.erase(remove(level.begin(), level.end(), '\r'), level.end());
// 🔥 FIX ADDED

    if (!isValidTime(time, hour, minute, second))
        return false;

    getline(iss, message);

    if (!message.empty() && message[0] == ' ')
        message.erase(0, 1);

    message = trim(message);   // 🔥 SAFE CLEAN

    return true;
}


/* =========================
   Main Processing
   ========================= */

void processLog(
    ifstream &inputfile,
    unordered_map<int,int> &errors,
    unordered_map<string,int> &levelCount,
    unordered_map<string,string> &keywordcategory,
    unordered_map<string,int> &categoryCount,
    vector<string> &spikeEvents,
    int windowSize,
    int threshold,
    int &skippedlines
) {

    queue<int> errorwindow;

    string line;
    int hour,minute,second;
    string level;
    string message;
    bool spikeactive=false;

    while (getline(inputfile, line)) {

        if (!isValidLogLine(line, hour,minute,second, level, message)) {
            skippedlines++;
              cout << "Skipped line: [" << line << "]" << endl;
            continue;
        }

        for (char &c : level)
            c = toupper(c);
            // Remove carriage return if exists
if (!level.empty() && level.back() == '\r')
    level.pop_back();


        levelCount[level]++;

        if (level == "ERROR") {

            errors[hour]++;

            int totalseconds = hour*3600 + minute*60 + second;

            errorwindow.push(totalseconds);

            while(!errorwindow.empty() &&
                  totalseconds - errorwindow.front() > windowSize) {
                errorwindow.pop();
            }

            if(errorwindow.size() >=(size_t)threshold && !spikeactive){

                ostringstream alert;
                alert << "SPIKE DETECTED at time "
                      << hour << ":" << minute << ":" << second
                      << " (" << errorwindow.size()
                      << " errors in last 5 minutes)";

                spikeEvents.push_back(alert.str());
                spikeactive = true;
            }

            if(errorwindow.size() < (size_t)threshold)
                spikeactive = false;

            string lower = message;

            for(char &c : lower)
                c = tolower(c);

            bool matched = false;

            for(auto &p : keywordcategory){
    string cleanCategory = p.second;
    cleanCategory.erase(remove(cleanCategory.begin(), cleanCategory.end(), '\r'), cleanCategory.end());

    if(lower.find(p.first) != string::npos){
        categoryCount[cleanCategory]++;
        matched = true;
    }
}


            if(!matched)
                categoryCount["UNKNOWN"]++;
        }
    }
}


/* =========================
   Peak Hour
   ========================= */

int findPeakHour(const unordered_map<int,int> &errors, int &maxerrors) {

    int peakhour = -1;
    maxerrors = 0;

    for (const auto &it : errors) {
        if (it.second > maxerrors) {
            maxerrors = it.second;
            peakhour = it.first;
        }
    }

    return peakhour;
}


/* =========================
   Load Categories
   ========================= */

void loadKeywordCategories(const string& filename,
                           unordered_map<string,string> &keywordcategory){

    ifstream file(filename);
    string line;

    while(getline(file,line)){

        // Remove Windows carriage return if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if(line.empty())
            continue;

        size_t pos = line.find('=');

        if(pos == string::npos)
            continue;

        string keyword = line.substr(0,pos);
        string category = line.substr(pos+1);

        // Trim keyword
        while(!keyword.empty() && isspace(keyword.back()))
            keyword.pop_back();
        while(!keyword.empty() && isspace(keyword.front()))
            keyword.erase(keyword.begin());

        // Trim category
        while(!category.empty() && isspace(category.back()))
            category.pop_back();
        while(!category.empty() && isspace(category.front()))
            category.erase(category.begin());

        // Convert keyword to lowercase
        for(char &c : keyword)
            c = tolower(c);

        keywordcategory[keyword] = category;
    }
}

