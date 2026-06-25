#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>

// Backup vector for undo (stores last session rows)
extern std::vector<std::vector<std::string>> attendanceBackup;
extern bool hasBackup;

// Marks attendance for all enrolled students in a course for a date
void markAttendance(const std::string& courseCode, const std::string& date);

// Computes attendance % = (P + 0.5*L) / total * 100
double getAttendancePct(const std::string& roll, const std::string& courseCode);

// Returns students with attendance < 75% in a course
std::vector<std::vector<std::string>> getShortageList(const std::string& courseCode);

// Undoes last session (restores from backup). Returns false if no backup.
bool undoLastSession();

// Prints formatted daily sheet for a course+date
void printDailySheet(const std::string& courseCode, const std::string& date);

// Returns next log ID string (e.g., L01277)
std::string nextLogID();

#endif
