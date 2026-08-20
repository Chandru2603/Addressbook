#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CONTACTS 100
#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15
#define MAX_EMAIL_LEN 50
#define FILE_NAME "address_book.dat"

typedef struct {
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
} Contact;

Contact addressBook[MAX_CONTACTS];
int contactCount = 0;

// Helper: Remove trailing newline from fgets
void stripNewline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// Validation: Numbers only
int isNumeric(const char *str) {
    if (strlen(str) == 0) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

// Validation: Basic Email format
int isValidEmail(const char *email) {
    int len = strlen(email);
    if (len < 5 || strchr(email, ' ') != NULL) return 0;

    const char *at = strchr(email, '@');
    if (!at || strchr(at + 1, '@') != NULL || at == email) return 0;

    const char *dot = strchr(at + 1, '.');
    if (!dot || dot == at + 1 || email[len - 1] == '.') return 0;

    return 1;
}

// Duplicate Checks
int isNameExists(const char *name, int excludeIndex) {
    for (int i = 0; i < contactCount; i++) {
        if (i == excludeIndex) continue;
        if (strcmp(addressBook[i].name, name) == 0) return 1;
    }
    return 0;
}

int isPhoneExists(const char *phone, int excludeIndex) {
    for (int i = 0; i < contactCount; i++) {
        if (i == excludeIndex) continue;
        if (strcmp(addressBook[i].phone, phone) == 0) return 1;
    }
    return 0;
}

int isEmailExists(const char *email, int excludeIndex) {
    for (int i = 0; i < contactCount; i++) {
        if (i == excludeIndex) continue;
        if (strcmp(addressBook[i].email, email) == 0) return 1;
    }
    return 0;
}

// File Operations
void loadFromFile() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp != NULL) {
        fread(&contactCount, sizeof(int), 1, fp);
        fread(addressBook, sizeof(Contact), contactCount, fp);
        fclose(fp);
    }
}

void saveToFile() {
    FILE *fp = fopen(FILE_NAME, "wb");
    if (fp == NULL) {
        printf("Error: Could not open file for saving.\n");
        return;
    }
    fwrite(&contactCount, sizeof(int), 1, fp);
    fwrite(addressBook, sizeof(Contact), contactCount, fp);
    fclose(fp);
    printf("Changes saved successfully to %s\n", FILE_NAME);
}

// Centralized Search Component (Shared node in flowchart)
int searchComponent() {
    if (contactCount == 0) {
        printf("\n[Search Component] Address book is empty.\n");
        return -1;
    }

    char term[MAX_NAME_LEN];
    printf("\n[Search Component] Enter Name, Phone, or Email: ");
    fgets(term, MAX_NAME_LEN, stdin);
    stripNewline(term);

    int matches[MAX_CONTACTS];
    int matchCount = 0;

    for (int i = 0; i < contactCount; i++) {
        if (strstr(addressBook[i].name, term) != NULL ||
            strstr(addressBook[i].phone, term) != NULL ||
            strstr(addressBook[i].email, term) != NULL) {
            matches[matchCount++] = i;
        }
    }

    if (matchCount == 0) {
        printf("No matching contact found.\n");
        return -1;
    }

    printf("\n--- Search Matches ---\n");
    for (int m = 0; m < matchCount; m++) {
        int idx = matches[m];
        printf("%d. Name: %s | Phone: %s | Email: %s\n", 
               m + 1, addressBook[idx].name, addressBook[idx].phone, addressBook[idx].email);
    }

    if (matchCount == 1) {
        return matches[0];
    }

    int selection;
    printf("\nSelect target contact number (1-%d): ", matchCount);
    if (scanf("%d", &selection) == 1 && selection >= 1 && selection <= matchCount) {
        getchar();
        return matches[selection - 1];
    }
    getchar();
    return -1;
}

// 1. ADD CONTACT MENU -> ENTER NAME -> ADD DETAIL MENU
void addContactMenu() {
    if (contactCount >= MAX_CONTACTS) {
        printf("Address book capacity full!\n");
        return;
    }

    Contact newContact;

    // Enter the contact name
    printf("\n--- ADD CONTACT MENU ---\n");
    do {
        printf("Enter contact name: ");
        fgets(newContact.name, MAX_NAME_LEN, stdin);
        stripNewline(newContact.name);

        if (strlen(newContact.name) == 0) {
            printf("Name cannot be empty.\n");
        } else if (isNameExists(newContact.name, -1)) {
            printf("Name already exists! Choose another.\n");
        } else {
            break;
        }
    } while (1);

    // Add detail menu (Phone & Email)
    printf("\n--- ADD DETAIL MENU ---\n");
    do {
        printf("Enter phone number: ");
        fgets(newContact.phone, MAX_PHONE_LEN, stdin);
        stripNewline(newContact.phone);

        if (!isNumeric(newContact.phone)) {
            printf("Phone number must contain digits only.\n");
        } else if (isPhoneExists(newContact.phone, -1)) {
            printf("Phone number already exists!\n");
        } else {
            break;
        }
    } while (1);

    do {
        printf("Enter email address: ");
        fgets(newContact.email, MAX_EMAIL_LEN, stdin);
        stripNewline(newContact.email);

        if (!isValidEmail(newContact.email)) {
            printf("Invalid email format (e.g. user@domain.com).\n");
        } else if (isEmailExists(newContact.email, -1)) {
            printf("Email address already exists!\n");
        } else {
            break;
        }
    } while (1);

    addressBook[contactCount++] = newContact;
    printf("\nContact added successfully!\n");
}

// 2. EDIT CONTACT MENU -> SEARCH COMPONENT -> MODIFICATION MENU
void editContactMenu() {
    printf("\n--- EDIT CONTACT MENU ---\n");
    int index = searchComponent();
    if (index == -1) return;

    int modChoice;
    char buffer[MAX_NAME_LEN];

    while (1) {
        printf("\n--- MODIFICATION MENU (Editing: %s) ---\n", addressBook[index].name);
        printf("1. Modify Contact Name\n");
        printf("2. Modify Contact Phone Number\n");
        printf("3. Modify Contact Email Address\n");
        printf("4. Return to Main Menu\n");
        printf("Enter choice (1-4): ");

        if (scanf("%d", &modChoice) != 1) {
            getchar();
            break;
        }
        getchar();

        switch (modChoice) {
            case 1:
                printf("Enter new name: ");
                fgets(buffer, MAX_NAME_LEN, stdin);
                stripNewline(buffer);
                if (strlen(buffer) > 0 && !isNameExists(buffer, index)) {
                    strcpy(addressBook[index].name, buffer);
                    printf("Name updated successfully!\n");
                } else {
                    printf("Invalid or duplicate name.\n");
                }
                break;
            case 2:
                printf("Enter new phone number: ");
                fgets(buffer, MAX_PHONE_LEN, stdin);
                stripNewline(buffer);
                if (isNumeric(buffer) && !isPhoneExists(buffer, index)) {
                    strcpy(addressBook[index].phone, buffer);
                    printf("Phone number updated successfully!\n");
                } else {
                    printf("Invalid or duplicate phone number.\n");
                }
                break;
            case 3:
                printf("Enter new email address: ");
                fgets(buffer, MAX_EMAIL_LEN, stdin);
                stripNewline(buffer);
                if (isValidEmail(buffer) && !isEmailExists(buffer, index)) {
                    strcpy(addressBook[index].email, buffer);
                    printf("Email address updated successfully!\n");
                } else {
                    printf("Invalid or duplicate email address.\n");
                }
                break;
            case 4:
                return;
            default:
                printf("Invalid selection.\n");
        }
    }
}

// 3. SEARCH CONTACT MENU -> SEARCH COMPONENT
void searchContactMenu() {
    printf("\n--- SEARCH CONTACT MENU ---\n");
    searchComponent();
}

// 4. DELETE CONTACT MENU -> SEARCH COMPONENT -> CONFIRMATION DELETE ? Y/N
void deleteContactMenu() {
    printf("\n--- DELETE CONTACT MENU ---\n");
    int index = searchComponent();
    if (index == -1) return;

    char confirm;
    printf("Confirmation: Are you sure you want to delete '%s'? (Y/N): ", addressBook[index].name);
    scanf(" %c", &confirm);
    getchar();

    if (confirm == 'Y' || confirm == 'y') {
        for (int i = index; i < contactCount - 1; i++) {
            addressBook[i] = addressBook[i + 1];
        }
        contactCount--;
        printf("Contact deleted successfully.\n");
    } else {
        printf("Deletion cancelled.\n");
    }
}

// 5. LIST ALL CONTACT MENU
void listAllContactMenu() {
    printf("\n--- LIST ALL CONTACTS ---\n");
    if (contactCount == 0) {
        printf("Address book is empty.\n");
        return;
    }

    printf("----------------------------------------------------------------------\n");
    printf("%-5s | %-25s | %-15s | %-25s\n", "ID", "Name", "Phone", "Email");
    printf("----------------------------------------------------------------------\n");
    for (int i = 0; i < contactCount; i++) {
        printf("%-5d | %-25s | %-15s | %-25s\n", 
               i + 1, addressBook[i].name, addressBook[i].phone, addressBook[i].email);
    }
    printf("----------------------------------------------------------------------\n");
}

// 6. SAVE CHANGES -> CONFIRMATION SAVE ? Y/N
void saveChangesMenu() {
    char confirm;
    printf("\nConfirmation Save? (Y/N): ");
    scanf(" %c", &confirm);
    getchar();

    if (confirm == 'Y' || confirm == 'y') {
        saveToFile();
    } else {
        printf("Save action cancelled.\n");
    }
}

// 7. EXIT FROM APPLICATION -> CONFIRMATION (SAVE & EXIT / DISCARD / CANCEL)
int exitApplicationMenu() {
    int exitChoice;
    printf("\n--- EXIT CONFIRMATION ---\n");
    printf("1. Save & Exit\n");
    printf("2. Discard Changes & Exit\n");
    printf("3. Cancel Exit\n");
    printf("Select option (1-3): ");

    if (scanf("%d", &exitChoice) != 1) {
        getchar();
        return 0; // Cancel
    }
    getchar();

    switch (exitChoice) {
        case 1:
            saveToFile();
            printf("Exiting application... Goodbye!\n");
            exit(0);
        case 2:
            printf("Exiting application without saving... Goodbye!\n");
            exit(0);
        case 3:
            printf("Exit cancelled. Returning to main menu.\n");
            return 0;
        default:
            printf("Invalid selection.\n");
            return 0;
    }
}

// MAIN MENU
int main() {
    loadFromFile();
    int choice;

    while (1) {
        printf("\n=========================================\n");
        printf("             MAIN MENU                   \n");
        printf("=========================================\n");
        printf("1. Add Contact Menu\n");
        printf("2. Edit Contact Menu\n");
        printf("3. Search Contact Menu\n");
        printf("4. Delete Contact Menu\n");
        printf("5. List All Contact Menu\n");
        printf("6. Save Changes\n");
        printf("7. Exit Application\n");
        printf("=========================================\n");
        printf("Enter your choice (1-7): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }
        getchar();

        switch (choice) {
            case 1: addContactMenu(); break;
            case 2: editContactMenu(); break;
            case 3: searchContactMenu(); break;
            case 4: deleteContactMenu(); break;
            case 5: listAllContactMenu(); break;
            case 6: saveChangesMenu(); break;
            case 7: exitApplicationMenu(); break;
            default: printf("Invalid choice! Choose between 1 and 7.\n");
        }
    }
    return 0;
}