#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <vector>
#include <string>
using namespace std;

// Header file for attendance functions and declarations
const string ATTENDANCE_FILE = "attendance_log.txt";
const double SHORTAGE_THRESHOLD = 75.0;

struct AttendanceRow {
    string roll;
    string courseCode;
    string date;
    string status; // P / A / L
};

// Iterates enrolled students for a course, prompts P/A/L for each,
// snapshots the current file to a backup vector before writing.
void markAttendance(const string &courseCode, const string &date);

// Computes (present + 0.5*late) / total * 100 using an accumulator loop.
double getAttendancePct(const string &roll, const string &courseCode);

// Returns rolls (with course) whose attendance % is below the threshold.
vector<AttendanceRow> getShortageList(const string &courseCode);

// Restores attendance_log.txt from the last backup snapshot. False if none.
bool undoLastSession();

// Prints a formatted console table for a course on a given date.
void printDailySheet(const string &courseCode, const string &date);

#endif
