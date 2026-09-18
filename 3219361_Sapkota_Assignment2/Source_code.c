#include <stdio.h>      // Standard input/output functions
#include <stdlib.h>     // Memory allocation functions
#include <string.h>     // String handling functions

// Constants for maximum sizes
#define MAX_CATEGORY_LEN 32
#define MAX_SUBJECT_LEN 256
#define MAX_DATE_LEN 16
#define INITIAL_CAPACITY 16

/* =========================
   Email Structure
   Stores information for one email
   ========================= */
typedef struct {
    char sender[MAX_CATEGORY_LEN];      // Sender category
    char subject[MAX_SUBJECT_LEN];      // Email subject
    char date[MAX_DATE_LEN];            // Date in MM-DD-YYYY format
    int category_weight;               // Priority value
    long date_numeric;                // Date converted to YYYYMMDD
} Email;

/* =========================
   Max Heap Structure
   Stores emails in priority order
   ========================= */
typedef struct {
    Email *data;       // Dynamic array of emails
    int size;          // Current number of emails
    int capacity;      // Maximum array size
} MaxHeap;

/* =====================================================
   Convert sender category into priority number
   Higher number = Higher priority
   ===================================================== */
int get_category_weight(const char *category) {
    if (strcmp(category, "Boss") == 0) return 5;
    if (strcmp(category, "Subordinate") == 0) return 4;
    if (strcmp(category, "Peer") == 0) return 3;
    if (strcmp(category, "ImportantPerson") == 0) return 2;
    if (strcmp(category, "OtherPerson") == 0) return 1;
    return 0;   // Unknown category
}

/* =====================================================
   Convert MM-DD-YYYY into YYYYMMDD
   Makes date comparison easier
   ===================================================== */
long parse_date_to_numeric(const char *date_str) {
    int month, day, year;

    if (sscanf(date_str, "%d-%d-%d", &month, &day, &year) == 3) {
        return (long)year * 10000 + (long)month * 100 + (long)day;
    }

    return 0;   // Invalid date
}

/* =====================================================
   Compare two emails
   Return:
   >0 if a has higher priority
   <0 if b has higher priority
   0 if equal
   ===================================================== */
int compare_emails(const Email *a, const Email *b) {

    // First compare sender priority
    if (a->category_weight != b->category_weight) {
        return a->category_weight - b->category_weight;
    }

    // If sender priority is same, compare dates
    if (a->date_numeric > b->date_numeric)
        return 1;

    if (a->date_numeric < b->date_numeric)
        return -1;

    return 0;
}

/* =====================================================
   Create an empty heap
   ===================================================== */
MaxHeap* create_heap() {

    MaxHeap *heap = (MaxHeap *)malloc(sizeof(MaxHeap));

    if (heap == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    heap->capacity = INITIAL_CAPACITY;
    heap->size = 0;

    heap->data = (Email *)malloc(sizeof(Email) * heap->capacity);

    if (heap->data == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    return heap;
}

/* =====================================================
   Swap two email records
   ===================================================== */
void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

/* =====================================================
   Move inserted item upward to maintain heap property
   ===================================================== */
void heapify_up(MaxHeap *heap, int idx) {

    while (idx > 0) {

        int parent = (idx - 1) / 2;

        if (compare_emails(&heap->data[idx],
                           &heap->data[parent]) > 0) {

            swap(&heap->data[idx],
                 &heap->data[parent]);

            idx = parent;
        }
        else {
            break;
        }
    }
}

/* =====================================================
   Move root downward after deletion
   ===================================================== */
void heapify_down(MaxHeap *heap, int idx) {

    while (1) {

        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int largest = idx;

        if (left < heap->size &&
            compare_emails(&heap->data[left],
                           &heap->data[largest]) > 0) {

            largest = left;
        }

        if (right < heap->size &&
            compare_emails(&heap->data[right],
                           &heap->data[largest]) > 0) {

            largest = right;
        }

        if (largest != idx) {

            swap(&heap->data[idx],
                 &heap->data[largest]);

            idx = largest;
        }
        else {
            break;
        }
    }
}

/* =====================================================
   Insert email into heap
   ===================================================== */
void insert_email(MaxHeap *heap, Email email) {

    // Expand array if full
    if (heap->size == heap->capacity) {

        heap->capacity *= 2;

        Email *temp = realloc(heap->data,
                              sizeof(Email) * heap->capacity);

        if (temp == NULL) {
            printf("Memory reallocation failed.\n");
            exit(1);
        }

        heap->data = temp;
    }

    // Insert new email
    heap->data[heap->size] = email;

    // Fix heap
    heapify_up(heap, heap->size);

    heap->size++;
}

/* =====================================================
   Return highest priority email
   ===================================================== */
Email* peek_email(MaxHeap *heap) {

    if (heap->size == 0)
        return NULL;

    return &heap->data[0];
}

/* =====================================================
   Remove highest priority email
   ===================================================== */
void pop_email(MaxHeap *heap) {

    if (heap->size == 0)
        return;

    heap->data[0] = heap->data[heap->size - 1];

    heap->size--;

    if (heap->size > 0) {
        heapify_down(heap, 0);
    }
}

/* =====================================================
   Remove spaces and newlines from end of string
   ===================================================== */
void trim_whitespace(char *str) {

    int len = strlen(str);

    while (len > 0 &&
          (str[len - 1] == ' ' ||
           str[len - 1] == '\t' ||
           str[len - 1] == '\n' ||
           str[len - 1] == '\r')) {

        str[--len] = '\0';
    }
}

/* =====================================================
   Main Function
   ===================================================== */
int main(int argc, char *argv[]) {

    FILE *file = stdin;

    // Open input file if provided
    if (argc > 1) {

        file = fopen(argv[1], "r");

        if (!file) {
            perror("Error opening file");
            return 1;
        }
    }

    MaxHeap *heap = create_heap();
    char line[512];

    // Read file line by line
    while (fgets(line, sizeof(line), file)) {

        trim_whitespace(line);

        if (strlen(line) == 0)
            continue;

        // EMAIL command
        if (strncmp(line, "EMAIL ", 6) == 0) {

            char *content = line + 6;

            char *sender = strtok(content, ",");
            char *subject = strtok(NULL, ",");
            char *date = strtok(NULL, ",");

            if (sender && subject && date) {

                Email email;

                strncpy(email.sender, sender,
                        sizeof(email.sender) - 1);

                email.sender[sizeof(email.sender) - 1] = '\0';

                strncpy(email.subject, subject,
                        sizeof(email.subject) - 1);

                email.subject[sizeof(email.subject) - 1] = '\0';

                strncpy(email.date, date,
                        sizeof(email.date) - 1);

                email.date[sizeof(email.date) - 1] = '\0';

                email.category_weight =
                    get_category_weight(email.sender);

                email.date_numeric =
                    parse_date_to_numeric(email.date);

                insert_email(heap, email);
            }
        }

        // NEXT command
        else if (strcmp(line, "NEXT") == 0) {

            Email *next = peek_email(heap);

            if (next) {
                printf("Next email:\n");
                printf("Sender: %s\n", next->sender);
                printf("Subject: %s\n", next->subject);
                printf("Date: %s\n", next->date);
            }
            else {
                printf("No emails available.\n");
            }
        }

        // READ command
        else if (strcmp(line, "READ") == 0) {
            pop_email(heap);
        }

        // COUNT command
        else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n",
                   heap->size);
        }
    }

    // Close file
    if (file != stdin) {
        fclose(file);
    }

    // Free memory
    free(heap->data);
    free(heap);

    return 0;
}
