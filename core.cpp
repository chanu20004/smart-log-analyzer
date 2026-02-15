#include "core.h"

#include <sstream>
#include <queue>
#include <cctype>

using namespace std;

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

    if (!isValidTime(time, hour, minute, second))
        return false;

    getline(iss, message);

    if (!message.empty() && message[0] == ' ')
        message.erase(0, 1);

    return true;
}

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
            continue;
        }

        for (char &c : level)
            c = toupper(c);

        levelCount[level]++;

        if (level == "ERROR") {

            errors[hour]++;

            int totalseconds = hour*3600 + minute*60 + second;

            errorwindow.push(totalseconds);

            while(!errorwindow.empty() &&
                  totalseconds - errorwindow.front() > windowSize) {
                errorwindow.pop();
            }

            if(errorwindow.size() > (size_t)threshold && !spikeactive){

                ostringstream alert;
                alert << "SPIKE DETECTED at time "
                      << hour << ":" << minute << ":" << second
                      << " (" << errorwindow.size()
                      << " errors in last 5 minutes)";

                spikeEvents.push_back(alert.str());
                spikeactive = true;
            }

            if(errorwindow.size() <= (size_t)threshold)
                spikeactive = false;

            string lower = message;

            for(char &c : lower)
                c = tolower(c);

            bool matched = false;

            for(auto &p : keywordcategory){
                if(lower.find(p.first) != string::npos){
                    categoryCount[p.second]++;
                    matched = true;
                }
            }

            if(!matched)
                categoryCount["UNKNOWN"]++;
        }
    }
}

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

void loadKeywordCategories(const string& filename,
                           unordered_map<string,string> &keywordcategory){

    ifstream file(filename);
    string line;

    while(getline(file,line)){

        if(line.empty())
            continue;

        size_t pos = line.find('=');

        if(pos == string::npos)
            continue;

        string keyword = line.substr(0,pos);
        string category = line.substr(pos+1);

        for(char &c : keyword)
            c = tolower(c);

        keywordcategory[keyword] = category;
    }
}
