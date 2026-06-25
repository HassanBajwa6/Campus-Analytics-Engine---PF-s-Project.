#include "course_ops.h"
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

const std::string COURSES_FILE     = "courses.txt";
const std::string ENROLLMENTS_FILE = "enrollments.txt";
const std::string GRADES_FILE      = "grades.txt";
const std::string ATTENDANCE_FILE  = "attendance_log.txt";

const std::string ENROLL_HEADER = "enrollment_id,roll_no,course_code,semester,enrollment_date,status";

// Columns in courses.txt:      0=code, 1=name, 2=credits, 3=instructor, 4=capacity, 5=enrolled, 6=prereq
// Columns in enrollments.txt:  0=id,   1=roll, 2=course,  3=semester,   4=date,     5=status
// Columns in grades.txt:       0=roll, 1=course, 2=semester, ... 8=letter_grade

void printEnrollResult(EnrollResult res) {
    switch(res) {
        case ENROLL_SUCCESS:          std::cout << "[SUCCESS] Student enrolled successfully.\n"; break;
        case ENROLL_STUDENT_INACTIVE: std::cout << "[ERROR] Student is inactive.\n"; break;
        case ENROLL_COURSE_NOT_FOUND: std::cout << "[ERROR] Course not found.\n"; break;
        case ENROLL_SEATS_FULL:       std::cout << "[ERROR] No seats available in this course.\n"; break;
        case ENROLL_ALREADY_ENROLLED: std::cout << "[ERROR] Student is already enrolled in this course.\n"; break;
        case ENROLL_CREDIT_OVERLOAD:  std::cout << "[ERROR] Credit load exceeds 21 hours.\n"; break;
        case ENROLL_PREREQ_NOT_MET:   std::cout << "[ERROR] Prerequisite not met.\n"; break;
    }
}

std::vector<std::string> getCourseByCode(const std::string& code) {
    std::vector<std::vector<std::string>> courses = readTXT(COURSES_FILE);
    return findRow(courses, 0, code);
}

void printAllCourses() {
    std::vector<std::vector<std::string>> courses = readTXT(COURSES_FILE);
    std::cout << "\n" << std::string(70, '-') << "\n";
    std::cout << std::left << std::setw(8) << "Code"
              << std::setw(40) << "Title"
              << std::setw(8) << "Credits"
              << std::setw(10) << "Seats"
              << "Prereq\n";
    std::cout << std::string(70, '-') << "\n";
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].size() >= 7) {
            std::cout << std::left << std::setw(8) << courses[i][0]
                      << std::setw(40) << courses[i][1]
                      << std::setw(8) << courses[i][2]
                      << std::setw(10) << (courses[i][4] + "/" + courses[i][5])
                      << courses[i][6] << "\n";
        }
    }
    std::cout << std::string(70, '-') << "\n";
}

int getCreditLoad(const std::string& roll, const std::string& semester) {
    std::vector<std::vector<std::string>> enrollments = readTXT(ENROLLMENTS_FILE);
    std::vector<std::vector<std::string>> courses     = readTXT(COURSES_FILE);
    int totalCredits = 0;
    for (int i = 0; i < (int)enrollments.size(); i++) {
        if (enrollments[i].size() >= 6 &&
            enrollments[i][1] == roll &&
            enrollments[i][3] == semester &&
            enrollments[i][5] == "active") {
            // Find this course's credit hours
            std::string courseCode = enrollments[i][2];
            for (int j = 0; j < (int)courses.size(); j++) {
                if (courses[j].size() >= 3 && courses[j][0] == courseCode) {
                    totalCredits += atoi(courses[j][2].c_str());
                    break;
                }
            }
        }
    }
    return totalCredits;
}

bool checkPrerequisite(const std::string& roll, const std::string& courseCode) {
    std::vector<std::string> course = getCourseByCode(courseCode);
    if (course.empty() || course.size() < 7) return true; // no prereq info
    std::string prereq = course[6];
    if (prereq == "NONE" || prereq.empty()) return true;
    // Check grades file for non-F grade in prereq course
    std::vector<std::vector<std::string>> grades = readTXT(GRADES_FILE);
    // grades columns: 0=roll, 1=course, 2=semester, 3=quiz, 4=assignment, 5=mid, 6=final, 7=total, 8=letter_grade
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 9 &&
            grades[i][0] == roll &&
            grades[i][1] == prereq &&
            grades[i][8] != "F") {
            return true;
        }
    }
    return false;
}

std::string nextEnrollID() {
    std::vector<std::vector<std::string>> enroll = readTXT(ENROLLMENTS_FILE);
    int maxNum = 0;
    for (int i = 0; i < (int)enroll.size(); i++) {
        if (!enroll[i].empty()) {
            std::string id = enroll[i][0]; // e.g. E0124
            if (id.length() >= 2 && id[0] == 'E') {
                int num = atoi(id.substr(1).c_str());
                if (num > maxNum) maxNum = num;
            }
        }
    }
    int next = maxNum + 1;
    std::string sid = "";
    // Build zero-padded number
    std::string numStr = "";
    int tmp = next;
    if (tmp == 0) numStr = "0";
    while (tmp > 0) { numStr = (char)('0' + tmp % 10) + numStr; tmp /= 10; }
    while ((int)numStr.length() < 4) numStr = "0" + numStr;
    return "E" + numStr;
}

EnrollResult enrollStudent(const std::string& roll, const std::string& courseCode, const std::string& semester) {
    // Check student active
    std::vector<std::string> student = searchByRoll(roll);
    if (student.empty() || student.size() < 6 || student[5] != "active") {
        return ENROLL_STUDENT_INACTIVE;
    }
    // Check course exists
    std::vector<std::string> course = getCourseByCode(courseCode);
    if (course.empty()) return ENROLL_COURSE_NOT_FOUND;
    // Check if already enrolled
    std::vector<std::vector<std::string>> enrollments = readTXT(ENROLLMENTS_FILE);
    for (int i = 0; i < (int)enrollments.size(); i++) {
        if (enrollments[i].size() >= 6 &&
            enrollments[i][1] == roll &&
            enrollments[i][2] == courseCode &&
            enrollments[i][3] == semester &&
            enrollments[i][5] == "active") {
            return ENROLL_ALREADY_ENROLLED;
        }
    }
    // Check credit load
    std::vector<std::vector<std::string>> courses = readTXT(COURSES_FILE);
    int courseCredits = 0;
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].size() >= 3 && courses[i][0] == courseCode) {
            courseCredits = atoi(courses[i][2].c_str());
            break;
        }
    }
    int currentLoad = getCreditLoad(roll, semester);
    if (currentLoad + courseCredits > 21) return ENROLL_CREDIT_OVERLOAD;
    // Check seats
    int capacity = 0, enrolled = 0;
    if (course.size() >= 6) {
        capacity = atoi(course[4].c_str());
        enrolled = atoi(course[5].c_str());
    }
    if (enrolled >= capacity) return ENROLL_SEATS_FULL;
    // Check prerequisite
    if (!checkPrerequisite(roll, courseCode)) return ENROLL_PREREQ_NOT_MET;
    // All checks passed — enroll
    std::string newID = nextEnrollID();
    std::vector<std::string> newRow;
    newRow.push_back(newID);
    newRow.push_back(roll);
    newRow.push_back(courseCode);
    newRow.push_back(semester);
    newRow.push_back("15-01-2024");
    newRow.push_back("active");
    appendTXT(ENROLLMENTS_FILE, newRow);
    // Update enrolled count in courses.txt
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].size() >= 6 && courses[i][0] == courseCode) {
            int cnt = atoi(courses[i][5].c_str()) + 1;
            std::string cntStr = "";
            if (cnt == 0) cntStr = "0";
            int tmp = cnt;
            while (tmp > 0) { cntStr = (char)('0' + tmp % 10) + cntStr; tmp /= 10; }
            courses[i][5] = cntStr;
            break;
        }
    }
    std::string coursesHeader = "course_code,course_name,credit_hours,instructor,capacity,enrolled,prerequisite";
    writeTXT(COURSES_FILE, coursesHeader, courses);
    return ENROLL_SUCCESS;
}

bool dropCourse(const std::string& roll, const std::string& courseCode, const std::string& semester) {
    // Check no attendance exists
    std::vector<std::vector<std::string>> attendance = readTXT(ATTENDANCE_FILE);
    for (int i = 0; i < (int)attendance.size(); i++) {
        if (attendance[i].size() >= 4 &&
            attendance[i][1] == roll &&
            attendance[i][2] == courseCode) {
            std::cout << "[ERROR] Cannot drop: attendance records exist for this course.\n";
            return false;
        }
    }
    // Update enrollment status to 'dropped'
    std::vector<std::vector<std::string>> enrollments = readTXT(ENROLLMENTS_FILE);
    bool found = false;
    for (int i = 0; i < (int)enrollments.size(); i++) {
        if (enrollments[i].size() >= 6 &&
            enrollments[i][1] == roll &&
            enrollments[i][2] == courseCode &&
            enrollments[i][3] == semester &&
            enrollments[i][5] == "active") {
            enrollments[i][5] = "dropped";
            found = true;
            break;
        }
    }
    if (!found) { std::cout << "[ERROR] Enrollment not found.\n"; return false; }
    writeTXT(ENROLLMENTS_FILE, ENROLL_HEADER, enrollments);
    // Decrement enrolled count
    std::vector<std::vector<std::string>> courses = readTXT(COURSES_FILE);
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].size() >= 6 && courses[i][0] == courseCode) {
            int cnt = atoi(courses[i][5].c_str()) - 1;
            if (cnt < 0) cnt = 0;
            std::string cntStr = "";
            if (cnt == 0) cntStr = "0";
            int tmp = cnt;
            while (tmp > 0) { cntStr = (char)('0' + tmp % 10) + cntStr; tmp /= 10; }
            courses[i][5] = cntStr;
            break;
        }
    }
    std::string coursesHeader = "course_code,course_name,credit_hours,instructor,capacity,enrolled,prerequisite";
    writeTXT(COURSES_FILE, coursesHeader, courses);
    std::cout << "[SUCCESS] Course dropped.\n";
    return true;
}

std::vector<std::vector<std::string>> listEnrolledStudents(const std::string& courseCode) {
    std::vector<std::vector<std::string>> enrollments = readTXT(ENROLLMENTS_FILE);
    std::vector<std::vector<std::string>> result;
    for (int i = 0; i < (int)enrollments.size(); i++) {
        if (enrollments[i].size() >= 6 &&
            enrollments[i][2] == courseCode &&
            enrollments[i][5] == "active") {
            result.push_back(enrollments[i]);
        }
    }
    return result;
}
