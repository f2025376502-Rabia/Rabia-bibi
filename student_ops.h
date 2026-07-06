#ifndef STUDENT_OPS_H
#define STUDENT_OPS_H

#include <vector>
#include <string>
using namespace std;

const string STUDENTS_FILE = "students.txt";

struct Student {
    string roll;
    string name;
    string dept;
    double cgpa;
    string status; // active / inactive
};

// Checks the BSAI-YY-XXX pattern using substr + char checks (no regex).
bool isValidRollFormat(const string &roll);

// Checks the name contains no digit characters.
bool isValidName(const string &name);

// Validates, checks duplicates, appends a new student row to students.txt.
bool addStudent(const string &roll, const string &name, const string &dept, double cgpa);

// Returns the student with the given roll, or a Student with empty roll if not found.
Student searchByRoll(const string &roll);

// Returns every student whose name contains the given substring.
vector<Student> searchByName(const string &partialName);

// Loads students.txt, finds the row, updates one field (not roll), rewrites file.
bool updateStudent(const string &roll, const string &fieldName, const string &newValue);

// Marks the given student's status as inactive without removing the row.
bool softDelete(const string &roll);

// Returns all active students, sorted ascending by roll using selection sort.
vector<Student> listActiveStudents();

// Helper: converts a raw text row into a Student struct.
Student rowToStudent(const vector<string> &row);

#endif
