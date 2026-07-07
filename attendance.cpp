#include "attendance.h"
#include "course_ops.h"
#include "filehandler.h"
#include <iostream>
#include <iomanip>
using namespace std;

// Handles student attendance tracking and logging
// Holds the pre-session snapshot in memory so undoLastSession can restore it.
static vector<vector<string>> g_backupRows;
static vector<string> g_backupHeader;
static bool g_hasBackup = false;

void markAttendance(const string &courseCode, const string &date) {
    // Snapshot current state before making any changes.
    g_backupRows = readTXT(ATTENDANCE_FILE);
    g_backupHeader = readHeader(ATTENDANCE_FILE);
    g_hasBackup = true;

    vector<string> rolls = listEnrolledStudents(courseCode);

    if (rolls.empty()) {
        cout << "No students enrolled in " << courseCode << "." << endl;
        return;
    }

    for (int i = 0; i < (int)rolls.size(); i++) {
        string status;
        cout << "Attendance for " << rolls[i] << " (P/A/L): ";
        cin >> status;

        while (status != "P" && status != "A" && status != "L") {
            cout << "Invalid entry. Enter P, A, or L: ";
            cin >> status;
        }

        vector<string> row;
        row.push_back(rolls[i]);
        row.push_back(courseCode);
        row.push_back(date);
        row.push_back(status);
        appendTXT(ATTENDANCE_FILE, row);
    }

    cout << "Attendance recorded for " << rolls.size() << " student(s)." << endl;
}

double getAttendancePct(const string &roll, const string &courseCode) {
    vector<vector<string>> rows = readTXT(ATTENDANCE_FILE);

    double present = 0.0;
    double late = 0.0;
    int total = 0;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 4) continue;
        if (rows[i][0] != roll || rows[i][1] != courseCode) continue;

        total++;
        if (rows[i][3] == "P") present += 1.0;
        else if (rows[i][3] == "L") late += 1.0;
    }

    if (total == 0) return 0.0;

    return (present + 0.5 * late) / (double)total * 100.0;
}

vector<AttendanceRow> getShortageList(const string &courseCode) {
    vector<AttendanceRow> shortages;
    vector<string> rolls = listEnrolledStudents(courseCode);

    for (int i = 0; i < (int)rolls.size(); i++) {
        double pct = getAttendancePct(rolls[i], courseCode);
        if (pct < SHORTAGE_THRESHOLD) {
            AttendanceRow r;
            r.roll = rolls[i];
            r.courseCode = courseCode;
            r.date = "";
            r.status = "";
            shortages.push_back(r);
        }
    }
    return shortages;
}

bool undoLastSession() {
    if (!g_hasBackup) {
        cout << "No backup available to undo." << endl;
        return false;
    }
    writeTXT(ATTENDANCE_FILE, g_backupHeader, g_backupRows);
    g_hasBackup = false;
    cout << "Last attendance session undone." << endl;
    return true;
}

void printDailySheet(const string &courseCode, const string &date) {
    vector<vector<string>> rows = readTXT(ATTENDANCE_FILE);

    cout << "\n---- Daily Attendance Sheet ----" << endl;
    cout << "Course: " << courseCode << "  Date: " << date << endl;
    cout << left << setw(15) << "Roll" << setw(10) << "Status" << endl;
    cout << "----------------------------------" << endl;

    bool any = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 4) continue;
        if (rows[i][1] == courseCode && rows[i][2] == date) {
            cout << left << setw(15) << rows[i][0] << setw(10) << rows[i][3] << endl;
            any = true;
        }
    }

    if (!any) {
        cout << "No attendance records found for this date." << endl;
    }
}
