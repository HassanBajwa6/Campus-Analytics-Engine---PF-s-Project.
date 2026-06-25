#ifndef FEE_TRACKER_H
#define FEE_TRACKER_H

#include <string>
#include <vector>

// Validates DD-MM-YYYY date format with string checks
bool validateDateFormat(const std::string& date);

// Records a payment for a student: validates date, updates fees.txt
bool recordPayment(const std::string& roll, const std::string& semester, double amount, const std::string& payDate);

// Computes late fine: 2% per complete week past due date
double computeLateFine(const std::string& roll, const std::string& semester);

// Calculates absolute number of days between two DD-MM-YYYY dates (no ctime)
int daysBetween(const std::string& date1, const std::string& date2);

// Prints formatted receipt for a student
void generateReceipt(const std::string& roll, const std::string& semester);

// Returns students with balance > 0 past due date, sorted by outstanding amount (bubble sort)
std::vector<std::vector<std::string>> getDefaulters();

// Prints all defaulters
void printDefaulters();

#endif
