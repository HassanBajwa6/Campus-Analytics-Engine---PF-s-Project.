#include "filehandler.h"
#include <fstream>
#include <iostream>

// Parses a single CSV line, handles quoted fields containing commas
std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field = "";
    bool inQuotes = false;

    for (int i = 0; i < (int)line.length(); i++) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field = "";
        } else if (c == '\r') {
            // skip carriage return
        } else {
            field += c;
        }
    }
    fields.push_back(field); // last field
    return fields;
}

// Formats a row to CSV, wrapping fields with commas in double quotes
std::string rowToCSV(const std::vector<std::string>& row) {
    std::string line = "";
    for (int i = 0; i < (int)row.size(); i++) {
        bool hasComma = false;
        for (int j = 0; j < (int)row[i].length(); j++) {
            if (row[i][j] == ',') { hasComma = true; break; }
        }
        if (hasComma) {
            line += "\"" + row[i] + "\"";
        } else {
            line += row[i];
        }
        if (i < (int)row.size() - 1) line += ",";
    }
    return line;
}

// Opens file, skips header, parses each line into a vector of string vectors
std::vector<std::vector<std::string>> readTXT(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot open file: " << filename << std::endl;
        return data;
    }
    std::string line = "";
    bool firstLine = true;
    while (std::getline(file, line)) {
        if (firstLine) { firstLine = false; continue; } // skip header
        if (line.empty() || line == "\r") continue;
        std::vector<std::string> row = parseCSVLine(line);
        if (!row.empty()) data.push_back(row);
    }
    file.close();
    return data;
}

// Opens file for overwrite, writes header then all rows
void writeTXT(const std::string& filename, const std::string& header, const std::vector<std::vector<std::string>>& rows) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot write to file: " << filename << std::endl;
        return;
    }
    file << header << "\n";
    for (int i = 0; i < (int)rows.size(); i++) {
        file << rowToCSV(rows[i]) << "\n";
    }
    file.close();
}

// Appends a single row to existing file without loading it fully
void appendTXT(const std::string& filename, const std::vector<std::string>& row) {
    std::ofstream file(filename.c_str(), std::ios::app);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot append to file: " << filename << std::endl;
        return;
    }
    file << rowToCSV(row) << "\n";
    file.close();
}

// Linear search: returns first matching row where row[colIndex] == value
std::vector<std::string> findRow(const std::vector<std::vector<std::string>>& rows, int colIndex, const std::string& value) {
    for (int i = 0; i < (int)rows.size(); i++) {
        if (colIndex < (int)rows[i].size() && rows[i][colIndex] == value) {
            return rows[i];
        }
    }
    return std::vector<std::string>(); // empty = not found
}

// Returns true if any row has value at colIndex
bool rowExists(const std::vector<std::vector<std::string>>& rows, int colIndex, const std::string& value) {
    for (int i = 0; i < (int)rows.size(); i++) {
        if (colIndex < (int)rows[i].size() && rows[i][colIndex] == value) {
            return true;
        }
    }
    return false;
}
