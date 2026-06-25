#ifndef GRADES_H
#define GRADES_H

#include <string>
#include <vector>

struct Stats {
    double highest;
    double lowest;
    double mean;
    double median;
};

// Enter marks for a student in a course
void enterMarks(const std::string& roll, const std::string& courseCode, const std::string& semester);

// Returns best 3 of up to 5 quiz marks average (no sort used)
double bestThreeOfFive(double quizzes[], int n);

// Computes weighted total: quiz*0.10 + asgn*0.10 + mid*0.30 + final*0.50
double computeWeightedTotal(double quizAvg, double asgnAvg, double mid, double finalMark);

// Maps score to letter grade
std::string getLetterGrade(double score);

// Credit-weighted semester GPA for a student
double computeGPA(const std::string& roll, const std::string& semester);

// Computes class stats for a course
Stats computeClassStats(const std::string& courseCode, const std::string& semester);

// Applies attendance penalty: sets grade to F if attendance < 75%
void applyAttendancePenalty(const std::string& roll, const std::string& courseCode, const std::string& semester);

// Prints grades for a student
void printStudentGrades(const std::string& roll, const std::string& semester);

#endif
