/*
 SRMS - Student Report Management System (C)
 - Menu driven
 - File persistence: accounts.dat (binary), students.dat (binary)
 - Admin role ('A') can add/modify/delete students and accounts and perform analytics
 - Student role ('S') can view their own record only
 - Each student: 5 subjects (DAA, DE, Discrete Maths, C++ OOPS, Coding Skills)
 - CGPA computed as: cgpa = (total / 500.0) * 10.0
 - Added features: average CGPA, record count, highest/lowest CGPA, sorting, export to CSV
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 1000
#define MAX_STUDENTS 2000
#define USERNAME_LEN 50
#define PASSWORD_LEN 50
#define ID_LEN 30
#define NAME_LEN 80

const char *ACCOUNTS_FILE = "accounts.dat";
const char *STUDENTS_FILE = "students.dat";
const char *STUDENTS_CSV  = "students.csv";

typedef struct {
    char role; // 'A' for admin, 'S' for student
    char id[ID_LEN]; // unique id (e.g., roll no)
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
} Account;

typedef struct {
    char id[ID_LEN];    // must match Account.id for student users
    char name[NAME_LEN];
    int marks[5];       // DAA, DE, Discrete Maths, C++ OOPS, Coding Skills
    int total;
    float cgpa;
} Student;

/* In-memory arrays */
Account accounts[MAX_USERS];
int accounts_count = 0;

Student students[MAX_STUDENTS];
int students_count = 0;

/* ---------- Utility functions ---------- */

void clear_console() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void press_enter_to_continue() {
    printf("\nPress Enter to continue...");
    while (getchar() != '\n'); // flush
}

/* finders */
int find_account_by_username(const char *username) {
    for (int i = 0; i < accounts_count; ++i) {
        if (strcmp(accounts[i].username, username) == 0) return i;
    }
    return -1;
}

int find_account_by_id(const char *id) {
    for (int i = 0; i < accounts_count; ++i) {
        if (strcmp(accounts[i].id, id) == 0) return i;
    }
    return -1;
}

int find_student_by_id(const char *id) {
    for (int i = 0; i < students_count; ++i) {
        if (strcmp(students[i].id, id) == 0) return i;
    }
    return -1;
}

void calc_total_and_cgpa(Student *s) {
    int sum = 0;
    for (int i = 0; i < 5; ++i) sum += s->marks[i];
    s->total = sum;
    s->cgpa = (sum / 500.0f) * 10.0f;
}

/* ---------- File IO ---------- */

void load_accounts() {
    FILE *fp = fopen(ACCOUNTS_FILE, "rb");
    if (!fp) {
        accounts_count = 0;
        return;
    }
    fread(&accounts_count, sizeof(int), 1, fp);
    if (accounts_count > 0) {
        fread(accounts, sizeof(Account), accounts_count, fp);
    }
    fclose(fp);
}

void save_accounts() {
    FILE *fp = fopen(ACCOUNTS_FILE, "wb");
    if (!fp) {
        perror("Unable to open accounts file for writing");
        return;
    }
    fwrite(&accounts_count, sizeof(int), 1, fp);
    if (accounts_count > 0) {
        fwrite(accounts, sizeof(Account), accounts_count, fp);
    }
    fclose(fp);
}

void load_students() {
    FILE *fp = fopen(STUDENTS_FILE, "rb");
    if (!fp) {
        students_count = 0;
        return;
    }
    fread(&students_count, sizeof(int), 1, fp);
    if (students_count > 0) {
        fread(students, sizeof(Student), students_count, fp);
    }
    fclose(fp);
}

void save_students() {
    FILE *fp = fopen(STUDENTS_FILE, "wb");
    if (!fp) {
        perror("Unable to open students file for writing");
        return;
    }
    fwrite(&students_count, sizeof(int), 1, fp);
    if (students_count > 0) {
        fwrite(students, sizeof(Student), students_count, fp);
    }
    fclose(fp);
}

/* ---------- Initial setup ---------- */

void create_admin_if_none() {
    if (accounts_count == 0) {
        Account a;
        a.role = 'A';
        strncpy(a.id, "admin", ID_LEN);
        strncpy(a.username, "admin", USERNAME_LEN);
        strncpy(a.password, "admin", PASSWORD_LEN);
        accounts[accounts_count++] = a;
        save_accounts();
        printf("Default admin created -> username: admin  password: admin\n");
    }
}

/* ---------- Account management ---------- */

void add_account() {
    if (accounts_count >= MAX_USERS) {
        printf("Max accounts reached.\n"); return;
    }
    Account a;
    char rolec;
    printf("Enter role (A for admin / S for student): ");
    if (scanf(" %c", &rolec) != 1) rolec = 'S';
    while (getchar() != '\n');
    if (rolec != 'A' && rolec != 'S') {
        printf("Invalid role.\n"); return;
    }
    a.role = rolec;

    printf("Enter unique id (roll or id): ");
    fgets(a.id, ID_LEN, stdin); a.id[strcspn(a.id, "\n")] = '\0';
    if (find_account_by_id(a.id) != -1) { printf("Account with this id already exists.\n"); return; }

    printf("Enter username: ");
    fgets(a.username, USERNAME_LEN, stdin); a.username[strcspn(a.username, "\n")] = '\0';
    if (find_account_by_username(a.username) != -1) { printf("Username already taken.\n"); return; }

    printf("Enter password: ");
    fgets(a.password, PASSWORD_LEN, stdin); a.password[strcspn(a.password, "\n")] = '\0';

    accounts[accounts_count++] = a;
    save_accounts();
    printf("Account created successfully.\n");
}

void delete_account() {
    char id[ID_LEN];
    printf("Enter account id to delete: ");
    fgets(id, ID_LEN, stdin); id[strcspn(id, "\n")] = '\0';
    int idx = find_account_by_id(id);
    if (idx == -1) { printf("No such account.\n"); return; }
    for (int i = idx; i < accounts_count - 1; ++i) accounts[i] = accounts[i+1];
    accounts_count--;
    save_accounts();
    printf("Account deleted.\n");
}

void list_accounts() {
    if (accounts_count == 0) { printf("No accounts.\n"); return; }
    printf("\nAccounts list:\n");
    for (int i = 0; i < accounts_count; ++i) {
        printf("%d) id: %s | username: %s | role: %c\n", i+1, accounts[i].id, accounts[i].username, accounts[i].role);
    }
}

/* ---------- Student record operations ---------- */

void add_student_record() {
    if (students_count >= MAX_STUDENTS) { printf("Student storage full.\n"); return; }
    Student s;
    printf("Enter student id (must match account id): ");
    fgets(s.id, ID_LEN, stdin); s.id[strcspn(s.id, "\n")] = '\0';
    if (find_student_by_id(s.id) != -1) { printf("Student record already exists.\n"); return; }
    if (find_account_by_id(s.id) == -1) {
        printf("Warning: No account exists for this id. Create account first if needed.\n");
    }
    printf("Enter student name: ");
    fgets(s.name, NAME_LEN, stdin); s.name[strcspn(s.name, "\n")] = '\0';

    const char *subjects[5] = {"DAA", "DE", "Discrete Maths", "C++ OOPS", "Coding Skills"};
    for (int i = 0; i < 5; ++i) {
        int m;
        printf("Enter marks for %s (0-100): ", subjects[i]);
        if (scanf("%d", &m) != 1) m = 0;
        while (getchar() != '\n');
        if (m < 0) m = 0; if (m > 100) m = 100;
        s.marks[i] = m;
    }
    calc_total_and_cgpa(&s);
    students[students_count++] = s;
    save_students();
    printf("Student record added successfully. Total=%d CGPA=%.2f\n", s.total, s.cgpa);
}

void modify_student_record() {
    char id[ID_LEN];
    printf("Enter student id to modify: ");
    fgets(id, ID_LEN, stdin); id[strcspn(id, "\n")] = '\0';
    int idx = find_student_by_id(id);
    if (idx == -1) { printf("Student not found.\n"); return; }
    Student *s = &students[idx];
    printf("Modifying record for %s (%s)\n", s->name, s->id);
    const char *subjects[5] = {"DAA", "DE", "Discrete Maths", "C++ OOPS", "Coding Skills"};
    for (int i = 0; i < 5; ++i) {
        printf("%s current marks = %d. Enter new marks (-1 to keep): ", subjects[i], s->marks[i]);
        int m;
        if (scanf("%d", &m) != 1) m = -1;
        while (getchar() != '\n');
        if (m >= 0 && m <= 100) s->marks[i] = m;
    }
    calc_total_and_cgpa(s);
    save_students();
    printf("Record updated. Total=%d CGPA=%.2f\n", s->total, s->cgpa);
}

void delete_student_record() {
    char id[ID_LEN];
    printf("Enter student id to delete: ");
    fgets(id, ID_LEN, stdin); id[strcspn(id, "\n")] = '\0';
    int idx = find_student_by_id(id);
    if (idx == -1) { printf("Student not found.\n"); return; }
    for (int i = idx; i < students_count - 1; ++i) students[i] = students[i+1];
    students_count--;
    save_students();
    printf("Student record deleted.\n");
}

void display_student(const Student *s) {
    const char *subjects[5] = {"DAA", "DE", "Discrete Maths", "C++ OOPS", "Coding Skills"};
    printf("\n--- Student Report ---\n");
    printf("ID: %s\nName: %s\n", s->id, s->name);
    for (int i = 0; i < 5; ++i) {
        printf("%s: %d/100\n", subjects[i], s->marks[i]);
    }
    printf("Total: %d/500\n", s->total);
    printf("CGPA (out of 10): %.2f\n", s->cgpa);
}

void view_student_record_by_id() {
    char id[ID_LEN];
    printf("Enter student id: ");
    fgets(id, ID_LEN, stdin); id[strcspn(id, "\n")] = '\0';
    int idx = find_student_by_id(id);
    if (idx == -1) { printf("Student not found.\n"); return; }
    display_student(&students[idx]);
}

/* ---------- Analytics and utilities ---------- */

void calculate_average_cgpa() {
    if (students_count == 0) { printf("No student records found.\n"); return; }
    float sum = 0.0f;
    for (int i = 0; i < students_count; ++i) sum += students[i].cgpa;
    float avg = sum / students_count;
    printf("\nTotal Students: %d\nAverage CGPA of All Students = %.2f\n", students_count, avg);
}

void count_records() {
    printf("\nNumber of student records saved = %d\n", students_count);
}

void highest_lowest_cgpa() {
    if (students_count == 0) { printf("No student records.\n"); return; }
    int hi = 0, lo = 0;
    for (int i = 1; i < students_count; ++i) {
        if (students[i].cgpa > students[hi].cgpa) hi = i;
        if (students[i].cgpa < students[lo].cgpa) lo = i;
    }
    printf("\nHighest CGPA:\n");
    display_student(&students[hi]);
    printf("\nLowest CGPA:\n");
    display_student(&students[lo]);
}

/* Sorting comparators for qsort */
int cmp_cgpa_asc(const void *a, const void *b) {
    const Student *sa = a;
    const Student *sb = b;
    if (sa->cgpa < sb->cgpa) return -1;
    if (sa->cgpa > sb->cgpa) return 1;
    return 0;
}
int cmp_cgpa_desc(const void *a, const void *b) {
    return -cmp_cgpa_asc(a, b);
}
int cmp_name_asc(const void *a, const void *b) {
    const Student *sa = a;
    const Student *sb = b;
    return strcasecmp(sa->name, sb->name);
}

void sort_students_by_cgpa_asc() {
    if (students_count <= 1) { printf("Not enough records to sort.\n"); return; }
    qsort(students, students_count, sizeof(Student), cmp_cgpa_asc);
    save_students();
    printf("Sorted by CGPA (ascending) and saved.\n");
}
void sort_students_by_cgpa_desc() {
    if (students_count <= 1) { printf("Not enough records to sort.\n"); return; }
    qsort(students, students_count, sizeof(Student), cmp_cgpa_desc);
    save_students();
    printf("Sorted by CGPA (descending) and saved.\n");
}
void sort_students_by_name() {
    if (students_count <= 1) { printf("Not enough records to sort.\n"); return; }
    qsort(students, students_count, sizeof(Student), cmp_name_asc);
    save_students();
    printf("Sorted by Name (A-Z) and saved.\n");
}

void export_students_csv() {
    if (students_count == 0) { printf("No students to export.\n"); return; }
    FILE *fp = fopen(STUDENTS_CSV, "w");
    if (!fp) { perror("Unable to open CSV file for writing"); return; }
    fprintf(fp, "ID,Name,DAA,DE,DiscreteMaths,CPP_OOPS,CodingSkills,Total,CGPA\n");
    for (int i = 0; i < students_count; ++i) {
        Student *s = &students[i];
        fprintf(fp, "\"%s\",\"%s\",%d,%d,%d,%d,%d,%d,%.2f\n",
                s->id, s->name,
                s->marks[0], s->marks[1], s->marks[2], s->marks[3], s->marks[4],
                s->total, s->cgpa);
    }
    fclose(fp);
    printf("Exported %d records to %s\n", students_count, STUDENTS_CSV);
}

/* ---------- Menus and login ---------- */

void admin_menu(const Account *me) {
    while (1) {
        printf("\n--- Admin Menu (logged in as %s) ---\n", me->username);
        printf("1. Add account\n2. Delete account\n3. List accounts\n");
        printf("4. Add student record\n5. Modify student record\n6. Delete student record\n");
        printf("7. View student record\n8. List all students\n");
        printf("9. Calculate Average CGPA\n10. Number of Records Saved\n");
        printf("11. Highest & Lowest CGPA\n12. Sorting Options\n13. Export students to CSV\n14. Logout\n");
        printf("Choose: ");
        int ch;
        if (scanf("%d", &ch) != 1) ch = 0;
        while (getchar() != '\n');

        switch (ch) {
            case 1: add_account(); break;
            case 2: delete_account(); break;
            case 3: list_accounts(); break;
            case 4: add_student_record(); break;
            case 5: modify_student_record(); break;
            case 6: delete_student_record(); break;
            case 7: view_student_record_by_id(); break;
            case 8:
                if (students_count == 0) { printf("No students.\n"); break; }
                for (int i = 0; i < students_count; ++i) {
                    printf("%d) %s | %s | Total=%d | CGPA=%.2f\n",
                           i+1, students[i].id, students[i].name, students[i].total, students[i].cgpa);
                }
                break;
            case 9: calculate_average_cgpa(); break;
            case 10: count_records(); break;
            case 11: highest_lowest_cgpa(); break;
            case 12: {
                printf("\nSorting Options:\n1. By CGPA (ascending)\n2. By CGPA (descending)\n3. By Name (A-Z)\nChoose: ");
                int sopt;
                if (scanf("%d", &sopt) != 1) sopt = 0;
                while (getchar() != '\n');
                if (sopt == 1) sort_students_by_cgpa_asc();
                else if (sopt == 2) sort_students_by_cgpa_desc();
                else if (sopt == 3) sort_students_by_name();
                else printf("Invalid sort option.\n");
                break;
            }
            case 13: export_students_csv(); break;
            case 14: return;
            default: printf("Invalid choice.\n");
        }
        press_enter_to_continue();
        clear_console();
    }
}

void student_menu(const Account *me) {
    while (1) {
        printf("\n--- Student Menu (logged in as %s) ---\n", me->username);
        printf("1. View my record\n2. Logout\nChoose: ");
        int ch;
        if (scanf("%d", &ch) != 1) ch = 0;
        while (getchar() != '\n');

        switch (ch) {
            case 1: {
                int idx = find_student_by_id(me->id);
                if (idx == -1) {
                    printf("No record found for your id (%s).\n", me->id);
                } else {
                    display_student(&students[idx]);
                }
                break;
            }
            case 2: return;
            default: printf("Invalid choice.\n");
        }
        press_enter_to_continue();
        clear_console();
    }
}

void signup_flow() {
    printf("Signup as new student account\n");
    Account a;
    a.role = 'S';
    printf("Enter unique id (roll/id): ");
    fgets(a.id, ID_LEN, stdin); a.id[strcspn(a.id, "\n")] = '\0';
    if (find_account_by_id(a.id) != -1) { printf("Account with this id exists already.\n"); return; }
    printf("Enter username: ");
    fgets(a.username, USERNAME_LEN, stdin); a.username[strcspn(a.username, "\n")] = '\0';
    if (find_account_by_username(a.username) != -1) { printf("Username taken.\n"); return; }
    printf("Enter password: ");
    fgets(a.password, PASSWORD_LEN, stdin); a.password[strcspn(a.password, "\n")] = '\0';
    accounts[accounts_count++] = a;
    save_accounts();
    printf("Student account created. You can now login.\n");
}

void login_flow() {
    printf("Login\n");
    char uname[USERNAME_LEN], pass[PASSWORD_LEN];
    printf("Username: ");
    fgets(uname, USERNAME_LEN, stdin); uname[strcspn(uname, "\n")] = '\0';
    printf("Password: ");
    fgets(pass, PASSWORD_LEN, stdin); pass[strcspn(pass, "\n")] = '\0';

    int idx = find_account_by_username(uname);
    if (idx == -1) { printf("Invalid username or password.\n"); return; }
    if (strcmp(accounts[idx].password, pass) != 0) { printf("Invalid username or password.\n"); return; }
    Account *me = &accounts[idx];
    if (me->role == 'A') admin_menu(me);
    else if (me->role == 'S') student_menu(me);
    else printf("Unknown role.\n");
}

/* ---------- main ---------- */

int main() {
    load_accounts();
    load_students();
    create_admin_if_none();

    while (1) {
        printf("======== SRMS ========\n");
        printf("1. Login\n2. Signup (student)\n3. Exit\n");
        printf("Choose option: ");
        int opt;
        if (scanf("%d", &opt) != 1) opt = 0;
        while (getchar() != '\n');

        switch (opt) {
            case 1: login_flow(); break;
            case 2: signup_flow(); break;
            case 3:
                printf("Exiting. Goodbye!\n");
                save_accounts();
                save_students();
                exit(0);
            default: printf("Invalid option.\n");
        }
        press_enter_to_continue();
        clear_console();
    }
    return 0;
}
