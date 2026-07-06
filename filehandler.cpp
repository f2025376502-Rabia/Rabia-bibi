#include "filehandler.h"
#include <fstream>
#include <iostream>
using namespace std;

vector<string> parseLine(const string &line) {
    vector<string> fields;
    string current = "";
    bool insideQuotes = false;
    int i = 0;
    int len = (int)line.length();

    while (i < len) {
        char c = line[i];

        if (c == '"') {
            insideQuotes = !insideQuotes;
            i++;
            continue;
        }

        if (c == ',' && !insideQuotes) {
            fields.push_back(current);
            current = "";
            i++;
            continue;
        }

        current += c;
        i++;
    }
    fields.push_back(current);
    return fields;
}

string buildLine(const vector<string> &row) {
    string line = "";
    for (int i = 0; i < (int)row.size(); i++) {
        string field = row[i];

        bool needsQuotes = false;
        for (int j = 0; j < (int)field.length(); j++) {
            if (field[j] == ',') {
                needsQuotes = true;
                break;
            }
        }

        if (needsQuotes) {
            line += "\"" + field + "\"";
        } else {
            line += field;
        }

        if (i != (int)row.size() - 1) {
            line += ",";
        }
    }
    return line;
}

vector<string> readHeader(const string &filename) {
    ifstream inFile(filename.c_str());
    vector<string> header;

    if (!inFile.is_open()) {
        return header;
    }

    string firstLine;
    if (getline(inFile, firstLine)) {
        header = parseLine(firstLine);
    }
    inFile.close();
    return header;
}

vector<vector<string>> readTXT(const string &filename) {
    vector<vector<string>> rows;
    ifstream inFile(filename.c_str());

    if (!inFile.is_open()) {
        cout << "Warning: could not open " << filename << endl;
        return rows;
    }

    string line;
    bool isHeader = true;

    while (getline(inFile, line)) {
        if (isHeader) {
            isHeader = false;
            continue;
        }
        if (line.length() == 0) {
            continue;
        }
        vector<string> fields = parseLine(line);
        rows.push_back(fields);
    }

    inFile.close();
    return rows;
}

void writeTXT(const string &filename, const vector<string> &header, const vector<vector<string>> &rows) {
    ofstream outFile(filename.c_str());

    if (!outFile.is_open()) {
        cout << "Error: could not write to " << filename << endl;
        return;
    }

    outFile << buildLine(header) << endl;

    for (int i = 0; i < (int)rows.size(); i++) {
        outFile << buildLine(rows[i]) << endl;
    }

    outFile.close();
}

void appendTXT(const string &filename, const vector<string> &row) {
    ofstream outFile(filename.c_str(), ios::app);

    if (!outFile.is_open()) {
        cout << "Error: could not append to " << filename << endl;
        return;
    }

    outFile << buildLine(row) << endl;
    outFile.close();
}

vector<string> findRow(const string &filename, int colIndex, const string &value) {
    vector<vector<string>> rows = readTXT(filename);

    for (int i = 0; i < (int)rows.size(); i++) {
        if (colIndex < (int)rows[i].size() && rows[i][colIndex] == value) {
            return rows[i];
        }
    }

    vector<string> empty;
    return empty;
}

bool rowExists(const string &filename, int colIndex, const string &value) {
    vector<string> result = findRow(filename, colIndex, value);
    return !result.empty();
}
