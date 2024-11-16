#include <stdbool.h>
#include <stdlib.h>

struct SelectionSortState {
    int* arr;
    int len;
    int iter_idx;  // index of the outer loop
    int inner_idx; // index of the inner loop
    int min_idx;   // index of the minimum element found after index iter_idx
    bool done;
};

struct InsertSortState {
    int* arr;
    int len;
    int iter_idx;   // index of the outer loop
    int insert_idx; // index of the inner loop
    int value;      // value to be inserted
    bool done;
};

struct MergeSortState {
    int* arr;
    int len;
    int* stack;
    int stack_ptr;
    int merge_left_idx;
    int merge_right_idx;
    int* subarr_left;
    int* subarr_right;
    int subarr_left_idx;
    int subarr_right_idx;
    int merge_iter;
    bool done;
    bool merge_done;
};

void selection_sort_init(struct SelectionSortState* state, int arr[], int n);
void insert_sort_init(struct InsertSortState* state, int* arr, int n);
void merge_sort_init(struct MergeSortState* state, int* arr, int len);

void selection_sort_step(struct SelectionSortState* s);
void insert_sort_step(struct InsertSortState* s);
void merge_sort_step(struct MergeSortState* state);
