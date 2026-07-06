#ifndef COURSE_OPS_H
#define COURSE_OPS_H

#include <vector>
#include <string>
using namespace std;

const string COURSES_FILE = "courses.txt";
const string ENROLLMENTS_FILE = "enrollments.txt";
const int MAX_CREDIT_LOAD = 21;

enum EnrollResult {
    ENROLL_SUCCESS,
    ERR_STUDENT_INACTIVE,
    ERR_COURSE_NOT_FOUND,
    ERR_NO_SEATS,
    ERR_ALREADY_ENROLLED,
    ERR_CREDIT_OVERLOAD,
    ERR_PREREQ_NOT_MET
};

struct Course {
    string code;
    string title;
    int credits;
    int seats;
    string prereq;
};

// Prints a human readable message for an EnrollResult value.
string enrollResultMessage(EnrollResult result);

// Loads a course row from courses.txt by its code. Empty code if not found.
Course getCourse(const string &code);

// Counts how many students are currently enrolled (status=enrolled) in a course.
int countEnrolled(const string &courseCode);

// Runs all required checks, then appends an enrollment row if everything passes.
EnrollResult enrollStudent(const string &roll, const string &courseCode, const string &semester);

// Marks an enrollment as dropped, but only if no attendance rows exist for it.
bool dropCourse(const string &roll, const string &courseCode, const string &semester);

// Sums credit hours of all active enrollments for the student in a semester.
int getCreditLoad(const string &roll, const string &semester);

// Looks at the course's prereq field; if not NONE, checks grades.txt for a pass.
bool checkPrerequisite(const string &roll, const string &courseCode);

// Returns rolls of all students actively enrolled in the given course.
vector<string> listEnrolledStudents(const string &courseCode);

#endif
