#include "../core.h"
#include <cassert>
#include <sstream>
#include <iostream>

using namespace std;

void testValidTime() {
    int h, m, s;

    assert(isValidTime("12:30:45", h, m, s) == true);
    assert(h == 12 && m == 30 && s == 45);

    assert(isValidTime("25:10:10", h, m, s) == false);
}

void testValidLogLine() {
    string level, message;
    int h, m, s;

    string line = "2024-01-12 10:15:30 ERROR Something failed";

    assert(isValidLogLine(line, h, m, s, level, message) == true);
    assert(level == "ERROR");
}

void testPeakHour() {
    unordered_map<int,int> errors;
    errors[10] = 2;
    errors[12] = 5;
    errors[14] = 3;

    int maxErrors;
    int peak = findPeakHour(errors, maxErrors);

    assert(peak == 12);
    assert(maxErrors == 5);
}

int main() {
    testValidTime();
    testValidLogLine();
    testPeakHour();

    cout << "All tests passed successfully.\n";
    return 0;
}
