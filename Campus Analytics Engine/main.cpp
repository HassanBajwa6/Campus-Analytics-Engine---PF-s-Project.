#include <iostream>
#include <string>
#include <iomanip>
#include "filehandler.h"
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include "reports.h"

// ============================================================
//  UTILITY: clear input buffer
// ============================================================
void clearBuffer() {
    std::cin.ignore(10000, '\n');
}

// ============================================================
//  BONUS: Search-as-you-type (prefix match loop)
// ============================================================
void searchAsYouType() {
    std::vector<std::vector<std::string>> students = readTXT("students.txt");
    std::string typed = "";
    char ch;
    std::cout << "\n  === SEARCH-AS-YOU-TYPE (press Enter when done, Backspace=0) ===\n";
    while (true) {
        std::cout << "  Query [" << typed << "]: ";
        ch = std::cin.get();
        if (ch == '\n') break;
        if (ch == '0' && !typed.empty()) {
            typed = typed.substr(0, typed.length() - 1);
        } else {
            typed += ch;
        }
        // Filter and reprint
        std::string lower = toLower(typed);
        std::cout << "\n  --- Matching Students ---\n";
        bool any = false;
        for (int i = 0; i < (int)students.size(); i++) {
            if (students[i].size() > 1) {
                std::string lname = toLower(students[i][1]);
                // Prefix match using substr
                bool match = false;
                if (lower.length() <= lname.length()) {
                    if (lname.substr(0, lower.length()) == lower) match = true;
                }
                if (match) {
                    std::cout << "  " << std::left << std::setw(15) << students[i][0]
                              << students[i][1] << "\n";
                    any = true;
                }
            }
        }
        if (!any) std::cout << "  (no matches)\n";
        std::cout << "\n";
    }
}

// ============================================================
//  MENU: Student Operations
// ============================================================
void menuStudentOps() {
    int choice = 0;
    while (true) {
        std::cout << "\n  +----- STUDENT OPERATIONS -----+\n";
        std::cout << "  | 1. Add New Student           |\n";
        std::cout << "  | 2. Search by Roll No         |\n";
        std::cout << "  | 3. Search by Name            |\n";
        std::cout << "  | 4. Update Student Record     |\n";
        std::cout << "  | 5. Soft Delete Student       |\n";
        std::cout << "  | 6. List All Active Students  |\n";
        std::cout << "  | 7. Search-As-You-Type (Bonus)|\n";
        std::cout << "  | 0. Back                      |\n";
        std::cout << "  +------------------------------+\n";
        std::cout << "  Choice: ";
        std::cin >> choice;
        clearBuffer();
        if (choice == 0) break;
        if (choice == 1) {
            std::string roll, name, dept, sem, cgpa;
            std::cout << "  Roll (BSAI-YY-XXX): "; std::getline(std::cin, roll);
            std::cout << "  Name: ";               std::getline(std::cin, name);
            std::cout << "  Department: ";         std::getline(std::cin, dept);
            std::cout << "  Semester: ";           std::getline(std::cin, sem);
            std::cout << "  CGPA (0.0-4.0): ";     std::getline(std::cin, cgpa);
            addStudent(roll, name, dept, sem, cgpa);
        } else if (choice == 2) {
            std::string roll;
            std::cout << "  Enter Roll No: "; std::getline(std::cin, roll);
            std::vector<std::string> s = searchByRoll(roll);
            if (s.empty()) std::cout << "  [Not found]\n";
            else printStudentDetails(s);
        } else if (choice == 3) {
            std::string name;
            std::cout << "  Enter Name (or part): "; std::getline(std::cin, name);
            std::vector<std::vector<std::string>> results = searchByName(name);
            if (results.empty()) { std::cout << "  [No matches]\n"; }
            else {
                std::cout << "\n  Found " << results.size() << " match(es):\n";
                for (int i = 0; i < (int)results.size(); i++) printStudentDetails(results[i]);
            }
        } else if (choice == 4) {
            std::string roll, val;
            int field;
            std::cout << "  Roll No: "; std::getline(std::cin, roll);
            std::cout << "  Field to update:\n";
            std::cout << "    1=name  2=department  3=semester  4=cgpa  5=status\n";
            std::cout << "  Field number: "; std::cin >> field; clearBuffer();
            std::cout << "  New value: "; std::getline(std::cin, val);
            updateStudent(roll, field, val);
        } else if (choice == 5) {
            std::string roll;
            std::cout << "  Roll No to delete: "; std::getline(std::cin, roll);
            if (softDelete(roll)) std::cout << "  [Student set to inactive]\n";
        } else if (choice == 6) {
            std::vector<std::vector<std::string>> all = listActiveStudents();
            std::cout << "\n  Active Students (" << all.size() << "):\n";
            std::cout << "  " << std::string(55, '-') << "\n";
            std::cout << "  " << std::left << std::setw(15) << "Roll"
                      << std::setw(25) << "Name" << "CGPA\n";
            std::cout << "  " << std::string(55, '-') << "\n";
            for (int i = 0; i < (int)all.size(); i++) {
                std::cout << "  " << std::left << std::setw(15) << all[i][0]
                          << std::setw(25) << all[i][1]
                          << all[i][4] << "\n";
            }
        } else if (choice == 7) {
            searchAsYouType();
        } else {
            std::cout << "  [Invalid choice]\n";
        }
    }
}

// ============================================================
//  MENU: Course Operations
// ============================================================
void menuCourseOps() {
    int choice = 0;
    while (true) {
        std::cout << "\n  +----- COURSE OPERATIONS ------+\n";
        std::cout << "  | 1. List All Courses          |\n";
        std::cout << "  | 2. Enroll Student            |\n";
        std::cout << "  | 3. Drop Course               |\n";
        std::cout << "  | 4. View Credit Load          |\n";
        std::cout << "  | 5. List Enrolled Students    |\n";
        std::cout << "  | 0. Back                      |\n";
        std::cout << "  +------------------------------+\n";
        std::cout << "  Choice: ";
        std::cin >> choice;
        clearBuffer();
        if (choice == 0) break;
        if (choice == 1) {
            printAllCourses();
        } else if (choice == 2) {
            std::string roll, code, sem;
            std::cout << "  Student Roll: "; std::getline(std::cin, roll);
            std::cout << "  Course Code:  "; std::getline(std::cin, code);
            std::cout << "  Semester:     "; std::getline(std::cin, sem);
            EnrollResult r = enrollStudent(roll, code, sem);
            printEnrollResult(r);
        } else if (choice == 3) {
            std::string roll, code, sem;
            std::cout << "  Student Roll: "; std::getline(std::cin, roll);
            std::cout << "  Course Code:  "; std::getline(std::cin, code);
            std::cout << "  Semester:     "; std::getline(std::cin, sem);
            dropCourse(roll, code, sem);
        } else if (choice == 4) {
            std::string roll, sem;
            std::cout << "  Student Roll: "; std::getline(std::cin, roll);
            std::cout << "  Semester:     "; std::getline(std::cin, sem);
            int load = getCreditLoad(roll, sem);
            std::cout << "  Credit Load: " << load << " hours\n";
        } else if (choice == 5) {
            std::string code;
            std::cout << "  Course Code: "; std::getline(std::cin, code);
            std::vector<std::vector<std::string>> enrolled = listEnrolledStudents(code);
            std::cout << "\n  Enrolled in " << code << ": " << enrolled.size() << " students\n";
            for (int i = 0; i < (int)enrolled.size(); i++) {
                std::string roll = enrolled[i][1];
                std::vector<std::string> s = searchByRoll(roll);
                std::string name = (s.size() > 1) ? s[1] : "-";
                std::cout << "  " << std::left << std::setw(15) << roll << name << "\n";
            }
        } else {
            std::cout << "  [Invalid choice]\n";
        }
    }
}

// ============================================================
//  MENU: Attendance Operations
// ============================================================
void menuAttendance() {
    int choice = 0;
    while (true) {
        std::cout << "\n  +----- ATTENDANCE --------------+\n";
        std::cout << "  | 1. Mark Attendance           |\n";
        std::cout << "  | 2. View Attendance %         |\n";
        std::cout << "  | 3. Shortage List (<75%)      |\n";
        std::cout << "  | 4. Undo Last Session         |\n";
        std::cout << "  | 5. Print Daily Sheet         |\n";
        std::cout << "  | 0. Back                      |\n";
        std::cout << "  +------------------------------+\n";
        std::cout << "  Choice: ";
        std::cin >> choice;
        clearBuffer();
        if (choice == 0) break;
        if (choice == 1) {
            std::string code, date;
            std::cout << "  Course Code: "; std::getline(std::cin, code);
            std::cout << "  Date (DD-MM-YYYY): "; std::getline(std::cin, date);
            markAttendance(code, date);
        } else if (choice == 2) {
            std::string roll, code;
            std::cout << "  Roll No:     "; std::getline(std::cin, roll);
            std::cout << "  Course Code: "; std::getline(std::cin, code);
            double pct = getAttendancePct(roll, code);
            std::cout << std::fixed << std::setprecision(1);
            std::cout << "  Attendance: " << pct << "%";
            if (pct < 75.0) std::cout << "  [WARNING: Below 75%!]";
            std::cout << "\n";
        } else if (choice == 3) {
            std::string code;
            std::cout << "  Course Code: "; std::getline(std::cin, code);
            std::vector<std::vector<std::string>> list = getShortageList(code);
            std::cout << "\n  Students below 75% in " << code << ": " << list.size() << "\n";
            for (int i = 0; i < (int)list.size(); i++) {
                std::vector<std::string> s = searchByRoll(list[i][0]);
                std::string name = (s.size() > 1) ? s[1] : "-";
                std::cout << "  " << std::left << std::setw(15) << list[i][0]
                          << std::setw(25) << name
                          << list[i][2] << "%\n";
            }
        } else if (choice == 4) {
            undoLastSession();
        } else if (choice == 5) {
            std::string code, date;
            std::cout << "  Course Code: "; std::getline(std::cin, code);
            std::cout << "  Date (DD-MM-YYYY): "; std::getline(std::cin, date);
            printDailySheet(code, date);
        } else {
            std::cout << "  [Invalid choice]\n";
        }
    }
}

// ============================================================
//  MENU: Grades Operations
// ============================================================
void menuGrades() {
    int choice = 0;
    while (true) {
        std::cout << "\n  +----- GRADES -----------------+\n";
        std::cout << "  | 1. Enter Marks               |\n";
        std::cout << "  | 2. View Student Grades       |\n";
        std::cout << "  | 3. Class Statistics          |\n";
        std::cout << "  | 4. Compute Semester GPA      |\n";
        std::cout << "  | 0. Back                      |\n";
        std::cout << "  +------------------------------+\n";
        std::cout << "  Choice: ";
        std::cin >> choice;
        clearBuffer();
        if (choice == 0) break;
        if (choice == 1) {
            std::string roll, code, sem;
            std::cout << "  Roll No:     "; std::getline(std::cin, roll);
            std::cout << "  Course Code: "; std::getline(std::cin, code);
            std::cout << "  Semester:    "; std::getline(std::cin, sem);
            enterMarks(roll, code, sem);
        } else if (choice == 2) {
            std::string roll, sem;
            std::cout << "  Roll No:  "; std::getline(std::cin, roll);
            std::cout << "  Semester: "; std::getline(std::cin, sem);
            printStudentGrades(roll, sem);
        } else if (choice == 3) {
            std::string code, sem;
            std::cout << "  Course Code: "; std::getline(std::cin, code);
            std::cout << "  Semester:    "; std::getline(std::cin, sem);
            Stats s = computeClassStats(code, sem);
            std::cout << "\n  Class Stats for " << code << " (Sem " << sem << "):\n";
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  Highest: " << s.highest << "\n";
            std::cout << "  Lowest : " << s.lowest  << "\n";
            std::cout << "  Mean   : " << s.mean    << "\n";
            std::cout << "  Median : " << s.median  << "\n";
        } else if (choice == 4) {
            std::string roll, sem;
            std::cout << "  Roll No:  "; std::getline(std::cin, roll);
            std::cout << "  Semester: "; std::getline(std::cin, sem);
            double gpa = computeGPA(roll, sem);
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  Semester GPA: " << gpa << "\n";
        } else {
            std::cout << "  [Invalid choice]\n";
        }
    }
}

// ============================================================
//  MENU: Fee Operations
// ============================================================
void menuFees() {
    int choice = 0;
    while (true) {
        std::cout << "\n  +----- FEE TRACKER ------------+\n";
        std::cout << "  | 1. Record Payment            |\n";
        std::cout << "  | 2. Compute Late Fine         |\n";
        std::cout << "  | 3. Generate Receipt          |\n";
        std::cout << "  | 4. View All Defaulters       |\n";
        std::cout << "  | 0. Back                      |\n";
        std::cout << "  +------------------------------+\n";
        std::cout << "  Choice: ";
        std::cin >> choice;
        clearBuffer();
        if (choice == 0) break;
        if (choice == 1) {
            std::string roll, sem, date;
            double amt;
            std::cout << "  Roll No:          "; std::getline(std::cin, roll);
            std::cout << "  Semester:         "; std::getline(std::cin, sem);
            std::cout << "  Amount:           "; std::cin >> amt; clearBuffer();
            std::cout << "  Payment Date (DD-MM-YYYY): "; std::getline(std::cin, date);
            recordPayment(roll, sem, amt, date);
        } else if (choice == 2) {
            std::string roll, sem;
            std::cout << "  Roll No:  "; std::getline(std::cin, roll);
            std::cout << "  Semester: "; std::getline(std::cin, sem);
            double fine = computeLateFine(roll, sem);
            std::cout << std::fixed << std::setprecision(0);
            std::cout << "  Late Fine: PKR " << fine << "\n";
        } else if (choice == 3) {
            std::string roll, sem;
            std::cout << "  Roll No:  "; std::getline(std::cin, roll);
            std::cout << "  Semester: "; std::getline(std::cin, sem);
            generateReceipt(roll, sem);
        } else if (choice == 4) {
            printDefaulters();
        } else {
            std::cout << "  [Invalid choice]\n";
        }
    }
}

// ============================================================
//  MENU: Reports
// ============================================================
void menuReports() {
    int choice = 0;
    while (true) {
        std::cout << "\n  +----- REPORTS ----------------+\n";
        std::cout << "  | 1. Merit List                |\n";
        std::cout << "  | 2. Attendance Defaulters     |\n";
        std::cout << "  | 3. Fee Defaulters            |\n";
        std::cout << "  | 4. Semester Result Sheet     |\n";
        std::cout << "  | 5. Department Summary        |\n";
        std::cout << "  | 6. Export Report to File     |\n";
        std::cout << "  | 0. Back                      |\n";
        std::cout << "  +------------------------------+\n";
        std::cout << "  Choice: ";
        std::cin >> choice;
        clearBuffer();
        if (choice == 0) break;
        if (choice == 1) {
            printMeritList();
        } else if (choice == 2) {
            printAttendanceDefaulters();
        } else if (choice == 3) {
            printFeeDefaulters();
        } else if (choice == 4) {
            std::string sem;
            std::cout << "  Semester: "; std::getline(std::cin, sem);
            printSemesterResult(sem);
        } else if (choice == 5) {
            std::string sem;
            std::cout << "  Semester: "; std::getline(std::cin, sem);
            printDepartmentSummary(sem);
        } else if (choice == 6) {
            std::string sem;
            int rpt;
            std::cout << "  Report (1=Merit 2=AttDefault 3=FeeDefault 4=SemResult 5=DeptSummary): ";
            std::cin >> rpt; clearBuffer();
            std::cout << "  Semester (if needed): "; std::getline(std::cin, sem);
            exportReportToFile(rpt, sem);
        } else {
            std::cout << "  [Invalid choice]\n";
        }
    }
}

// ============================================================
//  MAIN MENU
// ============================================================
int main() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════╗\n";
    std::cout << "  ║   CAMPUS ANALYTICS ENGINE  v1.0      ║\n";
    std::cout << "  ║   BS Artificial Intelligence         ║\n";
    std::cout << "  ╚══════════════════════════════════════╝\n";
    int choice = 0;
    while (true) {
        std::cout << "\n  ===== MAIN MENU =====\n";
        std::cout << "  1. Student Operations\n";
        std::cout << "  2. Course Operations\n";
        std::cout << "  3. Attendance\n";
        std::cout << "  4. Grades\n";
        std::cout << "  5. Fee Tracker\n";
        std::cout << "  6. Reports\n";
        std::cout << "  0. Exit\n";
        std::cout << "  Choice: ";
        std::cin >> choice;
        clearBuffer();
        switch (choice) {
            case 1: menuStudentOps(); break;
            case 2: menuCourseOps(); break;
            case 3: menuAttendance(); break;
            case 4: menuGrades(); break;
            case 5: menuFees(); break;
            case 6: menuReports(); break;
            case 0:
                std::cout << "\n  Goodbye!\n\n";
                return 0;
            default:
                std::cout << "  [Invalid choice. Enter 0-6]\n";
        }
    }
    return 0;
}
