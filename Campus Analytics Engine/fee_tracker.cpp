#include "fee_tracker.h"
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

const std::string FEES_FILE   = "fees.txt";
const std::string FEES_HEADER = "fee_id,roll_no,semester,total_fee,amount_paid,due_date,payment_date,payment_method,status";
// Columns: 0=fee_id, 1=roll, 2=semester, 3=total_fee, 4=amount_paid, 5=due_date, 6=payment_date, 7=method, 8=status

// Month lengths (non-leap year; handle leap year manually for Feb)
int monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

bool isLeapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

// Converts DD-MM-YYYY to total days from year 0 (for difference calculation)
int dateToDays(const std::string& date) {
    if (date.length() < 10) return 0;
    int dd = atoi(date.substr(0, 2).c_str());
    int mm = atoi(date.substr(3, 2).c_str());
    int yy = atoi(date.substr(6, 4).c_str());
    if (dd == 0 || mm == 0 || yy == 0) return 0;
    // Days from year 1 to start of this year
    int totalDays = 0;
    for (int y = 1; y < yy; y++) {
        totalDays += isLeapYear(y) ? 366 : 365;
    }
    // Days in completed months this year
    for (int m = 1; m < mm; m++) {
        int days = monthDays[m - 1];
        if (m == 2 && isLeapYear(yy)) days = 29;
        totalDays += days;
    }
    totalDays += dd;
    return totalDays;
}

int daysBetween(const std::string& date1, const std::string& date2) {
    int d1 = dateToDays(date1);
    int d2 = dateToDays(date2);
    int diff = d2 - d1;
    return diff < 0 ? -diff : diff;
}

bool validateDateFormat(const std::string& date) {
    if (date.length() != 10) return false;
    if (date[2] != '-' || date[5] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!(date[i] >= '0' && date[i] <= '9')) return false;
    }
    int dd = atoi(date.substr(0, 2).c_str());
    int mm = atoi(date.substr(3, 2).c_str());
    int yy = atoi(date.substr(6, 4).c_str());
    if (mm < 1 || mm > 12) return false;
    int maxDay = monthDays[mm - 1];
    if (mm == 2 && isLeapYear(yy)) maxDay = 29;
    if (dd < 1 || dd > maxDay) return false;
    return true;
}

double computeLateFine(const std::string& roll, const std::string& semester) {
    std::vector<std::vector<std::string>> fees = readTXT(FEES_FILE);
    for (int i = 0; i < (int)fees.size(); i++) {
        if (fees[i].size() >= 9 &&
            fees[i][1] == roll &&
            fees[i][2] == semester) {
            std::string dueDate  = fees[i][5];
            std::string paidDate = fees[i][6];
            double totalFee      = atof(fees[i][3].c_str());
            double amtPaid       = atof(fees[i][4].c_str());
            if (paidDate == "00-00-0000" || paidDate.empty()) {
                // Not yet paid; fine on outstanding
                return 0.0; // fine calculated based on outstanding, not paid date
            }
            if (!validateDateFormat(paidDate) || !validateDateFormat(dueDate)) return 0.0;
            int diff = dateToDays(paidDate) - dateToDays(dueDate);
            if (diff <= 0) return 0.0;
            int completeWeeks = diff / 7;
            return totalFee * 0.02 * completeWeeks;
        }
    }
    return 0.0;
}

bool recordPayment(const std::string& roll, const std::string& semester, double amount, const std::string& payDate) {
    if (!validateDateFormat(payDate)) {
        std::cout << "[ERROR] Invalid date format. Use DD-MM-YYYY.\n";
        return false;
    }
    std::vector<std::vector<std::string>> fees = readTXT(FEES_FILE);
    bool found = false;
    for (int i = 0; i < (int)fees.size(); i++) {
        if (fees[i].size() >= 9 &&
            fees[i][1] == roll &&
            fees[i][2] == semester) {
            double current = atof(fees[i][4].c_str());
            double total   = atof(fees[i][3].c_str());
            current += amount;
            if (current > total) current = total;
            // Convert current to string
            int whole = (int)current;
            std::string ws = "";
            if (whole == 0) ws = "0";
            int tmp = whole;
            while (tmp > 0) { ws = (char)('0' + tmp % 10) + ws; tmp /= 10; }
            fees[i][4] = ws;
            fees[i][6] = payDate;
            // Update status
            if (current >= total) {
                // Check if late
                int dueD  = dateToDays(fees[i][5]);
                int payD  = dateToDays(payDate);
                fees[i][8] = (payD > dueD) ? "paid_late" : "paid";
            } else {
                fees[i][8] = "partial";
            }
            found = true;
            break;
        }
    }
    if (!found) { std::cout << "[ERROR] Fee record not found.\n"; return false; }
    writeTXT(FEES_FILE, FEES_HEADER, fees);
    std::cout << "[SUCCESS] Payment recorded.\n";
    return true;
}

void generateReceipt(const std::string& roll, const std::string& semester) {
    std::vector<std::vector<std::string>> fees = readTXT(FEES_FILE);
    for (int i = 0; i < (int)fees.size(); i++) {
        if (fees[i].size() >= 9 &&
            fees[i][1] == roll &&
            fees[i][2] == semester) {
            double total  = atof(fees[i][3].c_str());
            double paid   = atof(fees[i][4].c_str());
            double fine   = computeLateFine(roll, semester);
            double balance = (total + fine) - paid;
            if (balance < 0) balance = 0;
            std::vector<std::string> student = searchByRoll(roll);
            std::string name = (student.size() > 1) ? student[1] : roll;
            std::cout << "\n";
            std::cout << std::string(50, '*') << "\n";
            std::cout << "          FEE RECEIPT\n";
            std::cout << std::string(50, '*') << "\n";
            std::cout << std::left << std::setw(20) << "Roll No"   << ": " << roll << "\n";
            std::cout << std::left << std::setw(20) << "Name"      << ": " << name << "\n";
            std::cout << std::left << std::setw(20) << "Semester"  << ": " << semester << "\n";
            std::cout << std::left << std::setw(20) << "Due Date"  << ": " << fees[i][5] << "\n";
            std::cout << std::left << std::setw(20) << "Paid Date" << ": " << fees[i][6] << "\n";
            std::cout << std::string(50, '-') << "\n";
            std::cout << std::left << std::setw(20) << "Tuition Fee"  << ": PKR " << std::setw(10) << std::fixed << std::setprecision(0) << total  << "\n";
            if (fine > 0)
            std::cout << std::left << std::setw(20) << "Late Fine"    << ": PKR " << std::setw(10) << fine   << "\n";
            std::cout << std::left << std::setw(20) << "Amount Paid"  << ": PKR " << std::setw(10) << paid   << "\n";
            std::cout << std::left << std::setw(20) << "Balance Due"  << ": PKR " << std::setw(10) << balance << "\n";
            std::cout << std::left << std::setw(20) << "Status"       << ": " << fees[i][8] << "\n";
            std::cout << std::string(50, '*') << "\n";
            return;
        }
    }
    std::cout << "[ERROR] Fee record not found.\n";
}

std::vector<std::vector<std::string>> getDefaulters() {
    std::vector<std::vector<std::string>> fees = readTXT(FEES_FILE);
    std::vector<std::vector<std::string>> defaulters;
    for (int i = 0; i < (int)fees.size(); i++) {
        if (fees[i].size() >= 9) {
            std::string roll = fees[i][1];
            std::string semester = fees[i][2];
            double total = atof(fees[i][3].c_str());
            double paid  = atof(fees[i][4].c_str());
            double fine  = computeLateFine(roll, semester);
            double bal   = (total + fine) - paid;
            if (bal > 0) {
                std::vector<std::string> row = fees[i];
                // Store outstanding as extra field
                std::string balStr = "";
                int bw = (int)bal;
                if (bw == 0) balStr = "0";
                int tmp = bw;
                while (tmp > 0) { balStr = (char)('0' + tmp % 10) + balStr; tmp /= 10; }
                row.push_back(balStr);
                defaulters.push_back(row);
            }
        }
    }
    // Bubble sort by outstanding amount (descending) — field index 9
    for (int i = 0; i < (int)defaulters.size() - 1; i++) {
        for (int j = 0; j < (int)defaulters.size() - i - 1; j++) {
            double a = (defaulters[j].size() > 9)   ? atof(defaulters[j][9].c_str())   : 0;
            double b = (defaulters[j+1].size() > 9) ? atof(defaulters[j+1][9].c_str()) : 0;
            if (a < b) {
                std::vector<std::string> tmp = defaulters[j];
                defaulters[j] = defaulters[j+1];
                defaulters[j+1] = tmp;
            }
        }
    }
    return defaulters;
}

void printDefaulters() {
    std::vector<std::vector<std::string>> defaulters = getDefaulters();
    std::cout << "\n" << std::string(75, '=') << "\n";
    std::cout << "  FEE DEFAULTERS LIST\n";
    std::cout << std::string(75, '=') << "\n";
    std::cout << std::left << std::setw(15) << "Roll No"
              << std::setw(25) << "Name"
              << std::setw(10) << "Semester"
              << std::setw(12) << "Total Fee"
              << std::setw(12) << "Paid"
              << "Outstanding\n";
    std::cout << std::string(75, '-') << "\n";
    for (int i = 0; i < (int)defaulters.size(); i++) {
        if (defaulters[i].size() >= 9) {
            std::string roll = defaulters[i][1];
            std::vector<std::string> stu = searchByRoll(roll);
            std::string name = (stu.size() > 1) ? stu[1] : "-";
            double total = atof(defaulters[i][3].c_str());
            double paid  = atof(defaulters[i][4].c_str());
            double fine  = computeLateFine(roll, defaulters[i][2]);
            double bal   = (total + fine) - paid;
            std::cout << std::left << std::setw(15) << roll
                      << std::setw(25) << name
                      << std::setw(10) << defaulters[i][2]
                      << std::setw(12) << std::fixed << std::setprecision(0) << total
                      << std::setw(12) << paid
                      << bal << "\n";
        }
    }
    if (defaulters.empty()) std::cout << "  No defaulters found.\n";
    std::cout << std::string(75, '=') << "\n";
}
