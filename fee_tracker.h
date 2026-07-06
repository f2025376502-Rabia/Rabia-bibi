#ifndef FEE_TRACKER_H
#define FEE_TRACKER_H

#include <vector>
#include <string>
using namespace std;

const string FEES_FILE = "fees.txt";

struct FeeRecord {
    string roll;
    string semester;
    double amountDue;
    double amountPaid;
    string dueDate;
    string paidDate;
};

struct Defaulter {
    string roll;
    double balance;
    int weeksOverdue;
};

// Validates a DD-MM-YYYY string using character/substr checks (no ctime).
bool isValidDateFormat(const string &date);

// Manual day-count difference between two DD-MM-YYYY dates (no ctime).
int daysBetween(const string &date1, const string &date2);

// Records a payment against a fee row; updates amount_paid and paid_date.
bool recordPayment(const string &roll, const string &semester, double amount, const string &paidDate);

// 2% of amount_due per complete week between due_date and paid_date.
double computeLateFine(const string &roll, const string &semester);

// Prints a formatted receipt using setw/setfill.
void generateReceipt(const string &roll, const string &semester);

// Returns students with balance > 0 past due date, sorted by balance (bubble sort).
vector<Defaulter> getDefaulters();

#endif
