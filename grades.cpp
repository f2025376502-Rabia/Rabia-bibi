#include "grades.h"
#include "attendance.h"
#include "student_ops.h"
#include "course_ops.h"
#include "filehandler.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
using namespace std;

// grades.txt columns: roll, course_code, semester, letter_grade, total_score

double bestThreeOfFive(double marks[], int n) {
    if (n <= 0) return 0.0;
    if (n < 3) {
        double sum = 0.0;
        for (int i = 0; i < n; i++) sum += marks[i];
        return sum / (double)n;
    }

    // Copy so we don't disturb the caller's array.
    double temp[5];
    for (int i = 0; i < n; i++) temp[i] = marks[i];

    // Find and "remove" the two lowest values by marking them used.
    bool used[5] = {false, false, false, false, false};

    for (int removeCount = 0; removeCount < 2; removeCount++) {
        int lowestIdx = -1;
        for (int i = 0; i < n; i++) {
            if (used[i]) continue;
            if (lowestIdx == -1 || temp[i] < temp[lowestIdx]) {
                lowestIdx = i;
            }
        }
        if (lowestIdx != -1) used[lowestIdx] = true;
    }

    double sum = 0.0;
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (!used[i]) {
            sum += temp[i];
            count++;
        }
    }

    if (count == 0) return 0.0;
    return sum / (double)count;
}

double computeWeightedTotal(double quizAvg, double asgnAvg, double mid, double final_) {
    return quizAvg * 0.10 + asgnAvg * 0.10 + mid * 0.30 + final_ * 0.50;
}

string getLetterGrade(double total) {
    if (total >= 85) return "A";
    if (total >= 80) return "B+";
    if (total >= 70) return "B";
    if (total >= 65) return "C+";
    if (total >= 60) return "C";
    if (total >= 50) return "D";
    return "F";
}

double letterToPoints(const string &letter) {
    if (letter == "A") return 4.0;
    if (letter == "B+") return 3.4;
    if (letter == "B") return 3.0;
    if (letter == "C+") return 2.4;
    if (letter == "C") return 2.0;
    if (letter == "D") return 1.0;
    return 0.0; // F
}

bool enterMarks(const string &roll, const string &courseCode, const string &semester,
                double quizzes[], int quizCount, double asgn[], int asgnCount,
                double mid, double final_) {

    if (mid < 0 || mid > 40) {
        cout << "Invalid mid marks (must be 0-40)." << endl;
        return false;
    }
    if (final_ < 0 || final_ > 60) {
        cout << "Invalid final marks (must be 0-60)." << endl;
        return false;
    }
    for (int i = 0; i < quizCount; i++) {
        if (quizzes[i] < 0 || quizzes[i] > 100) {
            cout << "Invalid quiz mark at index " << i << endl;
            return false;
        }
    }

    double quizAvg = bestThreeOfFive(quizzes, quizCount);

    double asgnSum = 0.0;
    for (int i = 0; i < asgnCount; i++) asgnSum += asgn[i];
    double asgnAvg = (asgnCount > 0) ? (asgnSum / (double)asgnCount) : 0.0;

    double total = computeWeightedTotal(quizAvg, asgnAvg, mid, final_);
    string letter = getLetterGrade(total);

    ostringstream totalStream;
    totalStream << total;

    vector<string> row;
    row.push_back(roll);
    row.push_back(courseCode);
    row.push_back(semester);
    row.push_back(letter);
    row.push_back(totalStream.str());

    appendTXT(GRADES_FILE_PATH, row);

    applyAttendancePenalty(roll, courseCode, semester);

    return true;
}

void applyAttendancePenalty(const string &roll, const string &courseCode, const string &semester) {
    double pct = getAttendancePct(roll, courseCode);
    if (pct >= SHORTAGE_THRESHOLD) return;

    vector<vector<string>> rows = readTXT(GRADES_FILE_PATH);
    vector<string> header = readHeader(GRADES_FILE_PATH);

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i][0] == roll && rows[i][1] == courseCode && rows[i][2] == semester) {
            rows[i][3] = "F";
        }
    }

    writeTXT(GRADES_FILE_PATH, header, rows);
}

double computeGPA(const string &roll, const string &semester) {
    vector<vector<string>> gradeRows = readTXT(GRADES_FILE_PATH);
    vector<vector<string>> courseRows = readTXT(COURSES_FILE);

    double pointSum = 0.0;
    int creditSum = 0;

    for (int i = 0; i < (int)gradeRows.size(); i++) {
        if (gradeRows[i].size() < 4) continue;
        if (gradeRows[i][0] != roll || gradeRows[i][2] != semester) continue;

        string courseCode = gradeRows[i][1];
        int credits = 0;
        for (int j = 0; j < (int)courseRows.size(); j++) {
            if (courseRows[j][0] == courseCode) {
                credits = atoi(courseRows[j][2].c_str());
                break;
            }
        }

        double points = letterToPoints(gradeRows[i][3]);
        pointSum += points * credits;
        creditSum += credits;
    }

    if (creditSum == 0) return 0.0;
    return pointSum / (double)creditSum;
}

Stats computeClassState(const string &courseCode, const string &semester) {
    vector<vector<string>> gradeRows = readTXT(GRADES_FILE_PATH);
    vector<double> totals;

    for (int i = 0; i < (int)gradeRows.size(); i++) {
        if (gradeRows[i].size() < 5) continue;
        if (gradeRows[i][1] != courseCode || gradeRows[i][2] != semester) continue;
        totals.push_back(atof(gradeRows[i][4].c_str()));
    }

    Stats stats;
    stats.highest = 0.0;
    stats.lowest = 0.0;
    stats.mean = 0.0;
    stats.median = 0.0;

    int n = (int)totals.size();
    if (n == 0) return stats;

    double sum = 0.0;
    double highest = totals[0];
    double lowest = totals[0];

    for (int i = 0; i < n; i++) {
        sum += totals[i];
        if (totals[i] > highest) highest = totals[i];
        if (totals[i] < lowest) lowest = totals[i];
    }

    stats.highest = highest;
    stats.lowest = lowest;
    stats.mean = sum / (double)n;

    // Bubble sort a copy to find the median (small n, fine to sort manually).
    vector<double> sorted = totals;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (sorted[j] > sorted[j + 1]) {
                double temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    if (n % 2 == 1) {
        stats.median = sorted[n / 2];
    } else {
        stats.median = (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
    }

    return stats;
}
