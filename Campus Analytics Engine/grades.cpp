#include "grades.h"
#include "filehandler.h"
#include "student_ops.h"
#include "attendance.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

const std::string GRADES_FILE   = "grades.txt";
const std::string GRADES_HEADER = "roll_no,course_code,semester,quiz_avg,asgn_avg,mid_marks,final_marks,total,letter_grade";
// Columns: 0=roll, 1=course, 2=semester, 3=quiz_avg, 4=asgn_avg, 5=mid, 6=final, 7=total, 8=letter_grade

double bestThreeOfFive(double quizzes[], int n) {
    if (n <= 0) return 0.0;
    // Simple bubble sort to order quiz scores ascending
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (quizzes[j] > quizzes[j+1]) {
                double t = quizzes[j];
                quizzes[j] = quizzes[j+1];
                quizzes[j+1] = t;
            }
        }
    }
    if (n == 1) return quizzes[0];
    if (n == 2) return (quizzes[0] + quizzes[1]) / 2.0;
    if (n == 3) return (quizzes[0] + quizzes[1] + quizzes[2]) / 3.0;
    if (n == 4) return (quizzes[1] + quizzes[2] + quizzes[3]) / 3.0;
    // n == 5 or more (up to 5)
    return (quizzes[n-3] + quizzes[n-2] + quizzes[n-1]) / 3.0;
}

double computeWeightedTotal(double quizAvg, double asgnAvg, double mid, double finalMark) {
    return (quizAvg * 0.10) + (asgnAvg * 0.10) + (mid * 0.30) + (finalMark * 0.50);
}

std::string getLetterGrade(double score) {
    if (score >= 85) return "A";
    if (score >= 80) return "B+";
    if (score >= 70) return "B";
    if (score >= 65) return "C+";
    if (score >= 60) return "C";
    if (score >= 50) return "D";
    return "F";
}

// Helper: double to string with 2 decimals
std::string dblToStr(double val) {
    // Round to 2 decimal places first to prevent fractional carriage overflow
    double rounded = (int)(val * 100.0 + 0.5) / 100.0;
    int whole = (int)rounded;
    int frac = (int)((rounded - whole) * 100 + 0.5);
    if (frac >= 100) {
        whole += 1;
        frac -= 100;
    }
    std::string ws = "", fs = "";
    if (whole == 0) ws = "0";
    int tmp = whole;
    while (tmp > 0) { ws = (char)('0' + tmp % 10) + ws; tmp /= 10; }
    if (frac < 10) {
        fs = "0";
        fs += (char)('0' + frac);
    } else {
        fs += (char)('0' + frac / 10);
        fs += (char)('0' + frac % 10);
    }
    return ws + "." + fs;
}

void enterMarks(const std::string& roll, const std::string& courseCode, const std::string& semester) {
    std::cout << "\n--- Enter Marks: " << roll << " | " << courseCode << " ---\n";
    // Quiz marks (up to 5)
    int numQuizzes = 0;
    std::cout << "How many quiz marks to enter (1-5)? ";
    std::cin >> numQuizzes;
    if (numQuizzes < 1) numQuizzes = 1;
    if (numQuizzes > 5) numQuizzes = 5;
    double quizzes[5] = {0,0,0,0,0};
    for (int i = 0; i < numQuizzes; i++) {
        std::cout << "  Quiz " << (i+1) << " (0-10): ";
        std::cin >> quizzes[i];
        if (quizzes[i] < 0) quizzes[i] = 0;
        if (quizzes[i] > 10) quizzes[i] = 10;
    }
    double quizAvg = bestThreeOfFive(quizzes, numQuizzes);
    // Convert quiz avg to /10 percentage
    double quizPct = quizAvg; // already 0-10, will be weighted on 10%

    // Assignments (up to 3)
    int numAsgn = 0;
    std::cout << "How many assignments (1-3)? ";
    std::cin >> numAsgn;
    if (numAsgn < 1) numAsgn = 1;
    if (numAsgn > 3) numAsgn = 3;
    double asgns[3] = {0,0,0};
    for (int i = 0; i < numAsgn; i++) {
        std::cout << "  Assignment " << (i+1) << " (0-10): ";
        std::cin >> asgns[i];
        if (asgns[i] < 0) asgns[i] = 0;
        if (asgns[i] > 10) asgns[i] = 10;
    }
    double asgnSum = 0;
    for (int i = 0; i < numAsgn; i++) asgnSum += asgns[i];
    double asgnAvg = asgnSum / numAsgn;

    double mid = 0, finalMark = 0;
    std::cout << "Mid marks (0-40): ";
    std::cin >> mid;
    if (mid < 0) mid = 0; if (mid > 40) mid = 40;
    std::cout << "Final marks (0-60): ";
    std::cin >> finalMark;
    if (finalMark < 0) finalMark = 0; if (finalMark > 60) finalMark = 60;

    // Normalize to percentages for weighted formula
    double quizNorm  = (quizPct / 10.0) * 100.0;
    double asgnNorm  = (asgnAvg / 10.0) * 100.0;
    double midNorm   = (mid / 40.0) * 100.0;
    double finalNorm = (finalMark / 60.0) * 100.0;

    double total = computeWeightedTotal(quizNorm, asgnNorm, midNorm, finalNorm);
    std::string grade = getLetterGrade(total);

    // Check if record exists; if so, update
    std::vector<std::vector<std::string>> grades = readTXT(GRADES_FILE);
    bool found = false;
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 3 &&
            grades[i][0] == roll &&
            grades[i][1] == courseCode &&
            grades[i][2] == semester) {
            grades[i].resize(9);
            grades[i][3] = dblToStr(quizNorm);
            grades[i][4] = dblToStr(asgnNorm);
            grades[i][5] = dblToStr(mid);
            grades[i][6] = dblToStr(finalMark);
            grades[i][7] = dblToStr(total);
            grades[i][8] = grade;
            found = true;
            break;
        }
    }
    if (found) {
        writeTXT(GRADES_FILE, GRADES_HEADER, grades);
    } else {
        std::vector<std::string> newRow;
        newRow.push_back(roll);
        newRow.push_back(courseCode);
        newRow.push_back(semester);
        newRow.push_back(dblToStr(quizNorm));
        newRow.push_back(dblToStr(asgnNorm));
        newRow.push_back(dblToStr(mid));
        newRow.push_back(dblToStr(finalMark));
        newRow.push_back(dblToStr(total));
        newRow.push_back(grade);
        appendTXT(GRADES_FILE, newRow);
    }
    std::cout << "\n  Total: " << dblToStr(total) << " | Grade: " << grade << "\n";
    std::cout << "[SUCCESS] Marks saved.\n";
    // Apply attendance penalty
    applyAttendancePenalty(roll, courseCode, semester);
}

double computeGPA(const std::string& roll, const std::string& semester) {
    std::vector<std::vector<std::string>> grades = readTXT(GRADES_FILE);
    std::vector<std::vector<std::string>> courses = readTXT("courses.txt");
    double totalPoints = 0.0;
    int totalCredits = 0;
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 9 &&
            grades[i][0] == roll &&
            grades[i][2] == semester) {
            // Get credit hours
            int credits = 3; // default
            for (int j = 0; j < (int)courses.size(); j++) {
                if (courses[j].size() >= 3 && courses[j][0] == grades[i][1]) {
                    credits = atoi(courses[j][2].c_str());
                    break;
                }
            }
            // GPA points from letter grade
            std::string lg = grades[i][8];
            double gpaPoint = 0.0;
            if (lg == "A")  gpaPoint = 4.0;
            else if (lg == "B+") gpaPoint = 3.5;
            else if (lg == "B")  gpaPoint = 3.0;
            else if (lg == "C+") gpaPoint = 2.5;
            else if (lg == "C")  gpaPoint = 2.0;
            else if (lg == "D")  gpaPoint = 1.0;
            else                 gpaPoint = 0.0; // F
            totalPoints  += gpaPoint * credits;
            totalCredits += credits;
        }
    }
    if (totalCredits == 0) return 0.0;
    return totalPoints / totalCredits;
}

Stats computeClassStats(const std::string& courseCode, const std::string& semester) {
    std::vector<std::vector<std::string>> grades = readTXT(GRADES_FILE);
    std::vector<double> totals;
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 8 &&
            grades[i][1] == courseCode &&
            grades[i][2] == semester) {
            totals.push_back(atof(grades[i][7].c_str()));
        }
    }
    Stats s;
    s.highest = 0; s.lowest = 100; s.mean = 0; s.median = 0;
    if (totals.empty()) { s.lowest = 0; return s; }
    double sum = 0;
    for (int i = 0; i < (int)totals.size(); i++) {
        if (totals[i] > s.highest) s.highest = totals[i];
        if (totals[i] < s.lowest)  s.lowest  = totals[i];
        sum += totals[i];
    }
    s.mean = sum / totals.size();
    // Selection sort for median
    for (int i = 0; i < (int)totals.size() - 1; i++) {
        int minIdx = i;
        for (int j = i+1; j < (int)totals.size(); j++) {
            if (totals[j] < totals[minIdx]) minIdx = j;
        }
        double tmp = totals[i]; totals[i] = totals[minIdx]; totals[minIdx] = tmp;
    }
    int n = (int)totals.size();
    s.median = (n % 2 == 0) ? (totals[n/2 - 1] + totals[n/2]) / 2.0 : totals[n/2];
    return s;
}

void applyAttendancePenalty(const std::string& roll, const std::string& courseCode, const std::string& semester) {
    double pct = getAttendancePct(roll, courseCode);
    if (pct < 75.0) {
        std::vector<std::vector<std::string>> grades = readTXT(GRADES_FILE);
        for (int i = 0; i < (int)grades.size(); i++) {
            if (grades[i].size() >= 9 &&
                grades[i][0] == roll &&
                grades[i][1] == courseCode &&
                grades[i][2] == semester) {
                grades[i][8] = "F";
                break;
            }
        }
        writeTXT(GRADES_FILE, GRADES_HEADER, grades);
        std::cout << "[WARNING] Attendance < 75%! Grade overridden to F for " << courseCode << ".\n";
    }
}

void printStudentGrades(const std::string& roll, const std::string& semester) {
    std::vector<std::vector<std::string>> grades = readTXT(GRADES_FILE);
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  GRADE REPORT | " << roll << " | Semester: " << semester << "\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << std::left << std::setw(10) << "Course"
              << std::setw(10) << "Quiz%"
              << std::setw(10) << "Asgn%"
              << std::setw(8)  << "Mid"
              << std::setw(8)  << "Final"
              << std::setw(10) << "Total"
              << "Grade\n";
    std::cout << std::string(70, '-') << "\n";
    bool any = false;
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 9 && grades[i][0] == roll && grades[i][2] == semester) {
            std::cout << std::left << std::setw(10) << grades[i][1]
                      << std::setw(10) << grades[i][3]
                      << std::setw(10) << grades[i][4]
                      << std::setw(8)  << grades[i][5]
                      << std::setw(8)  << grades[i][6]
                      << std::setw(10) << grades[i][7]
                      << grades[i][8] << "\n";
            any = true;
        }
    }
    if (!any) std::cout << "  No grade records found.\n";
    double gpa = computeGPA(roll, semester);
    std::cout << std::string(70, '-') << "\n";
    std::cout << "  Semester GPA: " << std::fixed << std::setprecision(2) << gpa << "\n";
    std::cout << std::string(70, '=') << "\n";
}
