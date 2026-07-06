#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <vector>
#include <string>
using namespace std;

// Parses one raw line of text into fields, respecting quoted commas.
vector<string> parseLine(const string &line);

// Turns a row of fields back into a single comma-separated line,
// wrapping any field that itself contains a comma in quotes.
string buildLine(const vector<string> &row);

// Opens filename, skips the header row, parses every remaining line
// into a vector<string> using a manual character loop (no getline-split).
vector<vector<string>> readTXT(const string &filename);

// Reads just the header row of a file (first line), split into fields.
vector<string> readHeader(const string &filename);

// Overwrites filename: writes header row first, then every row in rows.
void writeTXT(const string &filename, const vector<string> &header, const vector<vector<string>> &rows);

// Appends a single row to filename without loading the whole file.
void appendTXT(const string &filename, const vector<string> &row);

// Linear search through filename's rows. Returns the first row whose
// colIndex field equals value, or an empty vector if nothing matches.
vector<string> findRow(const string &filename, int colIndex, const string &value);

// True if any row in filename has value at colIndex.
bool rowExists(const string &filename, int colIndex, const string &value);

#endif
