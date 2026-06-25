# Campus Analytics Engine
### BS Artificial Intelligence — Programming Fundamentals Project

A multi-file, menu-driven data analytics system for a fictional university campus built in **pure C++** with no OOP, no STL algorithms, and no forbidden libraries.

---

## Features

| Module | Description |
|--------|-------------|
| **M1 – filehandler** | CSV read/write/append with quoted-field support |
| **M2 – student_ops** | Add, search, update, soft-delete students; roll validation |
| **M3 – course_ops** | Enroll/drop with prereq, seat, and credit-load checks |
| **M4 – attendance** | Mark P/A/L, compute %, shortage list, undo last session |
| **M5 – grades** | Enter marks, bestThreeOfFive, weighted total, GPA, attendance penalty |
| **M6 – fee_tracker** | Record payments, late fine (manual date math), receipts |
| **M7 – reports** | Merit list, attendance/fee defaulters, semester result, department summary, export |

**Bonus:** Search-as-you-type with prefix matching (Option 7 in Student Operations).

---

## File Structure

```
campus_analytics/
├── main.cpp              # Entry point, 3-level nested menu
├── filehandler.h/.cpp    # All file I/O
├── student_ops.h/.cpp    # Student CRUD
├── course_ops.h/.cpp     # Course enrollment
├── attendance.h/.cpp     # Attendance tracking
├── grades.h/.cpp         # Marks and GPA
├── fee_tracker.h/.cpp    # Fee and fine management
├── reports.h/.cpp        # All report generation
├── students.txt          # Student data
├── courses.txt           # Course catalog
├── enrollments.txt       # Enrollment records
├── attendance_log.txt    # Attendance sessions
└── fees.txt              # Fee transactions
```

---

## Compile & Run

### Linux / macOS
```bash
g++ -o campus_analytics main.cpp filehandler.cpp student_ops.cpp course_ops.cpp attendance.cpp grades.cpp fee_tracker.cpp reports.cpp
./campus_analytics
```

### Windows (MinGW)
```cmd
g++ -o campus_analytics.exe main.cpp filehandler.cpp student_ops.cpp course_ops.cpp attendance.cpp grades.cpp fee_tracker.cpp reports.cpp
campus_analytics.exe
```

> **Note:** Run the executable from the same directory as the `.txt` data files.

---

## Sample Run

```
  ╔══════════════════════════════════════╗
  ║   CAMPUS ANALYTICS ENGINE  v1.0      ║
  ║   BS Artificial Intelligence         ║
  ╚══════════════════════════════════════╝

  ===== MAIN MENU =====
  1. Student Operations
  2. Course Operations
  3. Attendance
  4. Grades
  5. Fee Tracker
  6. Reports
  0. Exit
  Choice:
```

**Example — View Merit List:**
```
  Choice: 6    (Reports)
  Choice: 1    (Merit List)

=================================================================
  MERIT LIST (Active Students by CGPA)
=================================================================
Rank  Roll No        Name                          CGPA
-----------------------------------------------------------------
1     BSAI-23-006    Sana Pervez                   3.90
2     BSAI-23-012    Maham Javed                   3.80
...
```

---

## PF Concepts Demonstrated

- **Arrays & loops** — sorting (selection sort, bubble sort), iteration
- **Functions** — all logic split across 8 .cpp files with .h headers
- **Structs** — `Stats` struct for class statistics, `EnrollResult` enum
- **File I/O** — `ifstream`/`ofstream` for read/write/append
- **String operations** — `substr`, `length`, manual parsing (no regex)
- **Nested loops** — credit load, prerequisite checking, report generation
- **Sorting** — selection sort (students by CGPA/roll), bubble sort (fee defaulters)
- **iomanip** — `setw`, `setfill`, `setprecision` for formatted output
- **Manual date arithmetic** — `daysBetween` without `<ctime>`

---

## Constraints Respected

- ❌ No `#include <algorithm>` — no `sort()`, `find()`
- ❌ No `#include <map>` / `<unordered_map>` / `<set>`
- ❌ No `class` keyword — only `struct` used
- ❌ No monolithic single-file submission
- ❌ No hardcoded data — all reads from `.txt` files
- ❌ No `#include <ctime>` — date math done manually

---

## Commit History Guide

| Commit | Description |
|--------|-------------|
| `init` | Initial project structure and data files |
| `feat: filehandler` | M1 — CSV read/write/append/find functions |
| `feat: student_ops` | M2 — Add/search/update/delete students |
| `feat: course_ops` | M3 — Enrollment, credit load, prerequisites |
| `feat: attendance` | M4 — Mark attendance, shortage, undo |
| `feat: grades` | M5 — Marks, GPA, attendance penalty |
| `feat: fee_tracker` | M6 — Payments, late fine, receipts |
| `feat: reports` | M7 — All reports and export |
| `feat: main_menu` | 3-level nested menu in main.cpp |
| `bonus: search-as-you-type` | Prefix matching with per-keystroke reprint |
| `final: integration` | Full build tested and working |
