#ifndef STUDENT_OPS_H
#define STUDENT_OPS_H

#include <string>
#include <vector>

// Validates BSAI-YY-XXX roll format using substr/char checks (no regex)
bool validateRollFormat(const std::string& roll);

// Validates name (no digits allowed)
bool validateName(const std::string& name);

// Validates CGPA in [0.0, 4.0]
bool validateCGPA(const std::string& cgpa);

// Adds a new student: validates, checks duplicates, appends to students.txt
bool addStudent(const std::string& roll, const std::string& name, const std::string& dept, const std::string& semester, const std::string& cgpa);

// Returns student row by roll number (empty if not found)
std::vector<std::string> searchByRoll(const std::string& roll);

// Returns all students whose name contains the substring (case-insensitive)
std::vector<std::vector<std::string>> searchByName(const std::string& nameSubstr);

// Updates a specified field (not roll) for a student
bool updateStudent(const std::string& roll, int fieldIndex, const std::string& newValue);

// Sets status field to 'inactive' (soft delete)
bool softDelete(const std::string& roll);

// Returns all active students sorted by roll number using selection sort
std::vector<std::vector<std::string>> listActiveStudents();

// Prints student details in formatted way
void printStudentDetails(const std::vector<std::string>& student);

// Helper: convert string to lowercase
std::string toLower(const std::string& s);

#endif
