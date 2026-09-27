#include <stdio.h>
#include <stdlib.h>

#define MAX 20

// Doubly Linked List Node to represent a Memory Block
struct Block {
    int id;
    int size;
    struct Block *prev;
    struct Block *next;
};

typedef struct Block Block;

int memory[MAX];
int process[MAX];
int numBlocks = 0;
int numProcesses = 0;

// Function to create a new block node
Block* createNode(int id, int size) {
    Block *newNode = (Block*)malloc(sizeof(Block));
    newNode->id = id;
    newNode->size = size;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize the Doubly Linked List from the memory array
Block* createList(int arr[], int n) {
    if (n <= 0) return NULL;
    
    Block *head = createNode(1, arr[0]);
    Block *curr = head;

    for (int i = 1; i < n; i++) {
        Block *temp = createNode(i + 1, arr[i]);
        curr->next = temp;
        temp->prev = curr;
        curr = temp;
    }
    return head;
}

// Function to free the linked list
void freeList(Block *head) {
    Block *curr = head;
    while (curr != NULL) {
        Block *temp = curr;
        curr = curr->next;
        free(temp);
    }
}

// Function to input memory block sizes
void inputMemory() {
    printf("\nEnter number of Memory Blocks: ");
    scanf("%d", &numBlocks);
    for (int i = 0; i < numBlocks; i++) {
        printf("Enter size of Block %d (KB): ", i + 1);
        scanf("%d", &memory[i]);
    }
}

// Function to input process sizes
void inputProcess() {
    printf("\nEnter number of Processes: ");
    scanf("%d", &numProcesses);
    for (int i = 0; i < numProcesses; i++) {
        printf("Enter size of Process %d (KB): ", i + 1);
        scanf("%d", &process[i]);
    }
}

// Function to display current blocks in the linked list
void displayBlocks(Block *head) {
    printf("\n--- Current Memory Blocks in Doubly Linked List ---\n");
    Block *curr = head;
    while (curr != NULL) {
        printf("[Block %d: %d KB] <-> ", curr->id, curr->size);
        curr = curr->next;
    }
    printf("NULL\n");
}

// First Fit Algorithm
void firstFit() {
    Block *head = createList(memory, numBlocks);
    int allocated = 0;
    int totalRemaining = 0;

    printf("\n================= FIRST FIT ALLOCATION =================\n");
    printf("%-12s %-15s %-12s %-15s\n", "Process", "Process Size", "Block No", "Remaining Size");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < numProcesses; i++) {
        Block *curr = head;
        int placed = 0;

        while (curr != NULL) {
            if (curr->size >= process[i]) {
                curr->size = curr->size - process[i];
                printf("Process %-4d %-15d Block %-6d %d KB\n",
                       i + 1, process[i], curr->id, curr->size);
                placed = 1;
                allocated++;
                break;
            }
            curr = curr->next;
        }

        if (!placed) {
            printf("Process %-4d %-15d %-12s %-15s\n",
                   i + 1, process[i], "Not Allocated", "--");
        }
    }

    // Calculate remaining free memory in all blocks
    Block *curr = head;
    while (curr != NULL) {
        totalRemaining += curr->size;
        curr = curr->next;
    }

    printf("--------------------------------------------------------\n");
    printf("Successfully Allocated: %d / %d processes\n", allocated, numProcesses);
    printf("Total Free Memory Left: %d KB\n", totalRemaining);
    displayBlocks(head);
    freeList(head);
}

// Best Fit Algorithm
void bestFit() {
    Block *head = createList(memory, numBlocks);
    int allocated = 0;
    int totalRemaining = 0;

    printf("\n================== BEST FIT ALLOCATION ==================\n");
    printf("%-12s %-15s %-12s %-15s\n", "Process", "Process Size", "Block No", "Remaining Size");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < numProcesses; i++) {
        Block *curr = head;
        Block *bestNode = NULL;
        int minFragment = 999999;

        while (curr != NULL) {
            if (curr->size >= process[i]) {
                int fragment = curr->size - process[i];
                if (fragment < minFragment) {
                    minFragment = fragment;
                    bestNode = curr;
                }
            }
            curr = curr->next;
        }

        if (bestNode != NULL) {
            bestNode->size = bestNode->size - process[i];
            printf("Process %-4d %-15d Block %-6d %d KB\n",
                   i + 1, process[i], bestNode->id, bestNode->size);
            allocated++;
        } else {
            printf("Process %-4d %-15d %-12s %-15s\n",
                   i + 1, process[i], "Not Allocated", "--");
        }
    }

    // Calculate remaining free memory
    Block *curr = head;
    while (curr != NULL) {
        totalRemaining += curr->size;
        curr = curr->next;
    }

    printf("--------------------------------------------------------\n");
    printf("Successfully Allocated: %d / %d processes\n", allocated, numProcesses);
    printf("Total Free Memory Left: %d KB\n", totalRemaining);
    displayBlocks(head);
    freeList(head);
}

// Worst Fit Algorithm
void worstFit() {
    Block *head = createList(memory, numBlocks);
    int allocated = 0;
    int totalRemaining = 0;

    printf("\n================= WORST FIT ALLOCATION =================\n");
    printf("%-12s %-15s %-12s %-15s\n", "Process", "Process Size", "Block No", "Remaining Size");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < numProcesses; i++) {
        Block *curr = head;
        Block *worstNode = NULL;
        int maxFragment = -1;

        while (curr != NULL) {
            if (curr->size >= process[i]) {
                int fragment = curr->size - process[i];
                if (fragment > maxFragment) {
                    maxFragment = fragment;
                    worstNode = curr;
                }
            }
            curr = curr->next;
        }

        if (worstNode != NULL) {
            worstNode->size = worstNode->size - process[i];
            printf("Process %-4d %-15d Block %-6d %d KB\n",
                   i + 1, process[i], worstNode->id, worstNode->size);
            allocated++;
        } else {
            printf("Process %-4d %-15d %-12s %-15s\n",
                   i + 1, process[i], "Not Allocated", "--");
        }
    }

    // Calculate remaining free memory
    Block *curr = head;
    while (curr != NULL) {
        totalRemaining += curr->size;
        curr = curr->next;
    }

    printf("--------------------------------------------------------\n");
    printf("Successfully Allocated: %d / %d processes\n", allocated, numProcesses);
    printf("Total Free Memory Left: %d KB\n", totalRemaining);
    displayBlocks(head);
    freeList(head);
}

int main() {
    int choice;

    // Default sample values so you can test quickly without typing each time
    numBlocks = 5;
    memory[0] = 100; memory[1] = 500; memory[2] = 200; memory[3] = 300; memory[4] = 600;

    numProcesses = 4;
    process[0] = 212; process[1] = 417; process[2] = 112; process[3] = 426;

    printf("========================================================\n");
    printf("        MEMORY ALLOCATION SIMULATOR USING DOUBLY LINKED LIST\n");
    printf("========================================================\n");
    printf("Loaded default inputs: 5 Blocks (100, 500, 200, 300, 600 KB)\n");
    printf("                       4 Processes (212, 417, 112, 426 KB)\n");

    while (1) {
        printf("\n------------- MENU -------------\n");
        printf("1. Enter New Memory Blocks and Processes\n");
        printf("2. Run First Fit\n");
        printf("3. Run Best Fit\n");
        printf("4. Run Worst Fit\n");
        printf("5. Display Initial Memory Blocks\n");
        printf("6. Exit\n");
        printf("--------------------------------\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1:
                inputMemory();
                inputProcess();
                break;
            case 2:
                firstFit();
                break;
            case 3:
                bestFit();
                break;
            case 4:
                worstFit();
                break;
            case 5: {
                Block *head = createList(memory, numBlocks);
                displayBlocks(head);
                freeList(head);
                break;
            }
            case 6:
                printf("\nExiting program.\n");
                return 0;
            default:
                printf("\nInvalid choice! Please select 1 to 6.\n");
        }
    }

    return 0;
}
