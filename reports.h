#ifndef REPORTS_H
#define REPORTS_H

#include <string>
using namespace std;

// All active students sorted by CGPA descending, printed with rank column.
void printMeritList();

// Students with any course attendance below 75%, with roll/name/course/pct.
void printAttendanceDefaulters();

// All fee defaulters with outstanding amount and weeks overdue.
void printFeeDefaulters();

// Full result sheet: grade, GPA, attendance status per student, for a semester.
void printSemesterResult(const string &semester);

// Groups students by department: count, average CGPA, pass rate.
void printDepartmentSummary();

// Redirects cout to the given report function's output into a file.
void exportReportToFile(const string &reportName, const string &outputFile);

#endif
