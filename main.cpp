// Smart Log Analyzer
// ------------------------------------
// Features:
// - Validates log format
// - Skips malformed / empty lines
// - Time-based ERROR analysis (per hour)
// - Peak error hour detection
// - Command-line configurable input/output

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include<vector>
#include<cctype>
using namespace std;
bool isValidTime(const string &time,int &hour){
    istringstream ts(time);
    int minute,second;
    char c1,c2;
    if(!(ts>>hour>>c1>>minute>>c2>>second)||c1!=':'||c2!=':'){
        return false;
    }
    if(hour<0||hour>23){
        return false;
    }
    return true;
}
bool isValidLogLine(const string &line,int &hour,string &level){
    if(line.find_first_not_of(" \t\r\n")==string::npos){
        return false;
    }
    istringstream iss(line);
    string date,time;
    if(!(iss>>date>>time>>level)){
        return false;
    }
    if(!isValidTime(time,hour)){
        return false;
    }
    return true;

}
bool isCriticalError(
    const string &message,
    const vector<string> &keywords,
    string &matched
){
    string lower = message;
    for(char &c : lower){
        c = tolower(c);
    }

    for(const string &k : keywords){
        if(lower.find(k) != string::npos){
            matched = k;
            return true;
        }
    }
    return false;
}

void loadCriticalKeywords(
    const string &filename,
    vector<string> &keywords
){
    ifstream file(filename);
    string word;

    while(getline(file, word)){
        if(!word.empty()){
            for(char &c : word){
                c = tolower(c);
            }
            keywords.push_back(word);
        }
    }
}

void processLog(
    ifstream &inputfile,
    map<int,int> &errors,
    map<string,int> &levelCount,
    map<string,int> &criticalCount,
    const vector<string> &criticalKeywords,
    int &skippedlines
){

    string line;
    int hour;
    string level;
    while(getline(inputfile,line)){
        if(!isValidLogLine(line,hour,level)){
            skippedlines++;
            continue;
        }
           istringstream iss(line);
        string date, time;
        iss >> date >> time >> level;
        for(char &c:level){
            c=toupper(c);
        }
        levelCount[level]++;
        string message;
        getline(iss,message);
        if(!message.empty() && message[0]==' '){
            message.erase(0,1);
        }
        if(level==("ERROR")){
            errors[hour]++;
            string keyword;
            if(isCriticalError(message, criticalKeywords, keyword)){
    criticalCount[keyword]++;
}

        }

    }
}
int findpeakhour(const map<int,int> &errors,int &maxerrors){
    int peakhour=-1;
    maxerrors=0;
    for(auto it:errors){
        if(it.second>maxerrors){
            maxerrors=it.second;
            peakhour=it.first;
        }
    }
    return peakhour;
}
int main(int argc,char *argv[]){
    vector<string> criticalKeywords;
loadCriticalKeywords("critical.conf", criticalKeywords);

    if(argc!=3){
        cout<<" usage "<<argv[0]
        <<" <input_file><output_file\n> ";
        return 0;
    }
    ifstream inputfile(argv[1]);
    ofstream errorfile(argv[2]);
    if(!inputfile.is_open()||!errorfile.is_open()){
        cout<<"could not open files\n";
        return 0;
    }
    map<int,int> errors;
    map<string,int> levelCount;
    map<string,int> criticalCount;
    int skippedlines=0;
    processLog(
    inputfile,
    errors,
    levelCount,
    criticalCount,
    criticalKeywords,
    skippedlines
);

    errorfile<<"Errors per hour\n";
    errorfile<<"-----------------\n";
    
if(errors.empty()){
    errorfile << "No ERROR entries found.\n";
  }else{
    for(auto it:errors){
        errorfile<<"Hour "<<it.first<<" : "<<it.second<<" errors\n";
    }
}
    int maxerrors;
    int peakhour=findpeakhour(errors,maxerrors);
    if(peakhour!=-1){
        errorfile<<"\nPeak Error Hour:"<<peakhour<<" with "<<maxerrors<<"  errors";
    }
    errorfile<<"\n-------------------------------";
    errorfile << "\nLog Level Summary\n";
errorfile << "-----------------\n";

for(auto it : levelCount){
    errorfile << it.first << " : "
              << it.second << "\n";
}
errorfile << "-----------------\n";
errorfile << "\nCritical ERROR Summary\n";
errorfile << "---------------------\n";

if(criticalCount.empty()){
    errorfile << "No critical ERRORs found.\n";
} else {
    for(auto it : criticalCount){
        errorfile << it.first << " : "
                  << it.second << "\n";
    }
}

     errorfile<<"\nSkippedLines : "<<skippedlines<<"\n";
       inputfile.close();
       errorfile.close();
       return 0;
}