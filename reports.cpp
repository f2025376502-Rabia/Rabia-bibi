#include "reports.h"
#include "student_ops.h"
#include "attendance.h"
#include "fee_tracker.h"
#include "grades.h"
#include "course_ops.h"
#include "filehandler.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

void printMeritList() {
    vector<Student> students = listActiveStudents();

    // Sort descending by CGPA (selection sort, reusing simple logic here).
    int n = (int)students.size();
    for (int i = 0; i < n - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (students[j].cgpa > students[maxIdx].cgpa) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            Student temp = students[i];
            students[i] = students[maxIdx];
            students[maxIdx] = temp;
        }
    }

    cout << "\n==================== MERIT LIST ====================" << endl;
    cout << left << setw(6) << "Rank" << setw(14) << "Roll" << setw(20) << "Name" << setw(8) << "CGPA" << endl;
    cout << "-----------------------------------------------------" << endl;

    for (int i = 0; i < n; i++) {
        cout << left << setw(6) << (i + 1) << setw(14) << students[i].roll
             << setw(20) << students[i].name << setw(8) << fixed << setprecision(2) << students[i].cgpa << endl;
    }
}

void printAttendanceDefaulters() {
    vector<vector<string>> courseRows = readTXT(COURSES_FILE);

    cout << "\n============ ATTENDANCE DEFAULTERS ============" << endl;
    cout << left << setw(14) << "Roll" << setw(20) << "Name" << setw(10) << "Course" << setw(10) << "Percent" << endl;
    cout << "-------------------------------------------------" << endl;

    bool any = false;
    for (int c = 0; c < (int)courseRows.size(); c++) {
        string courseCode = courseRows[c][0];
        vector<AttendanceRow> shortages = getShortageList(courseCode);

        for (int i = 0; i < (int)shortages.size(); i++) {
            Student s = searchByRoll(shortages[i].roll);
            double pct = getAttendancePct(shortages[i].roll, courseCode);

            cout << left << setw(14) << shortages[i].roll << setw(20) << s.name
                 << setw(10) << courseCode << setw(10) << fixed << setprecision(1) << pct << endl;
            any = true;
        }
    }

    if (!any) {
        cout << "No attendance defaulters found." << endl;
    }
}

void printFeeDefaulters() {
    vector<Defaulter> defaulters = getDefaulters();

    cout << "\n================ FEE DEFAULTERS ================" << endl;
    cout << left << setw(14) << "Roll" << setw(14) << "Balance" << setw(14) << "WeeksOverdue" << endl;
    cout << "-------------------------------------------------" << endl;

    for (int i = 0; i < (int)defaulters.size(); i++) {
        cout << left << setw(14) << defaulters[i].roll
             << setw(14) << fixed << setprecision(2) << defaulters[i].balance
             << setw(14) << defaulters[i].weeksOverdue << endl;
    }

    if (defaulters.empty()) {
        cout << "No fee defaulters found." << endl;
    }
}

void printSemesterResult(const string &semester) {
    vector<Student> students = listActiveStudents();

    cout << "\n=========================== SEMESTER RESULT: " << semester << " ===========================" << endl;
    cout << left << setw(14) << "Roll" << setw(20) << "Name" << setw(10) << "GPA" << setw(12) << "Attendance" << endl;
    cout << "--------------------------------------------------------------------------" << endl;

    for (int i = 0; i < (int)students.size(); i++) {
        double gpa = computeGPA(students[i].roll, semester);
        cout << left << setw(14) << students[i].roll << setw(20) << students[i].name
             << setw(10) << fixed << setprecision(2) << gpa << setw(12) << "See course" << endl;
    }
}

void printDepartmentSummary() {
    vector<Student> students = listActiveStudents();

    // Parallel arrays to group by department without a map.
    string deptNames[100];
    int deptCount[100];
    double deptCgpaSum[100];
    int passCount[100];
    int numDepts = 0;

    for (int i = 0; i < (int)students.size(); i++) {
        string dept = students[i].dept;

        int idx = -1;
        for (int d = 0; d < numDepts; d++) {
            if (deptNames[d] == dept) {
                idx = d;
                break;
            }
        }

        if (idx == -1) {
            idx = numDepts;
            deptNames[idx] = dept;
            deptCount[idx] = 0;
            deptCgpaSum[idx] = 0.0;
            passCount[idx] = 0;
            numDepts++;
        }

        deptCount[idx]++;
        deptCgpaSum[idx] += students[i].cgpa;
        if (students[i].cgpa >= 2.0) {
            passCount[idx]++;
        }
    }

    cout << "\n================ DEPARTMENT SUMMARY ================" << endl;
    cout << left << setw(12) << "Dept" << setw(10) << "Count" << setw(12) << "AvgCGPA" << setw(12) << "PassRate%" << endl;
    cout << "------------------------------------------------------" << endl;

    for (int d = 0; d < numDepts; d++) {
        double avgCgpa = deptCgpaSum[d] / (double)deptCount[d];
        double passRate = (double)passCount[d] / (double)deptCount[d] * 100.0;

        cout << left << setw(12) << deptNames[d] << setw(10) << deptCount[d]
             << setw(12) << fixed << setprecision(2) << avgCgpa
             << setw(12) << fixed << setprecision(1) << passRate << endl;
    }
}

void exportReportToFile(const string &reportName, const string &outputFile) {
    // Redirect cout's buffer to a file, run the chosen report, then restore.
    ofstream outFile(outputFile.c_str());
    if (!outFile.is_open()) {
        cout << "Could not open " << outputFile << " for writing." << endl;
        return;
    }

    streambuf *originalBuf = cout.rdbuf();
    cout.rdbuf(outFile.rdbuf());

    if (reportName == "merit") {
        printMeritList();
    } else if (reportName == "attendance") {
        printAttendanceDefaulters();
    } else if (reportName == "fees") {
        printFeeDefaulters();
    } else if (reportName == "department") {
        printDepartmentSummary();
    } else {
        cout << "Unknown report name: " << reportName << endl;
    }

    cout.rdbuf(originalBuf);
    outFile.close();

    cout << "Report exported to " << outputFile << endl;
}
