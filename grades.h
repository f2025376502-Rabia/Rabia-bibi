#ifndef GRADES_H
#define GRADES_H

#include <vector>
#include <string>
using namespace std;

const string GRADES_FILE_PATH = "grades.txt";

struct Stats {
    double highest;
    double lowest;
    double mean;
    double median;
};

// Records marks for a student in a course/semester, computes the totals,
// and appends them to grades.txt.
bool enterMarks(const string &roll, const string &courseCode, const string &semester,
                double quizzes[], int quizCount, double asgn[], int asgnCount,
                double mid, double final_);

// Finds and excludes the two lowest of up to 5 quiz marks using a loop
// (no sort). Returns the average of the remaining. Handles n < 3.
double bestThreeOfFive(double marks[], int n);

// quiz*0.10 + asgn*0.10 + mid*0.30 + final*0.50
double computeWeightedTotal(double quizAvg, double asgnAvg, double mid, double final_);

// Maps a numeric total to a letter grade.
string getLetterGrade(double total);

// Credit-weighted average of grade points across all courses in a semester.
double computeGPA(const string &roll, const string &semester);

// Returns highest, lowest, mean and median total score for a course.
Stats computeClassState(const string &courseCode, const string &semester);

// If attendance % is below 75, overrides the stored letter grade to F.
void applyAttendancePenalty(const string &roll, const string &courseCode, const string &semester);

// Converts a letter grade to grade points (used internally by computeGPA).
double letterToPoints(const string &letter);

#endif
