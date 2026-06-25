#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>

// Maximum rows and columns for file data
const int MAX_ROWS = 2000;
const int MAX_COLS = 12;

// Reads a CSV file into a 2D vector of strings (skips header)
std::vector<std::vector<std::string>> readTXT(const std::string& filename);

// Overwrites a file with provided header and rows
void writeTXT(const std::string& filename, const std::string& header, const std::vector<std::vector<std::string>>& rows);

// Appends a single row to file (comma-separated)
void appendTXT(const std::string& filename, const std::vector<std::string>& row);

// Finds first row where row[colIndex] == value, returns it (empty if not found)
std::vector<std::string> findRow(const std::vector<std::vector<std::string>>& rows, int colIndex, const std::string& value);

// Returns true if any row has value at colIndex
bool rowExists(const std::vector<std::vector<std::string>>& rows, int colIndex, const std::string& value);

// Parses a CSV line (handles quoted fields with commas)
std::vector<std::string> parseCSVLine(const std::string& line);

// Formats a row back to CSV string (wraps fields with commas in quotes)
std::string rowToCSV(const std::vector<std::string>& row);

#endif
