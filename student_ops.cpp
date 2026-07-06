#include "student_ops.h"
#include "filehandler.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
using namespace std;

bool isValidRollFormat(const string &roll) {
    // Expected shape: BSAI-YY-XXX  (11 characters)
    if (roll.length() != 11) return false;

    if (roll.substr(0, 4) != "BSAI") return false;
    if (roll[4] != '-') return false;
    if (roll[7] != '-') return false;

    // YY digits
    for (int i = 5; i <= 6; i++) {
        if (roll[i] < '0' || roll[i] > '9') return false;
    }
    // XXX digits
    for (int i = 8; i <= 10; i++) {
        if (roll[i] < '0' || roll[i] > '9') return false;
    }
    return true;
}

bool isValidName(const string &name) {
    if (name.length() == 0) return false;
    for (int i = 0; i < (int)name.length(); i++) {
        if (name[i] >= '0' && name[i] <= '9') return false;
    }
    return true;
}

Student rowToStudent(const vector<string> &row) {
    Student s;
    if (row.size() < 5) {
        s.roll = "";
        s.name = "";
        s.dept = "";
        s.cgpa = 0.0;
        s.status = "";
        return s;
    }
    s.roll = row[0];
    s.name = row[1];
    s.dept = row[2];
    s.cgpa = atof(row[3].c_str());
    s.status = row[4];
    return s;
}

bool addStudent(const string &roll, const string &name, const string &dept, double cgpa) {
    if (!isValidRollFormat(roll)) {
        cout << "Invalid roll format. Expected BSAI-YY-XXX." << endl;
        return false;
    }
    if (!isValidName(name)) {
        cout << "Invalid name: names cannot contain digits." << endl;
        return false;
    }
    if (cgpa < 0.0 || cgpa > 4.0) {
        cout << "Invalid CGPA: must be between 0.0 and 4.0." << endl;
        return false;
    }
    if (rowExists(STUDENTS_FILE, 0, roll)) {
        cout << "A student with roll " << roll << " already exists." << endl;
        return false;
    }

    ostringstream cgpaStream;
    cgpaStream << cgpa;

    vector<string> newRow;
    newRow.push_back(roll);
    newRow.push_back(name);
    newRow.push_back(dept);
    newRow.push_back(cgpaStream.str());
    newRow.push_back("active");

    appendTXT(STUDENTS_FILE, newRow);
    return true;
}

Student searchByRoll(const string &roll) {
    vector<string> row = findRow(STUDENTS_FILE, 0, roll);
    return rowToStudent(row);
}

vector<Student> searchByName(const string &partialName) {
    vector<Student> matches;
    vector<vector<string>> rows = readTXT(STUDENTS_FILE);

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 2) continue;
        string name = rows[i][1];

        // substring search without STL find/algorithm
        bool found = false;
        int nLen = (int)name.length();
        int pLen = (int)partialName.length();
        if (pLen == 0) {
            found = true;
        }
        for (int start = 0; start + pLen <= nLen && !found; start++) {
            bool match = true;
            for (int k = 0; k < pLen; k++) {
                if (name[start + k] != partialName[k]) {
                    match = false;
                    break;
                }
            }
            if (match) found = true;
        }

        if (found) {
            matches.push_back(rowToStudent(rows[i]));
        }
    }
    return matches;
}

bool updateStudent(const string &roll, const string &fieldName, const string &newValue) {
    if (fieldName == "roll") {
        cout << "Roll number cannot be updated." << endl;
        return false;
    }

    vector<vector<string>> rows = readTXT(STUDENTS_FILE);
    vector<string> header = readHeader(STUDENTS_FILE);

    int colIndex = -1;
    if (fieldName == "name") colIndex = 1;
    else if (fieldName == "dept") colIndex = 2;
    else if (fieldName == "cgpa") colIndex = 3;
    else if (fieldName == "status") colIndex = 4;
    else {
        cout << "Unknown field: " << fieldName << endl;
        return false;
    }

    bool found = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i][0] == roll) {
            rows[i][colIndex] = newValue;
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "No student found with roll " << roll << endl;
        return false;
    }

    writeTXT(STUDENTS_FILE, header, rows);
    return true;
}

bool softDelete(const string &roll) {
    return updateStudent(roll, "status", "inactive");
}

vector<Student> listActiveStudents() {
    vector<Student> active;
    vector<vector<string>> rows = readTXT(STUDENTS_FILE);

    for (int i = 0; i < (int)rows.size(); i++) {
        Student s = rowToStudent(rows[i]);
        if (s.status == "active") {
            active.push_back(s);
        }
    }

    // Selection sort ascending by roll
    int n = (int)active.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (active[j].roll < active[minIdx].roll) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            Student temp = active[i];
            active[i] = active[minIdx];
            active[minIdx] = temp;
        }
    }

    return active;
}
