# Campus Analytics Engine

A multi-file, menu-driven C++ data analytics system for a fictional university campus.
Built for the **Programming Fundamentals** course (BS AI) — pure C++, no OOP, no STL algorithms.

## Features

- **Student Management** — add, search (by roll or name), update, and soft-delete student records with roll-format and CGPA validation.
- **Course Management** — enrollment with prerequisite/seat/credit-load checks, course dropping, and credit-load reporting.
- **Attendance Tracking** — per-session P/A/L marking, attendance percentage calculation, shortage detection (<75%), and single-session undo.
- **Grades** — quiz best-3-of-5 scoring, weighted totals, letter grades, semester GPA, and class statistics (highest/lowest/mean/median).
- **Fee Tracking** — payment recording, manual date-arithmetic late-fine calculation (no `<ctime>`), and formatted receipts.
- **Reports** — merit list, attendance defaulters, fee defaulters, semester result sheets, department summaries, and file export.

All data is persisted in plain `.txt` files and re-read on every operation — nothing is hardcoded.

## File Structure

```
main.cpp            Entry point, 3-level nested menu
filehandler.h/.cpp   Generic TXT read/write/append/search helpers
student_ops.h/.cpp   Student CRUD + roll validation
course_ops.h/.cpp    Enrollment, drop, prerequisite & credit checks
attendance.h/.cpp    Attendance marking, percentage, undo
grades.h/.cpp        Marks entry, GPA, class statistics
fee_tracker.h/.cpp   Payments, late fines, receipts
reports.h/.cpp       All console/file reports

students.txt         Student records
courses.txt          Course catalogue
enrollments.txt      Enrollment records
attendance_log.txt   Attendance history
fees.txt             Fee/payment records
grades.txt           Stored letter grades & totals (supports GPA + prerequisite checks)
```

## How to Compile & Run

Requires a C++11-compatible compiler (e.g. g++).

```bash
g++ -std=c++11 -o campus_engine main.cpp filehandler.cpp student_ops.cpp course_ops.cpp attendance.cpp grades.cpp fee_tracker.cpp reports.cpp
./campus_engine
```

On Windows (MinGW / g++ in PowerShell):

```powershell
g++ -std=c++11 -o campus_engine.exe main.cpp filehandler.cpp student_ops.cpp course_ops.cpp attendance.cpp grades.cpp fee_tracker.cpp reports.cpp
.\campus_engine.exe
```

Make sure all `.txt` data files stay in the same folder as the executable, since they're read/written using relative paths.

## Sample Run

```
=====================================================
        CAMPUS ANALYTICS ENGINE - BS AI
=====================================================

================ MAIN MENU ================
1. Student Management
2. Course Management
3. Attendance Management
4. Grades Management
5. Fee Management
6. Reports
0. Exit
Choice: 6

----- Reports -----
1. Merit List
...
Choice: 1

==================== MERIT LIST ====================
Rank  Roll          Name                CGPA
-----------------------------------------------------
1     BSAI-23-002   Sana Malik          3.80
2     BSAI-23-001   Ahmed Raza          3.45
3     BSAI-23-004   Ayesha Noor         3.20
4     BSAI-23-003   Bilal Hussain       2.90
```

## Notes on Design Choices

- `grades.txt` is used internally as the store of computed letter grades, since prerequisite checks and GPA calculation both need persisted grade history.
- All searching/sorting (selection sort, bubble sort, manual substring matching) is implemented by hand — no `<algorithm>` header is used anywhere.
- Department grouping uses parallel arrays with nested loops instead of `std::map`.
- Date arithmetic for late fees is done with a manual day-count function and a hardcoded month-length table (leap years included) — no `<ctime>`.
