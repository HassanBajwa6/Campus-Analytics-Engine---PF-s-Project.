#include "reports.h"
#include "filehandler.h"
#include "student_ops.h"
#include "grades.h"
#include "attendance.h"
#include "fee_tracker.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

// Helper: double to string with 2 dp
static std::string rpt_dbl(double val) {
    double rounded = (int)(val * 100.0 + 0.5) / 100.0;
    int w = (int)rounded;
    int f = (int)((rounded - w) * 100 + 0.5);
    if (f >= 100) {
        w += 1;
        f -= 100;
    }
    std::string ws = "", fs = "";
    if (w == 0) ws = "0";
    int tmp = w;
    while (tmp > 0) { ws = (char)('0' + tmp % 10) + ws; tmp /= 10; }
    if (f < 10) { fs = "0"; fs += (char)('0' + f); }
    else { fs += (char)('0' + f/10); fs += (char)('0' + f%10); }
    return ws + "." + fs;
}

void printMeritList() {
    std::vector<std::vector<std::string>> students = listActiveStudents();
    // Selection sort by CGPA descending
    for (int i = 0; i < (int)students.size() - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < (int)students.size(); j++) {
            if (atof(students[j][4].c_str()) > atof(students[maxIdx][4].c_str()))
                maxIdx = j;
        }
        if (maxIdx != i) {
            std::vector<std::string> tmp = students[i];
            students[i] = students[maxIdx];
            students[maxIdx] = tmp;
        }
    }
    std::cout << "\n" << std::string(65, '=') << "\n";
    std::cout << "  MERIT LIST (Active Students by CGPA)\n";
    std::cout << std::string(65, '=') << "\n";
    std::cout << std::left << std::setw(6)  << "Rank"
              << std::setw(15) << "Roll No"
              << std::setw(30) << "Name"
              << "CGPA\n";
    std::cout << std::string(65, '-') << "\n";
    for (int i = 0; i < (int)students.size(); i++) {
        std::cout << std::left << std::setw(6)  << (i+1)
                  << std::setw(15) << students[i][0]
                  << std::setw(30) << students[i][1]
                  << students[i][4] << "\n";
    }
    std::cout << std::string(65, '=') << "\n";
}

void printAttendanceDefaulters() {
    std::vector<std::vector<std::string>> courses = readTXT("courses.txt");
    std::cout << "\n" << std::string(65, '=') << "\n";
    std::cout << "  ATTENDANCE DEFAULTERS (< 75%)\n";
    std::cout << std::string(65, '=') << "\n";
    std::cout << std::left << std::setw(15) << "Roll No"
              << std::setw(25) << "Name"
              << std::setw(10) << "Course"
              << "Attendance%\n";
    std::cout << std::string(65, '-') << "\n";
    bool any = false;
    for (int c = 0; c < (int)courses.size(); c++) {
        if (courses[c].empty()) continue;
        std::string code = courses[c][0];
        std::vector<std::vector<std::string>> shortage = getShortageList(code);
        for (int i = 0; i < (int)shortage.size(); i++) {
            std::string roll = shortage[i][0];
            std::vector<std::string> stu = searchByRoll(roll);
            std::string name = (stu.size() > 1) ? stu[1] : "-";
            std::cout << std::left << std::setw(15) << roll
                      << std::setw(25) << name
                      << std::setw(10) << code
                      << shortage[i][2] << "%\n";
            any = true;
        }
    }
    if (!any) std::cout << "  No attendance defaulters found.\n";
    std::cout << std::string(65, '=') << "\n";
}

void printFeeDefaulters() {
    printDefaulters();
}

void printSemesterResult(const std::string& semester) {
    std::vector<std::vector<std::string>> students = listActiveStudents();
    std::vector<std::vector<std::string>> courses  = readTXT("courses.txt");
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "  SEMESTER RESULT SHEET | Semester: " << semester << "\n";
    std::cout << std::string(80, '=') << "\n";
    for (int i = 0; i < (int)students.size(); i++) {
        std::string roll = students[i][0];
        std::string name = students[i][1];
        std::cout << "\n  " << roll << " | " << name << "\n";
        std::cout << "  " << std::string(76, '-') << "\n";
        std::cout << "  " << std::left << std::setw(10) << "Course"
                  << std::setw(10) << "Grade"
                  << std::setw(12) << "Attendance"
                  << "Status\n";
        std::cout << "  " << std::string(76, '-') << "\n";
        std::vector<std::vector<std::string>> grades = readTXT("grades.txt");
        for (int g = 0; g < (int)grades.size(); g++) {
            if (grades[g].size() >= 9 &&
                grades[g][0] == roll &&
                grades[g][2] == semester) {
                std::string course = grades[g][1];
                std::string grade  = grades[g][8];
                double attPct = getAttendancePct(roll, course);
                std::string attStr = rpt_dbl(attPct) + "%";
                std::string status = (grade != "F") ? "PASS" : "FAIL";
                std::cout << "  " << std::left << std::setw(10) << course
                          << std::setw(10) << grade
                          << std::setw(12) << attStr
                          << status << "\n";
            }
        }
        double gpa = computeGPA(roll, semester);
        std::cout << "  GPA: " << std::fixed << std::setprecision(2) << gpa << "\n";
    }
    std::cout << "\n" << std::string(80, '=') << "\n";
}

void printDepartmentSummary(const std::string& semester) {
    std::vector<std::vector<std::string>> students = listActiveStudents();
    // Collect unique departments using parallel arrays
    std::string depts[50];
    int    deptCount[50] = {0};
    double deptCGPA[50]  = {0.0};
    int    deptPass[50]  = {0};
    int    numDepts = 0;
    for (int i = 0; i < (int)students.size(); i++) {
        std::string dept = (students[i].size() > 2) ? students[i][2] : "Unknown";
        // Find dept index
        int dIdx = -1;
        for (int d = 0; d < numDepts; d++) {
            if (depts[d] == dept) { dIdx = d; break; }
        }
        if (dIdx == -1) { dIdx = numDepts; depts[numDepts++] = dept; }
        deptCount[dIdx]++;
        deptCGPA[dIdx] += atof(students[i][4].c_str());
        // Count as pass if GPA > 0 in semester
        double gpa = computeGPA(students[i][0], semester);
        if (gpa > 0.0) deptPass[dIdx]++;
    }
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  DEPARTMENT SUMMARY | Semester: " << semester << "\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << std::left << std::setw(30) << "Department"
              << std::setw(10) << "Count"
              << std::setw(12) << "Avg CGPA"
              << "Pass Rate\n";
    std::cout << std::string(70, '-') << "\n";
    for (int d = 0; d < numDepts; d++) {
        double avgCGPA = (deptCount[d] > 0) ? deptCGPA[d] / deptCount[d] : 0.0;
        double passRate = (deptCount[d] > 0) ? (deptPass[d] * 100.0 / deptCount[d]) : 0.0;
        std::cout << std::left << std::setw(30) << depts[d]
                  << std::setw(10) << deptCount[d]
                  << std::setw(12) << rpt_dbl(avgCGPA)
                  << rpt_dbl(passRate) << "%\n";
    }
    std::cout << std::string(70, '=') << "\n";
}

void exportReportToFile(int reportChoice, const std::string& semester) {
    std::string filename = "report_output.txt";
    std::ofstream outFile(filename.c_str());
    if (!outFile.is_open()) {
        std::cout << "[ERROR] Could not create output file.\n";
        return;
    }
    // Redirect cout to file
    std::streambuf* coutBuf = std::cout.rdbuf();
    std::cout.rdbuf(outFile.rdbuf());
    switch (reportChoice) {
        case 1: printMeritList(); break;
        case 2: printAttendanceDefaulters(); break;
        case 3: printFeeDefaulters(); break;
        case 4: printSemesterResult(semester); break;
        case 5: printDepartmentSummary(semester); break;
        default: std::cout << "Invalid report choice.\n";
    }
    // Restore cout
    std::cout.rdbuf(coutBuf);
    outFile.close();
    std::cout << "[SUCCESS] Report exported to " << filename << "\n";
}
