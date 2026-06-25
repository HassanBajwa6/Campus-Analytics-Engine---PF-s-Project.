#include "student_ops.h"
#include "filehandler.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

const std::string STUDENTS_FILE = "students.txt";
const std::string STUDENTS_HEADER = "roll_no,name,department,semester,cgpa,status";

// --- Column indices in students.txt ---
// 0=roll_no, 1=name, 2=department, 3=semester, 4=cgpa, 5=status

std::string toLower(const std::string& s) {
    std::string result = s;
    for (int i = 0; i < (int)result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z')
            result[i] = result[i] + 32;
    }
    return result;
}

// Validates BSAI-YY-XXX format using substr and character checks
bool validateRollFormat(const std::string& roll) {
    // Must be exactly 11 chars: BSAI-23-001
    if (roll.length() != 11) return false;
    if (roll.substr(0, 5) != "BSAI-") return false;
    // YY: chars 5-6 must be digits
    if (!(roll[5] >= '0' && roll[5] <= '9')) return false;
    if (!(roll[6] >= '0' && roll[6] <= '9')) return false;
    if (roll[7] != '-') return false;
    // XXX: chars 8-10 must be digits
    if (!(roll[8] >= '0' && roll[8] <= '9')) return false;
    if (!(roll[9] >= '0' && roll[9] <= '9')) return false;
    if (!(roll[10] >= '0' && roll[10] <= '9')) return false;
    return true;
}

// Name must not contain any digits
bool validateName(const std::string& name) {
    if (name.empty()) return false;
    for (int i = 0; i < (int)name.length(); i++) {
        if (name[i] >= '0' && name[i] <= '9') return false;
    }
    return true;
}

// CGPA must be numeric and in [0.0, 4.0]
bool validateCGPA(const std::string& cgpa) {
    if (cgpa.empty()) return false;
    bool hasDot = false;
    for (int i = 0; i < (int)cgpa.length(); i++) {
        if (cgpa[i] == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (!(cgpa[i] >= '0' && cgpa[i] <= '9')) {
            return false;
        }
    }
    double val = atof(cgpa.c_str());
    return (val >= 0.0 && val <= 4.0);
}

// Adds student with full validation
bool addStudent(const std::string& roll, const std::string& name, const std::string& dept, const std::string& semester, const std::string& cgpa) {
    // Validate roll format
    if (!validateRollFormat(roll)) {
        std::cout << "[ERROR] Invalid roll format. Must be BSAI-YY-XXX (e.g., BSAI-23-001)\n";
        return false;
    }
    // Validate name
    if (!validateName(name)) {
        std::cout << "[ERROR] Name must not contain digits.\n";
        return false;
    }
    // Validate CGPA
    if (!validateCGPA(cgpa)) {
        std::cout << "[ERROR] CGPA must be a number between 0.0 and 4.0.\n";
        return false;
    }
    // Check for duplicate
    std::vector<std::vector<std::string>> students = readTXT(STUDENTS_FILE);
    if (rowExists(students, 0, roll)) {
        std::cout << "[ERROR] Student with roll " << roll << " already exists.\n";
        return false;
    }
    // Append to file
    std::vector<std::string> newRow;
    newRow.push_back(roll);
    newRow.push_back(name);
    newRow.push_back(dept);
    newRow.push_back(semester);
    newRow.push_back(cgpa);
    newRow.push_back("active");
    appendTXT(STUDENTS_FILE, newRow);
    std::cout << "[SUCCESS] Student " << name << " (" << roll << ") added.\n";
    return true;
}

// Returns student row by roll
std::vector<std::string> searchByRoll(const std::string& roll) {
    std::vector<std::vector<std::string>> students = readTXT(STUDENTS_FILE);
    return findRow(students, 0, roll);
}

// Returns all students whose name contains substring (case-insensitive)
std::vector<std::vector<std::string>> searchByName(const std::string& nameSubstr) {
    std::vector<std::vector<std::string>> students = readTXT(STUDENTS_FILE);
    std::vector<std::vector<std::string>> results;
    std::string lowerSubstr = toLower(nameSubstr);
    for (int i = 0; i < (int)students.size(); i++) {
        if (students[i].size() > 1) {
            std::string lowerName = toLower(students[i][1]);
            // Check if lowerName contains lowerSubstr
            bool found = false;
            if (lowerSubstr.length() <= lowerName.length()) {
                for (int j = 0; j <= (int)lowerName.length() - (int)lowerSubstr.length(); j++) {
                    if (lowerName.substr(j, lowerSubstr.length()) == lowerSubstr) {
                        found = true;
                        break;
                    }
                }
            }
            if (found) results.push_back(students[i]);
        }
    }
    return results;
}

// Updates specified field for student identified by roll
bool updateStudent(const std::string& roll, int fieldIndex, const std::string& newValue) {
    if (fieldIndex == 0) {
        std::cout << "[ERROR] Roll number cannot be changed.\n";
        return false;
    }
    if (fieldIndex == 1) { // name
        if (!validateName(newValue)) {
            std::cout << "[ERROR] Name must not contain digits.\n";
            return false;
        }
    }
    if (fieldIndex == 4) { // cgpa
        if (!validateCGPA(newValue)) {
            std::cout << "[ERROR] CGPA must be a number between 0.0 and 4.0.\n";
            return false;
        }
    }
    if (fieldIndex == 5) { // status
        if (newValue != "active" && newValue != "inactive") {
            std::cout << "[ERROR] Status must be 'active' or 'inactive'.\n";
            return false;
        }
    }
    std::vector<std::vector<std::string>> students = readTXT(STUDENTS_FILE);
    bool found = false;
    for (int i = 0; i < (int)students.size(); i++) {
        if (students[i].size() > 0 && students[i][0] == roll) {
            if (fieldIndex < (int)students[i].size()) {
                students[i][fieldIndex] = newValue;
                found = true;
                break;
            }
        }
    }
    if (!found) {
        std::cout << "[ERROR] Student not found.\n";
        return false;
    }
    writeTXT(STUDENTS_FILE, STUDENTS_HEADER, students);
    std::cout << "[SUCCESS] Student record updated.\n";
    return true;
}

// Soft delete: sets status to 'inactive'
bool softDelete(const std::string& roll) {
    return updateStudent(roll, 5, "inactive");
}

// Returns active students sorted by roll using selection sort
std::vector<std::vector<std::string>> listActiveStudents() {
    std::vector<std::vector<std::string>> students = readTXT(STUDENTS_FILE);
    std::vector<std::vector<std::string>> active;
    for (int i = 0; i < (int)students.size(); i++) {
        if (students[i].size() > 5 && students[i][5] == "active") {
            active.push_back(students[i]);
        }
    }
    // Selection sort by roll (index 0)
    for (int i = 0; i < (int)active.size() - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < (int)active.size(); j++) {
            if (active[j][0] < active[minIdx][0]) minIdx = j;
        }
        if (minIdx != i) {
            std::vector<std::string> temp = active[i];
            active[i] = active[minIdx];
            active[minIdx] = temp;
        }
    }
    return active;
}

void printStudentDetails(const std::vector<std::string>& s) {
    if (s.empty()) { std::cout << "  [No data]\n"; return; }
    std::cout << "  Roll   : " << (s.size()>0?s[0]:"") << "\n";
    std::cout << "  Name   : " << (s.size()>1?s[1]:"") << "\n";
    std::cout << "  Dept   : " << (s.size()>2?s[2]:"") << "\n";
    std::cout << "  Sem    : " << (s.size()>3?s[3]:"") << "\n";
    std::cout << "  CGPA   : " << (s.size()>4?s[4]:"") << "\n";
    std::cout << "  Status : " << (s.size()>5?s[5]:"") << "\n";
}
