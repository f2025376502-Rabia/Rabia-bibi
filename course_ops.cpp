#include "course_ops.h"
#include "student_ops.h"
#include "filehandler.h"
#include <iostream>
#include <cstdlib>
using namespace std;

// grades.txt is used only to verify prerequisites (see checkPrerequisite).
const string GRADES_FILE = "grades.txt";

string enrollResultMessage(EnrollResult result) {
    switch (result) {
        case ENROLL_SUCCESS: return "Enrollment successful.";
        case ERR_STUDENT_INACTIVE: return "Student is inactive.";
        case ERR_COURSE_NOT_FOUND: return "Course code does not exist.";
        case ERR_NO_SEATS: return "No seats remaining in this course.";
        case ERR_ALREADY_ENROLLED: return "Student is already enrolled in this course.";
        case ERR_CREDIT_OVERLOAD: return "Enrolling would exceed the 21-credit limit.";
        case ERR_PREREQ_NOT_MET: return "Prerequisite for this course has not been passed.";
        default: return "Unknown result.";
    }
}

Course getCourse(const string &code) {
    vector<string> row = findRow(COURSES_FILE, 0, code);
    Course c;
    if (row.empty()) {
        c.code = "";
        c.title = "";
        c.credits = 0;
        c.seats = 0;
        c.prereq = "";
        return c;
    }
    c.code = row[0];
    c.title = row[1];
    c.credits = atoi(row[2].c_str());
    c.seats = atoi(row[3].c_str());
    c.prereq = row[4];
    return c;
}

int countEnrolled(const string &courseCode) {
    vector<vector<string>> rows = readTXT(ENROLLMENTS_FILE);
    int count = 0;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 4 && rows[i][1] == courseCode && rows[i][3] == "enrolled") {
            count++;
        }
    }
    return count;
}

bool isAlreadyEnrolled(const string &roll, const string &courseCode, const string &semester) {
    vector<vector<string>> rows = readTXT(ENROLLMENTS_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 4 &&
            rows[i][0] == roll &&
            rows[i][1] == courseCode &&
            rows[i][2] == semester &&
            rows[i][3] == "enrolled") {
            return true;
        }
    }
    return false;
}

int getCreditLoad(const string &roll, const string &semester) {
    vector<vector<string>> enrollRows = readTXT(ENROLLMENTS_FILE);
    vector<vector<string>> courseRows = readTXT(COURSES_FILE);
    int total = 0;

    for (int i = 0; i < (int)enrollRows.size(); i++) {
        if (enrollRows[i].size() < 4) continue;
        if (enrollRows[i][0] != roll) continue;
        if (enrollRows[i][2] != semester) continue;
        if (enrollRows[i][3] != "enrolled") continue;

        string courseCode = enrollRows[i][1];
        for (int j = 0; j < (int)courseRows.size(); j++) {
            if (courseRows[j][0] == courseCode) {
                total += atoi(courseRows[j][2].c_str());
                break;
            }
        }
    }
    return total;
}

bool checkPrerequisite(const string &roll, const string &courseCode) {
    Course c = getCourse(courseCode);
    if (c.prereq == "NONE" || c.prereq.length() == 0) {
        return true;
    }

    vector<vector<string>> gradeRows = readTXT(GRADES_FILE);
    for (int i = 0; i < (int)gradeRows.size(); i++) {
        // Expected grades.txt layout: roll, course_code, semester, letter_grade
        if (gradeRows[i].size() < 4) continue;
        if (gradeRows[i][0] == roll && gradeRows[i][1] == c.prereq) {
            if (gradeRows[i][3] != "F") {
                return true;
            }
        }
    }
    return false;
}

EnrollResult enrollStudent(const string &roll, const string &courseCode, const string &semester) {
    Student s = searchByRoll(roll);
    if (s.roll.empty() || s.status != "active") {
        return ERR_STUDENT_INACTIVE;
    }

    Course c = getCourse(courseCode);
    if (c.code.empty()) {
        return ERR_COURSE_NOT_FOUND;
    }

    if (countEnrolled(courseCode) >= c.seats) {
        return ERR_NO_SEATS;
    }

    if (isAlreadyEnrolled(roll, courseCode, semester)) {
        return ERR_ALREADY_ENROLLED;
    }

    if (getCreditLoad(roll, semester) + c.credits > MAX_CREDIT_LOAD) {
        return ERR_CREDIT_OVERLOAD;
    }

    if (!checkPrerequisite(roll, courseCode)) {
        return ERR_PREREQ_NOT_MET;
    }

    vector<string> newRow;
    newRow.push_back(roll);
    newRow.push_back(courseCode);
    newRow.push_back(semester);
    newRow.push_back("enrolled");
    appendTXT(ENROLLMENTS_FILE, newRow);

    return ENROLL_SUCCESS;
}

bool dropCourse(const string &roll, const string &courseCode, const string &semester) {
    // Check attendance_log.txt for any existing rows tied to this enrollment.
    vector<vector<string>> attRows = readTXT("attendance_log.txt");
    for (int i = 0; i < (int)attRows.size(); i++) {
        if (attRows[i].size() >= 2 && attRows[i][0] == roll && attRows[i][1] == courseCode) {
            cout << "Cannot drop: attendance already recorded for this course." << endl;
            return false;
        }
    }

    vector<vector<string>> rows = readTXT(ENROLLMENTS_FILE);
    vector<string> header = readHeader(ENROLLMENTS_FILE);
    bool found = false;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i][0] == roll && rows[i][1] == courseCode && rows[i][2] == semester) {
            rows[i][3] = "dropped";
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "No matching enrollment found." << endl;
        return false;
    }

    writeTXT(ENROLLMENTS_FILE, header, rows);
    return true;
}

vector<string> listEnrolledStudents(const string &courseCode) {
    vector<string> rolls;
    vector<vector<string>> rows = readTXT(ENROLLMENTS_FILE);

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 4 && rows[i][1] == courseCode && rows[i][3] == "enrolled") {
            rolls.push_back(rows[i][0]);
        }
    }
    return rolls;
}
