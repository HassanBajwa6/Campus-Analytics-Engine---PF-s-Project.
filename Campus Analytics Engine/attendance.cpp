#include "attendance.h"
#include "filehandler.h"
#include "course_ops.h"
#include "student_ops.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

const std::string ATTENDANCE_FILE = "attendance_log.txt";
const std::string ATTEND_HEADER   = "log_id,roll_no,course_code,session_date,status";

// Global backup for undo
std::vector<std::vector<std::string>> attendanceBackup;
bool hasBackup = false;

std::string nextLogID() {
    std::vector<std::vector<std::string>> logs = readTXT(ATTENDANCE_FILE);
    int maxNum = 0;
    for (int i = 0; i < (int)logs.size(); i++) {
        if (!logs[i].empty()) {
            std::string id = logs[i][0]; // e.g. L01276
            if (id.length() >= 2 && id[0] == 'L') {
                int num = atoi(id.substr(1).c_str());
                if (num > maxNum) maxNum = num;
            }
        }
    }
    int next = maxNum + 1;
    std::string numStr = "";
    if (next == 0) numStr = "0";
    int tmp = next;
    while (tmp > 0) { numStr = (char)('0' + tmp % 10) + numStr; tmp /= 10; }
    while ((int)numStr.length() < 5) numStr = "0" + numStr;
    return "L" + numStr;
}

void markAttendance(const std::string& courseCode, const std::string& date) {
    std::vector<std::vector<std::string>> enrolled = listEnrolledStudents(courseCode);
    if (enrolled.empty()) {
        std::cout << "[INFO] No enrolled students found for " << courseCode << ".\n";
        return;
    }
    // Backup current attendance before session
    attendanceBackup = readTXT(ATTENDANCE_FILE);
    hasBackup = true;
    std::cout << "\n--- Marking Attendance for " << courseCode << " on " << date << " ---\n";
    std::cout << "Enter P (Present), A (Absent), L (Late) for each student:\n\n";
    for (int i = 0; i < (int)enrolled.size(); i++) {
        std::string roll = enrolled[i][1];
        std::vector<std::string> student = searchByRoll(roll);
        std::string name = (student.size() > 1) ? student[1] : roll;
        std::string status = "";
        while (status != "P" && status != "A" && status != "L") {
            std::cout << "  " << std::left << std::setw(15) << roll
                      << std::setw(25) << name << " [P/A/L]: ";
            std::cin >> status;
            // Convert to uppercase
            for (int k = 0; k < (int)status.length(); k++) {
                if (status[k] >= 'a' && status[k] <= 'z') status[k] -= 32;
            }
            if (status != "P" && status != "A" && status != "L")
                std::cout << "  [ERROR] Enter P, A, or L only.\n";
        }
        std::string logID = nextLogID();
        std::vector<std::string> row;
        row.push_back(logID);
        row.push_back(roll);
        row.push_back(courseCode);
        row.push_back(date);
        row.push_back(status);
        appendTXT(ATTENDANCE_FILE, row);
    }
    std::cout << "[SUCCESS] Attendance marked for " << enrolled.size() << " students.\n";
}

double getAttendancePct(const std::string& roll, const std::string& courseCode) {
    std::vector<std::vector<std::string>> logs = readTXT(ATTENDANCE_FILE);
    int totalSessions = 0;
    double score = 0.0;
    for (int i = 0; i < (int)logs.size(); i++) {
        if (logs[i].size() >= 5 &&
            logs[i][1] == roll &&
            logs[i][2] == courseCode) {
            totalSessions++;
            if      (logs[i][4] == "P") score += 1.0;
            else if (logs[i][4] == "L") score += 0.5;
            // A = 0
        }
    }
    if (totalSessions == 0) return 0.0;
    return (score / (double)totalSessions) * 100.0;
}

std::vector<std::vector<std::string>> getShortageList(const std::string& courseCode) {
    std::vector<std::vector<std::string>> enrolled = listEnrolledStudents(courseCode);
    std::vector<std::vector<std::string>> shortage;
    for (int i = 0; i < (int)enrolled.size(); i++) {
        std::string roll = enrolled[i][1];
        double pct = getAttendancePct(roll, courseCode);
        if (pct < 75.0) {
            std::vector<std::string> entry;
            entry.push_back(roll);
            entry.push_back(courseCode);
            // Store pct as string
            std::string pctStr = "";
            int whole = (int)pct;
            int frac  = (int)((pct - whole) * 10);
            int tmp = whole;
            if (tmp == 0) pctStr = "0";
            while (tmp > 0) { pctStr = (char)('0' + tmp % 10) + pctStr; tmp /= 10; }
            pctStr += ".";
            pctStr += (char)('0' + frac);
            entry.push_back(pctStr);
            shortage.push_back(entry);
        }
    }
    return shortage;
}

bool undoLastSession() {
    if (!hasBackup) {
        std::cout << "[ERROR] No backup available to undo.\n";
        return false;
    }
    writeTXT(ATTENDANCE_FILE, ATTEND_HEADER, attendanceBackup);
    hasBackup = false;
    std::cout << "[SUCCESS] Last attendance session undone.\n";
    return true;
}

void printDailySheet(const std::string& courseCode, const std::string& date) {
    std::vector<std::vector<std::string>> logs = readTXT(ATTENDANCE_FILE);
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  ATTENDANCE SHEET | " << courseCode << " | Date: " << date << "\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << std::left << std::setw(15) << "Roll No"
              << std::setw(30) << "Name"
              << "Status\n";
    std::cout << std::string(60, '-') << "\n";
    bool any = false;
    for (int i = 0; i < (int)logs.size(); i++) {
        if (logs[i].size() >= 5 &&
            logs[i][2] == courseCode &&
            logs[i][3] == date) {
            std::string roll = logs[i][1];
            std::vector<std::string> student = searchByRoll(roll);
            std::string name = (student.size() > 1) ? student[1] : "-";
            std::cout << std::left << std::setw(15) << roll
                      << std::setw(30) << name
                      << logs[i][4] << "\n";
            any = true;
        }
    }
    if (!any) std::cout << "  No records found for this date.\n";
    std::cout << std::string(60, '=') << "\n";
}
