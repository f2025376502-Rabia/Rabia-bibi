#include "fee_tracker.h"
#include "filehandler.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

// Handles fee tracking and payment records
// Days in each month for a non-leap year; adjusted for leap years in code.
static int monthLength(int month, int year) {
    int lengths[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeap) return 29;
    }
    return lengths[month - 1];
}

bool isValidDateFormat(const string &date) {
    if (date.length() != 10) return false;
    if (date[2] != '-' || date[5] != '-') return false;

    for (int i = 0; i < (int)date.length(); i++) {
        if (i == 2 || i == 5) continue;
        if (date[i] < '0' || date[i] > '9') return false;
    }

    int day = atoi(date.substr(0, 2).c_str());
    int month = atoi(date.substr(3, 2).c_str());
    int year = atoi(date.substr(6, 4).c_str());

    if (month < 1 || month > 12) return false;
    if (day < 1 || day > monthLength(month, year)) return false;

    return true;
}

// Converts a DD-MM-YYYY date into a running total-day count from a fixed epoch.
static long dateToDayCount(const string &date) {
    int day = atoi(date.substr(0, 2).c_str());
    int month = atoi(date.substr(3, 2).c_str());
    int year = atoi(date.substr(6, 4).c_str());

    long totalDays = 0;

    // Count full years since year 0 (using 365/366 days per year).
    for (int y = 0; y < year; y++) {
        bool isLeap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        totalDays += isLeap ? 366 : 365;
    }

    // Count full months of the current year.
    for (int m = 1; m < month; m++) {
        totalDays += monthLength(m, year);
    }

    totalDays += day;
    return totalDays;
}

int daysBetween(const string &date1, const string &date2) {
    long d1 = dateToDayCount(date1);
    long d2 = dateToDayCount(date2);
    long diff = d2 - d1;
    if (diff < 0) diff = -diff;
    return (int)diff;
}

FeeRecord findFeeRecord(const string &roll, const string &semester) {
    vector<vector<string>> rows = readTXT(FEES_FILE);
    FeeRecord rec;
    rec.roll = "";
    rec.amountDue = 0.0;
    rec.amountPaid = 0.0;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 6) continue;
        if (rows[i][0] == roll && rows[i][1] == semester) {
            rec.roll = rows[i][0];
            rec.semester = rows[i][1];
            rec.amountDue = atof(rows[i][2].c_str());
            rec.amountPaid = atof(rows[i][3].c_str());
            rec.dueDate = rows[i][4];
            rec.paidDate = rows[i][5];
            return rec;
        }
    }
    return rec;
}

bool recordPayment(const string &roll, const string &semester, double amount, const string &paidDate) {
    if (!isValidDateFormat(paidDate)) {
        cout << "Invalid date format. Expected DD-MM-YYYY." << endl;
        return false;
    }

    vector<vector<string>> rows = readTXT(FEES_FILE);
    vector<string> header = readHeader(FEES_FILE);
    bool found = false;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 6) continue;
        if (rows[i][0] == roll && rows[i][1] == semester) {
            double currentPaid = atof(rows[i][3].c_str());
            double newPaid = currentPaid + amount;

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.2f", newPaid);

            rows[i][3] = string(buffer);
            rows[i][5] = paidDate;
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "No fee record found for " << roll << " / " << semester << endl;
        return false;
    }

    writeTXT(FEES_FILE, header, rows);
    return true;
}

double computeLateFine(const string &roll, const string &semester) {
    FeeRecord rec = findFeeRecord(roll, semester);
    if (rec.roll.empty()) return 0.0;
    if (rec.paidDate.empty() || rec.dueDate.empty()) return 0.0;

    int diffDays = daysBetween(rec.dueDate, rec.paidDate);
    if (diffDays <= 0) return 0.0;

    int completeWeeks = diffDays / 7;
    return rec.amountDue * 0.02 * completeWeeks;
}

void generateReceipt(const string &roll, const string &semester) {
    FeeRecord rec = findFeeRecord(roll, semester);

    if (rec.roll.empty()) {
        cout << "No fee record found." << endl;
        return;
    }

    double fine = computeLateFine(roll, semester);
    double totalDue = rec.amountDue + fine;
    double balance = totalDue - rec.amountPaid;

    cout << "\n" << setfill('=') << setw(40) << "" << setfill(' ') << endl;
    cout << "               FEE RECEIPT" << endl;
    cout << setfill('=') << setw(40) << "" << setfill(' ') << endl;
    cout << left << setw(20) << "Roll:" << rec.roll << endl;
    cout << left << setw(20) << "Semester:" << rec.semester << endl;
    cout << left << setw(20) << "Tuition Due:" << fixed << setprecision(2) << rec.amountDue << endl;
    cout << left << setw(20) << "Late Fine:" << fixed << setprecision(2) << fine << endl;
    cout << left << setw(20) << "Total Due:" << fixed << setprecision(2) << totalDue << endl;
    cout << left << setw(20) << "Amount Paid:" << fixed << setprecision(2) << rec.amountPaid << endl;
    cout << left << setw(20) << "Balance:" << fixed << setprecision(2) << balance << endl;
    cout << setfill('=') << setw(40) << "" << setfill(' ') << endl;
}

vector<Defaulter> getDefaulters() {
    vector<vector<string>> rows = readTXT(FEES_FILE);
    vector<Defaulter> defaulters;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 6) continue;

        string roll = rows[i][0];
        string semester = rows[i][1];
        double due = atof(rows[i][2].c_str());
        double paid = atof(rows[i][3].c_str());
        string paidDate = rows[i][5];

        double fine = computeLateFine(roll, semester);
        double balance = due + fine - paid;

        if (balance > 0 && paidDate.empty()) {
            Defaulter d;
            d.roll = roll;
            d.balance = balance;
            d.weeksOverdue = 0;
            defaulters.push_back(d);
        }
    }

    // Bubble sort descending by balance.
    int n = (int)defaulters.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (defaulters[j].balance < defaulters[j + 1].balance) {
                Defaulter temp = defaulters[j];
                defaulters[j] = defaulters[j + 1];
                defaulters[j + 1] = temp;
            }
        }
    }

    return defaulters;
}
