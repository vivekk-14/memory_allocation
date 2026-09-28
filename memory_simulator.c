#include <stdio.h>
#include <stdlib.h>

// max 20 blocks and processes pettukovalani define chesamu
#define MAX 20

// memory block ni represent cheyyadaniki oka node structure create chesamu
// doubly linked list use chesamu because prev and next rendu directions lo move avvadam easy avutundi
struct Block {
    int id;            // block ki oka number isthamu - Block 1, Block 2 ani
    int size;          // aa block lo evvaro KB space undi ani
    struct Block *prev; // left side block ki pointer - previous block
    struct Block *next; // right side block ki pointer - next block
};

// struct Block ani raayadam instead of Block ani direct ga use cheyyadam kosam typedef chesamu
typedef struct Block Block;

// memory block sizes store cheyyadaniki array
int memory[MAX];
// process sizes store cheyyadaniki array
int process[MAX];
int numBlocks = 0;    // enni blocks unnayo count
int numProcesses = 0; // enni processes unnayo count

// kotta block node create cheyyadaniki ee function
Block* createNode(int id, int size) {
    // heap lo oka Block ki memory allocate chesamu - malloc use chesamu
    Block *newNode = (Block*)malloc(sizeof(Block));
    newNode->id = id;      // block number set chesamu
    newNode->size = size;  // block size set chesamu
    newNode->prev = NULL;  // ippudu left side emi ledu so NULL
    newNode->next = NULL;  // ippudu right side emi ledu so NULL
    return newNode;        // create chesina node return chesamu
}

// array lo unna block sizes ni doubly linked list ga create cheyyadaniki ee function
Block* createList(int arr[], int n) {
    // blocks emi lekapothe NULL return chesamu
    if (n <= 0) return NULL;

    // first block create chesamu - head avutundi
    Block *head = createNode(1, arr[0]);
    Block *curr = head; // curr pointer head nunchi start avutundi

    // migilina blocks anni create chesi link chesamu
    for (int i = 1; i < n; i++) {
        Block *temp = createNode(i + 1, arr[i]); // kotta node create chesamu
        curr->next = temp;  // current node right side ki kotta node connect chesamu
        temp->prev = curr;  // kotta node left side ki current node connect chesamu - doubly linked list idi
        curr = temp;        // curr pointer ni forward move chesamu
    }
    return head; // list start ayye head node return chesamu
}

// linked list lo unna anni nodes ki memory free cheyyadaniki ee function
// malloc chesina prathi dani ki free cheyyadam chala important - memory leak vastundi lekapothe
void freeList(Block *head) {
    Block *curr = head;
    while (curr != NULL) {
        Block *temp = curr;   // current node save chesamu
        curr = curr->next;    // free cheyyamundu next ki move avvamu - lekapothe lose avutamu
        free(temp);           // ippudu safe ga free cheyyavacchu
    }
}

// user dggara enni blocks, enta size ani input teesukodaniki
void inputMemory() {
    printf("\nEnter number of Memory Blocks: ");
    scanf("%d", &numBlocks);
    for (int i = 0; i < numBlocks; i++) {
        printf("Enter size of Block %d (KB): ", i + 1);
        scanf("%d", &memory[i]);
    }
}

// user dggara enni processes, enta size ani input teesukodaniki
void inputProcess() {
    printf("\nEnter number of Processes: ");
    scanf("%d", &numProcesses);
    for (int i = 0; i < numProcesses; i++) {
        printf("Enter size of Process %d (KB): ", i + 1);
        scanf("%d", &process[i]);
    }
}

// linked list lo unna anni blocks display cheyyadaniki
void displayBlocks(Block *head) {
    printf("\n--- Current Memory Blocks in Doubly Linked List ---\n");
    Block *curr = head;
    // list lo unna prathi block print chesamu - <-> doubly linked list show avutundi
    while (curr != NULL) {
        printf("[Block %d: %d KB] <-> ", curr->id, curr->size);
        curr = curr->next;
    }
    printf("NULL\n"); // list end lo NULL untundi
}

// First Fit Algorithm - list start nunchi scan chesi first fit ayye block ki allocate chesamu
void firstFit() {
    // fresh ga list create chesamu - prathi algorithm ki fresh start isthamu
    Block *head = createList(memory, numBlocks);
    int allocated = 0;      // evvaro processes allocate ayyayo count cheyyadaniki
    int totalRemaining = 0; // anni blocks lo total free memory calculate cheyyadaniki

    printf("\n================= FIRST FIT ALLOCATION =================\n");
    printf("%-12s %-15s %-12s %-15s\n", "Process", "Process Size", "Block No", "Remaining Size");
    printf("--------------------------------------------------------\n");

    // prathi process ki suitable block find cheyyadaniki loop
    for (int i = 0; i < numProcesses; i++) {
        Block *curr = head; // prathi process ki head nunchi scan start chesamu
        int placed = 0;     // ee process allocate ayyinda leeda ani track cheyyadaniki

        // list lo blocks scan chesamu - first fit block dorikevaraku
        while (curr != NULL) {
            // block size process size kante ekkuva unte allocate cheyyavacchu
            if (curr->size >= process[i]) {
                curr->size = curr->size - process[i]; // block nunchi process size teesivestamu
                printf("Process %-4d %-15d Block %-6d %d KB\n",
                       i + 1, process[i], curr->id, curr->size);
                placed = 1;    // allocate ayyindi ani mark chesamu
                allocated++;   // allocated count penchamu
                break;         // FIRST FIT - first block dorikithe stop - idi first fit ki key point
            }
            curr = curr->next; // ee block fit avvatle so next block ki move avvamu
        }

        // loop complete ayyindi kani place kaatle - no block fit ayyindi ani print chesamu
        if (!placed) {
            printf("Process %-4d %-15d %-12s %-15s\n",
                   i + 1, process[i], "Not Allocated", "--");
        }
    }

    // anni blocks lo remaining free memory add chesamu
    Block *curr = head;
    while (curr != NULL) {
        totalRemaining += curr->size;
        curr = curr->next;
    }

    printf("--------------------------------------------------------\n");
    printf("Successfully Allocated: %d / %d processes\n", allocated, numProcesses);
    printf("Total Free Memory Left: %d KB\n", totalRemaining);
    displayBlocks(head);  // final block state show chesamu
    freeList(head);       // memory free chesamu - important step
}

// Best Fit Algorithm - anni blocks scan chesi smallest leftover isthe block ki allocate chesamu
void bestFit() {
    Block *head = createList(memory, numBlocks);
    int allocated = 0;
    int totalRemaining = 0;

    printf("\n================== BEST FIT ALLOCATION ==================\n");
    printf("%-12s %-15s %-12s %-15s\n", "Process", "Process Size", "Block No", "Remaining Size");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < numProcesses; i++) {
        Block *curr = head;
        Block *bestNode = NULL;   // best block ni save cheyyadaniki - initially NULL
        int minFragment = 999999; // minimum leftover track cheyyadaniki - chala pedda number tho start chesamu

        // BEST FIT lo anni blocks scan cheyyaali - first fit laaga break cheyyamu
        while (curr != NULL) {
            if (curr->size >= process[i]) {
                // ee block use chesthe evvaro KB waste avutundi ani calculate chesamu
                int fragment = curr->size - process[i];
                // idi ippativaaraku chusina best fit kante better unte update chesamu
                if (fragment < minFragment) {
                    minFragment = fragment;
                    bestNode = curr; // best block idi ani save chesamu
                }
            }
            curr = curr->next; // break ledu - anni blocks scan cheyyaali
        }

        // best block dorikithe allocate chesamu
        if (bestNode != NULL) {
            bestNode->size = bestNode->size - process[i];
            printf("Process %-4d %-15d Block %-6d %d KB\n",
                   i + 1, process[i], bestNode->id, bestNode->size);
            allocated++;
        } else {
            // emi fit avvatle
            printf("Process %-4d %-15d %-12s %-15s\n",
                   i + 1, process[i], "Not Allocated", "--");
        }
    }

    // remaining memory calculate chesamu
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

// Worst Fit Algorithm - anni blocks scan chesi biggest leftover isthe block ki allocate chesamu
// idea enti ante - peddha hole choose chesthe leftover kooda peddha ga untundi future use ki pani vastundi
void worstFit() {
    Block *head = createList(memory, numBlocks);
    int allocated = 0;
    int totalRemaining = 0;

    printf("\n================= WORST FIT ALLOCATION =================\n");
    printf("%-12s %-15s %-12s %-15s\n", "Process", "Process Size", "Block No", "Remaining Size");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < numProcesses; i++) {
        Block *curr = head;
        Block *worstNode = NULL; // worst (biggest) block save cheyyadaniki
        int maxFragment = -1;   // maximum leftover track cheyyadaniki - -1 tho start chesamu

        // anni blocks scan chesamu - biggest hole unna block find cheyyadaniki
        while (curr != NULL) {
            if (curr->size >= process[i]) {
                int fragment = curr->size - process[i];
                // best fit ki opposite - ikkada biggest fragment kosam chusthamu
                if (fragment > maxFragment) {
                    maxFragment = fragment;
                    worstNode = curr; // biggest hole unna block save chesamu
                }
            }
            curr = curr->next;
        }

        // worst (biggest) block dorikithe allocate chesamu
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

    // remaining memory calculate chesamu
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

    // default values load chesam
    numBlocks = 5;
    memory[0] = 100; memory[1] = 500; memory[2] = 200; memory[3] = 300; memory[4] = 600;

    numProcesses = 4;
    process[0] = 212; process[1] = 417; process[2] = 112; process[3] = 426;

    printf("========================================================\n");
    printf("        MEMORY ALLOCATION SIMULATOR\n");
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
                // current memory blocks display chesamu - DLL state show avutundi
                Block *head = createList(memory, numBlocks);
                displayBlocks(head);
                freeList(head); // display chesaaka free cheyyaali
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
