# AI-Based Dynamic Memory Allocation Simulator

**Course**: 2/4 B.Tech CSE — Data Structures and Algorithms  
**Academic Year**: 2026–27  
**Topic #19**: AI-Based Memory Allocation Simulator  
**Implementation**: C (Core DSA) & Python (AI/ML Extension)  

---

## 1. Project Overview & Objectives

Dynamic memory allocation is a core responsibility of computer operating systems. When processes are loaded and terminated, memory becomes fragmented into isolated free holes.

This project implements an **academic dynamic memory allocation simulator in C**, modeling physical RAM partitions using a **Doubly Linked List** with dynamic block splitting and $\mathcal{O}(1)$ free-hole coalescing.

### Core Features:
1. **Three Primary Allocation Algorithms**:
   * **First Fit**: Scans from head and allocates into the first hole large enough ($\mathcal{O}(1)$ average lookup).
   * **Best Fit**: Finds the closest matching partition to minimize immediate leftover waste.
   * **Worst Fit**: Finds the largest available partition, leaving behind large usable residual holes.
2. **Dynamic Block Splitting**: When a process is smaller than a block, it dynamically splits the partition and inserts a new free hole node in $\mathcal{O}(1)$ time.
3. **$\mathcal{O}(1)$ Free-Hole Coalescing**: When a process finishes, it checks both left (`prev`) and right (`next`) neighbors and merges them instantaneously without scanning the list.
4. **Side-by-Side Comparison**: Compares all 3 strategies against the same workload, measuring allocations, rejections, and search steps.
5. **AI Strategy Recommender (`ai_recommender.py`)**: Analyzes workload parameters (total memory, process sizes, variance, memory pressure) to recommend the optimal allocation strategy.

---

## 2. Project Files

| File | Purpose |
| :--- | :--- |
| **`memory_simulator.c`** | Complete C source code containing the Doubly Linked List, allocation algorithms, deallocation/coalescing, and interactive menu. |
| **`memory_simulator.exe`** | Pre-compiled executable for Windows (MinGW GCC). |
| **`ai_recommender.py`** | Python AI module that analyzes workload distributions and recommends the best strategy. |
| **`README.md`** | Complete technical report, complexity tables, and viva defense guide. |

---

## 3. Data Structure Rationale: Why Doubly Linked List?

```
[ Block 1: Free (100 KB) ] <== prev / next ==> [ Block 2: P1 (200 KB) ] <== prev / next ==> [ Block 3: Free (150 KB) ]
```

1. **Physical Contiguity Modeling**: Physical memory blocks are physically adjacent. Each block naturally has a physical left neighbor (`prev`) and right neighbor (`next`).
2. **$\mathcal{O}(1)$ Free-Hole Coalescing**: When process $P_1$ in Block 2 terminates:
   - It checks `block->next`: if free, merges right in $\mathcal{O}(1)$ time.
   - It checks `block->prev`: if free, merges left in $\mathcal{O}(1)$ time.
   - Total coalescing time: **$\mathcal{O}(1)$ constant time**.
3. **Flaw of Singly Linked Lists**: A singly linked list only points forward (`next`). Merging with the left neighbor requires an $\mathcal{O}(n)$ traversal from the head of the list to find the predecessor.
4. **Flaw of Static Arrays**: Arrays have fixed capacity limits, and splitting or merging blocks requires shifting all subsequent elements ($\mathcal{O}(n)$ copying overhead).

---

## 4. Time & Space Complexity Analysis

| Operation | First Fit | Best Fit | Worst Fit |
| :--- | :---: | :---: | :---: |
| **Search Time (Worst Case)** | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ |
| **Search Time (Best Case)** | $\mathcal{O}(1)$ | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ |
| **Search Time (Average)** | Fast ($\approx n/2$) | Slower (all $n$) | Slower (all $n$) |
| **Block Splitting Time** | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| **Free-Hole Coalescing Time** | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| **Auxiliary Space Complexity** | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |

*where $n$ is the number of active memory blocks.*

---

## 5. Team Roles (Suggested for 4 Members)

* **Student 1 (Architecture & Data Structure)**: Explains the `struct Block` (Doubly Linked List), memory pool initialization, and pointer updates.
* **Student 2 (Allocation Algorithms)**: Explains the implementation and search logic of `firstFit()`, `bestFit()`, and `worstFit()`.
* **Student 3 (Deallocation & Coalescing)**: Demonstrates process termination, pointer splicing during `coalesce()`, and fragmentation reduction.
* **Student 4 (AI Extension & Performance Analysis)**: Explains `ai_recommender.py`, workload features (memory pressure, variance), and algorithm trade-offs.

---

## 6. How to Compile & Run

### 1. Compile the C Program:
```cmd
gcc memory_simulator.c -o memory_simulator.exe
```

### 2. Run the C Simulator:
```cmd
.\memory_simulator.exe
```

### 3. Run the AI Recommender:
```cmd
python ai_recommender.py
```

---

## 7. Common Viva Questions & Answers

1. **Q: Why is Doubly Linked List better than an Array for dynamic memory?**  
   *Answer*: In an array, splitting a block or merging adjacent free blocks requires shifting array elements ($\mathcal{O}(n)$). In a Doubly Linked List, splitting and merging with both neighbors takes $\mathcal{O}(1)$ time using pointers.

2. **Q: What is the main drawback of Best Fit?**  
   *Answer*: Best Fit creates tiny leftover holes (fragments) that are too small to fit any other process, increasing external fragmentation.

3. **Q: Why does Worst Fit sometimes perform better than Best Fit?**  
   *Answer*: Worst Fit allocates from the largest available block, so the leftover piece is still relatively large and can easily accommodate future process requests.

4. **Q: How does the AI recommender choose a strategy?**  
   *Answer*: It evaluates memory pressure ($\text{requested memory} / \text{total memory}$) and process size variance. Under tight memory pressure, it recommends Best Fit to pack tightly; under low pressure or high variance, it recommends Worst Fit or First Fit.
