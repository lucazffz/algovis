#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int* arr, int left_idx, int right_idx) {
    int pivot_idx = left_idx;

    int i = left_idx + 1; // first element after pivot
    int j = right_idx;

    // pivot is first element so no initial swap neccecary
    while (true) {
        // could swap if equals pivot but would be unneccecary work
        while (arr[i] <= arr[pivot_idx] && i < right_idx) {
            i++;
        }

        while (arr[j] >= arr[pivot_idx] && j > left_idx) {
            j--;
        }

        // need check here because should not swap if i >= j which occurs
        // on the last iteration
        if (i < j) {
            swap(&arr[i], &arr[j]);
        } else {
            // place pivot at right index
            swap(&arr[left_idx], &arr[j]);
            return j;
        }
    }
}

void quick_sort_recursive(int* arr, int left_idx, int right_idx) {
    if (left_idx >= right_idx) {
        return;
    }

    int pivot_idx = partition(arr, left_idx, right_idx);

    // when we return if the call stask the array will already be sorted
    // know that pivot is already at the correct index
    quick_sort_recursive(arr, left_idx, pivot_idx - 1);
    quick_sort_recursive(arr, pivot_idx + 1, right_idx);
}

struct QuickSortState {
    int* arr;
    int len;
    int left_idx;
    int right_idx;
    int stack[1000];
    int stack_ptr;
    int pivot_idx;
    int i;
    int j;
    bool done;
    bool partition_done;
    bool first_iter;
};

void quick_sort_init(struct QuickSortState* state, int* arr, int len) {
    state->arr = arr;
    state->len = len;
    state->left_idx = 0;
    state->right_idx = len - 1;
    state->stack_ptr = 0;
    state->stack[state->stack_ptr++] = 0;
    state->stack[state->stack_ptr++] = len - 1;
    state->pivot_idx = 0;
    state->i = state->left_idx + 1;
    state->j = state->right_idx;
    state->done = false;
    state->partition_done = false;
    state->first_iter = true;
}

void partition_step(struct QuickSortState* state) {
    int pivot_idx = state->pivot_idx;

    int i = state->i; // first element after pivot
    int j = state->j;

    // pivot is first element so no initial swap neccecary
    // could swap if equals pivot but would be unneccecary work
    if (state->arr[i] <= state->arr[pivot_idx] && i < state->right_idx) {
        state->i++;
        return;
    }

    if (state->arr[j] >= state->arr[pivot_idx] && j > state->left_idx) {
        state->j--;
        return;
    }

    // need check here because should not swap if i >= j which occurs
    // on the last iteration
    if (i < j) {
        swap(&state->arr[i], &state->arr[j]);
    } else {
        // place pivot at right index
        swap(&state->arr[state->left_idx], &state->arr[j]);
        state->partition_done = true;
    }
}
void quick_sort_step(struct QuickSortState* state) {
    if (!state->partition_done) {
        partition_step(state);
    } else if (state->stack_ptr > 0) {
        if (!state->first_iter) {
            int pivot = state->j;
            state->stack[state->stack_ptr++] = state->left_idx;
            state->stack[state->stack_ptr++] = pivot - 1;

            state->stack[state->stack_ptr++] = pivot + 1;
            state->stack[state->stack_ptr++] = state->right_idx;
        }
        state->first_iter = false;

        int right_idx = state->stack[--state->stack_ptr];
        int left_idx = state->stack[--state->stack_ptr];
        if (left_idx >= right_idx) {
            return;
        }

        state->partition_done = false;
        state->pivot_idx = left_idx;
        state->i = left_idx + 1;
        state->j = right_idx;
    } else {
        state->done = true;
    }
}

void quick_sort_iterative(int* arr, int len) {
    int stack[1000];
    int stack_ptr = 0;
    stack[stack_ptr++] = 0;
    stack[stack_ptr++] = len - 1;

    while (stack_ptr > 0) {
        int right_idx = stack[--stack_ptr];
        int left_idx = stack[--stack_ptr];
        if (left_idx >= right_idx) {
            continue;
        }

        int pivot_idx = partition(arr, left_idx, right_idx);

        stack[stack_ptr++] = left_idx;
        stack[stack_ptr++] = pivot_idx - 1;

        stack[stack_ptr++] = pivot_idx + 1;
        stack[stack_ptr++] = right_idx;
    }
}

#define ARR_LEN 20

int main() {
    int arr[ARR_LEN];
    for (int i = 0; i < ARR_LEN; i++) {
        arr[i] = rand() % 20;
    }

    printf("Original array: ");
    for (int i = 0; i < ARR_LEN; i++) {
        printf("%d, ", arr[i]);
    }
    clock_t start, end;

    start = clock();
    struct QuickSortState state;
    quick_sort_init(&state, arr, ARR_LEN);
    while (!state.done) {
        quick_sort_step(&state);
    }
    end = clock();

    // printf("\npivot: %d", state.arr[state.pivot_idx]);
    printf("\nSorted array: ");
    for (int i = 0; i < ARR_LEN; i++) {
        printf("%d, ", arr[i]);
    }

    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nSorting took %f seconds", cpu_time_used);
    return 0;
}
