#ifndef COURSE_OPS_H
#define COURSE_OPS_H

#include <string>
#include <vector>

// Enrollment result enum
enum EnrollResult {
    ENROLL_SUCCESS,
    ENROLL_STUDENT_INACTIVE,
    ENROLL_COURSE_NOT_FOUND,
    ENROLL_SEATS_FULL,
    ENROLL_ALREADY_ENROLLED,
    ENROLL_CREDIT_OVERLOAD,
    ENROLL_PREREQ_NOT_MET
};

// Prints the string message for an EnrollResult
void printEnrollResult(EnrollResult res);

// Enroll student in a course: checks all conditions
EnrollResult enrollStudent(const std::string& roll, const std::string& courseCode, const std::string& semester);

// Drop a course: only if no attendance exists for this student+course+semester
bool dropCourse(const std::string& roll, const std::string& courseCode, const std::string& semester);

// Returns total credit hours of active enrollments for a student in a semester
int getCreditLoad(const std::string& roll, const std::string& semester);

// Checks if student has passed the prerequisite for a course (non-F grade)
bool checkPrerequisite(const std::string& roll, const std::string& courseCode);

// Returns all actively enrolled students in a course
std::vector<std::vector<std::string>> listEnrolledStudents(const std::string& courseCode);

// Returns next enrollment ID string (e.g. E0125)
std::string nextEnrollID();

// Returns course row by code (from courses.txt)
std::vector<std::string> getCourseByCode(const std::string& code);

// Lists all courses
void printAllCourses();

#endif
