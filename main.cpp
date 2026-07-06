#include <iostream>
#include <limits>
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include "reports.h"
using namespace std;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ---------------- Student Menu ----------------
void studentMenu() {
    int choice;
    do {
        cout << "\n----- Student Management -----" << endl;
        cout << "1. Add Student" << endl;
        cout << "2. Search by Roll" << endl;
        cout << "3. Search by Name" << endl;
        cout << "4. Update Student" << endl;
        cout << "5. Soft Delete Student" << endl;
        cout << "6. List Active Students" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            string roll, name, dept;
            double cgpa;
            cout << "Roll (BSAI-YY-XXX): "; cin >> roll;
            cout << "Name: "; cin.ignore(); getline(cin, name);
            cout << "Department: "; getline(cin, dept);
            cout << "CGPA: "; cin >> cgpa;
            if (addStudent(roll, name, dept, cgpa)) {
                cout << "Student added successfully." << endl;
            }
        } else if (choice == 2) {
            string roll;
            cout << "Roll: "; cin >> roll;
            Student s = searchByRoll(roll);
            if (s.roll.empty()) {
                cout << "Student not found." << endl;
            } else {
                cout << s.roll << " | " << s.name << " | " << s.dept << " | "
                     << s.cgpa << " | " << s.status << endl;
            }
        } else if (choice == 3) {
            string name;
            cout << "Name (or partial): "; cin.ignore(); getline(cin, name);
            vector<Student> results = searchByName(name);
            for (int i = 0; i < (int)results.size(); i++) {
                cout << results[i].roll << " | " << results[i].name << endl;
            }
            if (results.empty()) cout << "No matches found." << endl;
        } else if (choice == 4) {
            string roll, field, value;
            cout << "Roll: "; cin >> roll;
            cout << "Field to update (name/dept/cgpa/status): "; cin >> field;
            cout << "New value: "; cin.ignore(); getline(cin, value);
            updateStudent(roll, field, value);
        } else if (choice == 5) {
            string roll;
            cout << "Roll: "; cin >> roll;
            softDelete(roll);
        } else if (choice == 6) {
            vector<Student> list = listActiveStudents();
            for (int i = 0; i < (int)list.size(); i++) {
                cout << list[i].roll << " | " << list[i].name << " | CGPA " << list[i].cgpa << endl;
            }
        } else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }

        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

// ---------------- Course Menu ----------------
void courseMenu() {
    int choice;
    do {
        cout << "\n----- Course Management -----" << endl;
        cout << "1. Enroll Student" << endl;
        cout << "2. Drop Course" << endl;
        cout << "3. Check Credit Load" << endl;
        cout << "4. List Enrolled Students" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            string roll, code, semester;
            cout << "Roll: "; cin >> roll;
            cout << "Course code: "; cin >> code;
            cout << "Semester: "; cin >> semester;
            EnrollResult result = enrollStudent(roll, code, semester);
            cout << enrollResultMessage(result) << endl;
        } else if (choice == 2) {
            string roll, code, semester;
            cout << "Roll: "; cin >> roll;
            cout << "Course code: "; cin >> code;
            cout << "Semester: "; cin >> semester;
            dropCourse(roll, code, semester);
        } else if (choice == 3) {
            string roll, semester;
            cout << "Roll: "; cin >> roll;
            cout << "Semester: "; cin >> semester;
            cout << "Credit load: " << getCreditLoad(roll, semester) << endl;
        } else if (choice == 4) {
            string code;
            cout << "Course code: "; cin >> code;
            vector<string> rolls = listEnrolledStudents(code);
            for (int i = 0; i < (int)rolls.size(); i++) {
                cout << rolls[i] << endl;
            }
        } else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }

        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

// ---------------- Attendance Menu ----------------
void attendanceMenu() {
    int choice;
    do {
        cout << "\n----- Attendance Management -----" << endl;
        cout << "1. Mark Attendance" << endl;
        cout << "2. View Attendance %" << endl;
        cout << "3. Undo Last Session" << endl;
        cout << "4. Print Daily Sheet" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            string code, date;
            cout << "Course code: "; cin >> code;
            cout << "Date (DD-MM-YYYY): "; cin >> date;
            markAttendance(code, date);
        } else if (choice == 2) {
            string roll, code;
            cout << "Roll: "; cin >> roll;
            cout << "Course code: "; cin >> code;
            cout << "Attendance %: " << getAttendancePct(roll, code) << endl;
        } else if (choice == 3) {
            undoLastSession();
        } else if (choice == 4) {
            string code, date;
            cout << "Course code: "; cin >> code;
            cout << "Date (DD-MM-YYYY): "; cin >> date;
            printDailySheet(code, date);
        } else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }

        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

// ---------------- Grades Menu ----------------
void gradesMenu() {
    int choice;
    do {
        cout << "\n----- Grades Management -----" << endl;
        cout << "1. Enter Marks" << endl;
        cout << "2. Compute GPA" << endl;
        cout << "3. Class Statistics" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            string roll, code, semester;
            double quizzes[5];
            double asgn[3];
            double mid, final_;

            cout << "Roll: "; cin >> roll;
            cout << "Course code: "; cin >> code;
            cout << "Semester: "; cin >> semester;
            cout << "Enter up to 5 quiz marks: ";
            for (int i = 0; i < 5; i++) cin >> quizzes[i];
            cout << "Enter 3 assignment marks: ";
            for (int i = 0; i < 3; i++) cin >> asgn[i];
            cout << "Mid marks (out of 40): "; cin >> mid;
            cout << "Final marks (out of 60): "; cin >> final_;

            enterMarks(roll, code, semester, quizzes, 5, asgn, 3, mid, final_);
            cout << "Marks recorded." << endl;
        } else if (choice == 2) {
            string roll, semester;
            cout << "Roll: "; cin >> roll;
            cout << "Semester: "; cin >> semester;
            cout << "GPA: " << computeGPA(roll, semester) << endl;
        } else if (choice == 3) {
            string code, semester;
            cout << "Course code: "; cin >> code;
            cout << "Semester: "; cin >> semester;
            Stats s = computeClassState(code, semester);
            cout << "Highest: " << s.highest << " Lowest: " << s.lowest
                 << " Mean: " << s.mean << " Median: " << s.median << endl;
        } else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }

        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

// ---------------- Fees Menu ----------------
void feesMenu() {
    int choice;
    do {
        cout << "\n----- Fee Management -----" << endl;
        cout << "1. Record Payment" << endl;
        cout << "2. Compute Late Fine" << endl;
        cout << "3. Generate Receipt" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            string roll, semester, date;
            double amount;
            cout << "Roll: "; cin >> roll;
            cout << "Semester: "; cin >> semester;
            cout << "Amount: "; cin >> amount;
            cout << "Paid date (DD-MM-YYYY): "; cin >> date;
            recordPayment(roll, semester, amount, date);
        } else if (choice == 2) {
            string roll, semester;
            cout << "Roll: "; cin >> roll;
            cout << "Semester: "; cin >> semester;
            cout << "Late fine: " << computeLateFine(roll, semester) << endl;
        } else if (choice == 3) {
            string roll, semester;
            cout << "Roll: "; cin >> roll;
            cout << "Semester: "; cin >> semester;
            generateReceipt(roll, semester);
        } else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }

        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

// ---------------- Reports Menu ----------------
void reportsMenu() {
    int choice;
    do {
        cout << "\n----- Reports -----" << endl;
        cout << "1. Merit List" << endl;
        cout << "2. Attendance Defaulters" << endl;
        cout << "3. Fee Defaulters" << endl;
        cout << "4. Semester Result" << endl;
        cout << "5. Department Summary" << endl;
        cout << "6. Export Report to File" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            printMeritList();
        } else if (choice == 2) {
            printAttendanceDefaulters();
        } else if (choice == 3) {
            printFeeDefaulters();
        } else if (choice == 4) {
            string semester;
            cout << "Semester: "; cin >> semester;
            printSemesterResult(semester);
        } else if (choice == 5) {
            printDepartmentSummary();
        } else if (choice == 6) {
            string reportName, outputFile;
            cout << "Report (merit/attendance/fees/department): "; cin >> reportName;
            cout << "Output filename: "; cin >> outputFile;
            exportReportToFile(reportName, outputFile);
        } else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }

        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

// ---------------- Main Menu ----------------
int main() {
    int choice;

    cout << "=====================================================" << endl;
    cout << "        CAMPUS ANALYTICS ENGINE - BS AI" << endl;
    cout << "=====================================================" << endl;

    do {
        cout << "\n================ MAIN MENU ================" << endl;
        cout << "1. Student Management" << endl;
        cout << "2. Course Management" << endl;
        cout << "3. Attendance Management" << endl;
        cout << "4. Grades Management" << endl;
        cout << "5. Fee Management" << endl;
        cout << "6. Reports" << endl;
        cout << "0. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: studentMenu(); break;
            case 2: courseMenu(); break;
            case 3: attendanceMenu(); break;
            case 4: gradesMenu(); break;
            case 5: feesMenu(); break;
            case 6: reportsMenu(); break;
            case 0: cout << "Exiting Campus Analytics Engine. Goodbye!" << endl; break;
            default: cout << "Invalid choice, try again." << endl;
        }

    } while (choice != 0);

    return 0;
}
