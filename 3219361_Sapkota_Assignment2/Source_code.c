/*
 * ============================================================
 * Program Name: EECS 348 Assignment 2 - Email Priority Queue
 * ============================================================
 *
 * Description:
 * This C program implements an email priority queue using a
 * Max Heap. Emails are assigned a priority based on the sender
 * category. If two emails have the same sender priority, the
 * newer email is given higher priority.
 *
 * The program reads commands from standard input or from an
 * input file. It supports adding emails, viewing the next
 * highest-priority email, reading/removing an email, and
 * counting the remaining emails.
 *
 * Inputs:
 * - EMAIL commands containing sender category, subject, and date.
 * - NEXT command to view the highest-priority email.
 * - READ command to remove the highest-priority email.
 * - COUNT command to display the number of remaining emails.
 * - An optional input filename can be provided as a command-line
 *   argument.
 *
 * Output:
 * - Information about the next email to be read.
 * - The number of emails remaining in the priority queue.
 * - An error message if an input file cannot be opened.
 *
 * Collaborators:
 * - None
 *
 * Other Sources for Code:
 * - Google Gemini: Used for assistance with code development,
 *   comments, and understanding the implementation.
 * - GitHub Copilot: Used for coding assistance and suggestions.
 *
 * Author: Prabesh Sapkota
 *
 * Creation Date: September 8, 2026
 * Revision Date: September 17, 2026
 *
 * Revisions:
 * - Added detailed prologue comments.
 * - Added comments explaining individual lines and major
 *   sections of the program.
 * - Added memory allocation error checking.
 * - Added an output message when no emails are available.
 *
 * Code Attribution:
 * - The overall program was authored by Prabesh Sapkota.
 * - Gemini and GitHub Copilot were used as AI coding assistants.
 * - No human collaborators contributed code.
 *
 * ============================================================
 */


/* Include the standard input/output library for printf, fopen,
   fclose, fgets, and other input/output functions. */
#include <stdio.h>

/* Include the standard library for malloc, realloc, free,
   and exit. */
#include <stdlib.h>

/* Include the string library for strcmp, strlen, strncpy,
   strncmp, strtok, and other string functions. */
#include <string.h>


/* Define the maximum number of characters allowed for a
   sender category. */
#define MAX_CATEGORY_LEN 32

/* Define the maximum number of characters allowed for an
   email subject. */
#define MAX_SUBJECT_LEN 256

/* Define the maximum number of characters allowed for a date. */
#define MAX_DATE_LEN 16

/* Define the starting capacity of the heap. */
#define INITIAL_CAPACITY 16


/*
 * ------------------------------------------------------------
 * Email Structure
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * This structure stores all information associated with one
 * email.
 */
typedef struct {

    /* Store the sender category such as Boss or Peer. */
    char sender[MAX_CATEGORY_LEN];

    /* Store the subject of the email. */
    char subject[MAX_SUBJECT_LEN];

    /* Store the original date in MM-DD-YYYY format. */
    char date[MAX_DATE_LEN];

    /* Store the numerical priority of the sender category. */
    int category_weight;

    /* Store the date as YYYYMMDD for easy comparison. */
    long date_numeric;

} Email;


/*
 * ------------------------------------------------------------
 * MaxHeap Structure
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * This structure implements the dynamic array used by the
 * Max Heap.
 */
typedef struct {

    /* Pointer to the dynamically allocated email array. */
    Email *data;

    /* Store the current number of emails in the heap. */
    int size;

    /* Store the current maximum capacity of the heap. */
    int capacity;

} MaxHeap;


/*
 * ------------------------------------------------------------
 * get_category_weight
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Assigns a numerical priority to each sender category.
 * A larger number means a higher priority.
 */
int get_category_weight(const char *category) {

    /* Give Boss the highest priority of 5. */
    if (strcmp(category, "Boss") == 0)
        return 5;

    /* Give Subordinate a priority of 4. */
    if (strcmp(category, "Subordinate") == 0)
        return 4;

    /* Give Peer a priority of 3. */
    if (strcmp(category, "Peer") == 0)
        return 3;

    /* Give ImportantPerson a priority of 2. */
    if (strcmp(category, "ImportantPerson") == 0)
        return 2;

    /* Give OtherPerson the lowest recognized priority of 1. */
    if (strcmp(category, "OtherPerson") == 0)
        return 1;

    /* Return 0 if the sender category is not recognized. */
    return 0;
}


/*
 * ------------------------------------------------------------
 * parse_date_to_numeric
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Converts a date from MM-DD-YYYY into YYYYMMDD.
 *
 * Example:
 * 09-17-2026 becomes 20260917.
 *
 * This makes it easier to compare dates numerically.
 */
long parse_date_to_numeric(const char *date_str) {

    /* Declare variables to store the month, day, and year. */
    int month, day, year;

    /* Read the month, day, and year from the date string. */
    if (sscanf(date_str, "%d-%d-%d",
               &month, &day, &year) == 3) {

        /* Convert the date into YYYYMMDD format and return it. */
        return (long)year * 10000 +
               (long)month * 100 +
               (long)day;
    }

    /* Return 0 if the date could not be parsed correctly. */
    return 0;
}


/*
 * ------------------------------------------------------------
 * compare_emails
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Compares two emails to determine which one has higher
 * priority.
 *
 * Returns:
 *   Positive value -> email a has higher priority.
 *   Negative value -> email b has higher priority.
 *   Zero           -> both have equal priority.
 */
int compare_emails(const Email *a, const Email *b) {

    /* Check whether the sender priorities are different. */
    if (a->category_weight != b->category_weight) {

        /* Higher category weight means higher priority. */
        return a->category_weight - b->category_weight;
    }

    /* If categories are equal, compare their dates. */
    if (a->date_numeric > b->date_numeric)

        /* A newer email has higher priority. */
        return 1;

    /* Check whether email b has the newer date. */
    if (a->date_numeric < b->date_numeric)

        /* Return a negative value because b has higher priority. */
        return -1;

    /* Return zero when both emails have the same priority and date. */
    return 0;
}


/*
 * ------------------------------------------------------------
 * create_heap
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Allocates memory and initializes an empty Max Heap.
 */
MaxHeap* create_heap() {

    /* Allocate memory for the MaxHeap structure. */
    MaxHeap *heap = (MaxHeap *)malloc(sizeof(MaxHeap));

    /* Check whether memory allocation was successful. */
    if (heap == NULL) {

        /* Display an error message if allocation failed. */
        printf("Memory allocation failed.\n");

        /* Stop the program because the heap cannot be created. */
        exit(1);
    }

    /* Set the starting capacity of the heap. */
    heap->capacity = INITIAL_CAPACITY;

    /* Start with zero emails in the heap. */
    heap->size = 0;

    /* Allocate memory for the email array. */
    heap->data =
        (Email *)malloc(sizeof(Email) * heap->capacity);

    /* Check whether the email array was allocated successfully. */
    if (heap->data == NULL) {

        /* Display an error message if allocation failed. */
        printf("Memory allocation failed.\n");

        /* Free the previously allocated heap structure. */
        free(heap);

        /* Stop the program because memory is unavailable. */
        exit(1);
    }

    /* Return the newly created heap. */
    return heap;
}


/*
 * ------------------------------------------------------------
 * swap
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Exchanges the positions of two Email structures.
 */
void swap(Email *a, Email *b) {

    /* Create a temporary Email to hold the first email. */
    Email temp = *a;

    /* Copy the second email into the first position. */
    *a = *b;

    /* Copy the original first email into the second position. */
    *b = temp;
}


/*
 * ------------------------------------------------------------
 * heapify_up
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Moves an inserted email upward until the Max Heap property
 * is restored.
 */
void heapify_up(MaxHeap *heap, int idx) {

    /* Continue while the current element is not the root. */
    while (idx > 0) {

        /* Calculate the index of the current element's parent. */
        int parent = (idx - 1) / 2;

        /* Check whether the current email has higher priority
           than its parent. */
        if (compare_emails(&heap->data[idx],
                           &heap->data[parent]) > 0) {

            /* Swap the current email with its parent. */
            swap(&heap->data[idx],
                 &heap->data[parent]);

            /* Continue checking from the parent's position. */
            idx = parent;
        }
        else {

            /* Stop when the heap property has been restored. */
            break;
        }
    }
}


/*
 * ------------------------------------------------------------
 * heapify_down
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Moves an email downward until the Max Heap property is
 * restored.
 */
void heapify_down(MaxHeap *heap, int idx) {

    /* Continue until the correct position is found. */
    while (1) {

        /* Calculate the index of the left child. */
        int left = 2 * idx + 1;

        /* Calculate the index of the right child. */
        int right = 2 * idx + 2;

        /* Assume the current element has the highest priority. */
        int largest = idx;

        /* Check whether the left child has higher priority. */
        if (left < heap->size &&
            compare_emails(&heap->data[left],
                           &heap->data[largest]) > 0) {

            /* Make the left child the highest-priority element. */
            largest = left;
        }

        /* Check whether the right child has higher priority. */
        if (right < heap->size &&
            compare_emails(&heap->data[right],
                           &heap->data[largest]) > 0) {

            /* Make the right child the highest-priority element. */
            largest = right;
        }

        /* Check whether a child has higher priority than the
           current element. */
        if (largest != idx) {

            /* Swap the current element with the larger child. */
            swap(&heap->data[idx],
                 &heap->data[largest]);

            /* Continue from the child's position. */
            idx = largest;
        }
        else {

            /* Stop when the heap property is restored. */
            break;
        }
    }
}


/*
 * ------------------------------------------------------------
 * insert_email
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Adds a new email to the Max Heap. The array is automatically
 * expanded when it becomes full.
 */
void insert_email(MaxHeap *heap, Email email) {

    /* Check whether the heap has reached its capacity. */
    if (heap->size == heap->capacity) {

        /* Double the capacity to make room for more emails. */
        heap->capacity *= 2;

        /* Resize the dynamic email array. */
        Email *new_data =
            (Email *)realloc(heap->data,
                             sizeof(Email) * heap->capacity);

        /* Check whether realloc was successful. */
        if (new_data == NULL) {

            /* Display an error message. */
            printf("Memory allocation failed.\n");

            /* Stop the program because the array cannot grow. */
            exit(1);
        }

        /* Store the newly allocated array address. */
        heap->data = new_data;
    }

    /* Place the new email at the end of the heap. */
    heap->data[heap->size] = email;

    /* Move the new email upward if necessary. */
    heapify_up(heap, heap->size);

    /* Increase the number of emails stored in the heap. */
    heap->size++;
}


/*
 * ------------------------------------------------------------
 * peek_email
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Returns the highest-priority email without removing it.
 */
Email* peek_email(MaxHeap *heap) {

    /* Return NULL if the heap contains no emails. */
    if (heap->size == 0)
        return NULL;

    /* The highest-priority email is always at index 0. */
    return &heap->data[0];
}


/*
 * ------------------------------------------------------------
 * pop_email
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Removes the highest-priority email from the heap.
 */
void pop_email(MaxHeap *heap) {

    /* Do nothing if the heap is empty. */
    if (heap->size == 0)
        return;

    /* Move the last email to the root position. */
    heap->data[0] = heap->data[heap->size - 1];

    /* Decrease the number of emails in the heap. */
    heap->size--;

    /* Restore the heap property if emails remain. */
    if (heap->size > 0) {
        heapify_down(heap, 0);
    }
}


/*
 * ------------------------------------------------------------
 * trim_whitespace
 * ------------------------------------------------------------
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Removes spaces, tabs, and newline characters from the end
 * of an input string.
 */
void trim_whitespace(char *str) {

    /* Calculate the current length of the string. */
    int len = strlen(str);

    /* Continue while the final character is whitespace. */
    while (len > 0 &&
           (str[len - 1] == ' ' ||
            str[len - 1] == '\t' ||
            str[len - 1] == '\n' ||
            str[len - 1] == '\r')) {

        /* Replace the whitespace character with a null terminator. */
        str[--len] = '\0';
    }
}


/*
 * ============================================================
 * main
 * ============================================================
 * Authored by Prabesh Sapkota with coding assistance from
 * Gemini and GitHub Copilot.
 *
 * Main function that reads and processes commands.
 */
int main(int argc, char *argv[]) {

    /* Use standard input by default. */
    FILE *file = stdin;

    /* Check whether the user provided an input filename. */
    if (argc > 1) {

        /* Open the specified input file for reading. */
        file = fopen(argv[1], "r");

        /* Check whether the file was opened successfully. */
        if (!file) {

            /* Display the operating system's file error message. */
            perror("Error opening input file");

            /* Return 1 to indicate an error. */
            return 1;
        }
    }

    /* Create an empty Max Heap for the emails. */
    MaxHeap *heap = create_heap();

    /* Create a buffer for reading input lines. */
    char line[512];

    /* Read input one line at a time until the file ends. */
    while (fgets(line, sizeof(line), file)) {

        /* Remove unnecessary whitespace from the input line. */
        trim_whitespace(line);

        /* Ignore completely empty lines. */
        if (strlen(line) == 0)
            continue;


        /*
         * ----------------------------------------------------
         * EMAIL command
         * ----------------------------------------------------
         */

        /* Check whether the current command starts with EMAIL. */
        if (strncmp(line, "EMAIL ", 6) == 0) {

            /* Skip the word "EMAIL " to access the email data. */
            char *content = line + 6;

            /* Extract the sender category before the first comma. */
            char *sender = strtok(content, ",");

            /* Extract the subject before the next comma. */
            char *subject = strtok(NULL, ",");

            /* Extract the date after the second comma. */
            char *date = strtok(NULL, ",");

            /* Make sure all three email fields were found. */
            if (sender && subject && date) {

                /* Create a new Email structure. */
                Email email;

                /* Copy the sender into the email structure. */
                strncpy(email.sender,
                        sender,
                        sizeof(email.sender) - 1);

                /* Add the null terminator to the sender string. */
                email.sender[sizeof(email.sender) - 1] = '\0';

                /* Copy the subject into the email structure. */
                strncpy(email.subject,
                        subject,
                        sizeof(email.subject) - 1);

                /* Add the null terminator to the subject string. */
                email.subject[sizeof(email.subject) - 1] = '\0';

                /* Copy the date into the email structure. */
                strncpy(email.date,
                        date,
                        sizeof(email.date) - 1);

                /* Add the null terminator to the date string. */
                email.date[sizeof(email.date) - 1] = '\0';

                /* Determine the priority of the sender. */
                email.category_weight =
                    get_category_weight(email.sender);

                /* Convert the date into a number for comparison. */
                email.date_numeric =
                    parse_date_to_numeric(email.date);

                /* Insert the completed email into the Max Heap. */
                insert_email(heap, email);
            }
        }


        /*
         * ----------------------------------------------------
         * NEXT command
         * ----------------------------------------------------
         */

        /* Check whether the user wants to view the next email. */
        else if (strcmp(line, "NEXT") == 0) {

            /* Get the highest-priority email without removing it. */
            Email *next = peek_email(heap);

            /* Check whether an email was found. */
            if (next) {

                /* Display a heading for the next email. */
                printf("Next email:\n");

                /* Display the sender category. */
                printf("Sender: %s\n", next->sender);

                /* Display the email subject. */
                printf("Subject: %s\n", next->subject);

                /* Display the email date. */
                printf("Date: %s\n", next->date);
            }
            else {

                /* Tell the user that there are no emails. */
                printf("No emails available.\n");
            }
        }


        /*
         * ----------------------------------------------------
         * READ command
         * ----------------------------------------------------
         */

        /* Check whether the user wants to read/remove an email. */
        else if (strcmp(line, "READ") == 0) {

            /* Remove the highest-priority email from the heap. */
            pop_email(heap);
        }


        /*
         * ----------------------------------------------------
         * COUNT command
         * ----------------------------------------------------
         */

        /* Check whether the user wants the number of emails. */
        else if (strcmp(line, "COUNT") == 0) {

            /* Display the number of emails currently in the heap. */
            printf("There are %d emails to read.\n",
                   heap->size);
        }
    }


    /*
     * --------------------------------------------------------
     * Cleanup
     * --------------------------------------------------------
     */

    /* Close the input file if one was opened by the program. */
    if (file != stdin) {
        fclose(file);
    }

    /* Release the dynamically allocated email array. */
    free(heap->data);

    /* Release the Max Heap structure itself. */
    free(heap);

    /* Return 0 to indicate that the program completed normally. */
    return 0;
}
