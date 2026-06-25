#ifndef REPORTS_H
#define REPORTS_H

#include <string>
#include <vector>

// Prints merit list: active students sorted by CGPA desc with rank
void printMeritList();

// Prints attendance defaulters (any course < 75%)
void printAttendanceDefaulters();

// Prints fee defaulters with outstanding amount
void printFeeDefaulters();

// Prints full semester result sheet for all students
void printSemesterResult(const std::string& semester);

// Groups students by dept, shows count, avg CGPA, pass rate
void printDepartmentSummary(const std::string& semester);

// Redirects cout to file for a chosen report
void exportReportToFile(int reportChoice, const std::string& semester);

#endif
