#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void merge(int* arr, int left_idx, int mid, int right_idx) {
    int a_len = mid - left_idx + 1;
    int b_len = right_idx - mid;
    int a[a_len];
    int b[b_len];

    // copy elements from arr to a within interval [left, mid]
    for (int i = 0; i < a_len; i++) {
        a[i] = arr[left_idx + i];
    }

    // copy elements from arr to b within interval (mid, right]
    for (int i = 0; i < b_len; i++) {
        b[i] = arr[mid + i + 1];
    }

    int a_idx = 0, b_idx = 0;
    // loop until we have traversed all elements in a and b
    for (int i = 0; i < right_idx - left_idx + 1; i++) {
        // if we have traversed all elements in a,
        // then add the remaining elements from b
        if (a_idx >= a_len) {
            arr[left_idx + i] = b[b_idx];
            b_idx++;
            continue;
        }

        // if we have traversed all elements in b,
        // then add the remaining elements from a
        if (b_idx >= b_len) {
            arr[left_idx + i] = a[a_idx];
            a_idx++;
            continue;
        }

        // if element in a is less than or equal to element in b,
        // then add element in a to arr and move to next element in a
        if (a[a_idx] <= b[b_idx]) {
            arr[left_idx + i] = a[a_idx];
            a_idx++;
        } else {
            arr[left_idx + i] = b[b_idx];
            b_idx++;
        }
    }
}

void merge_sort_recursive(int* arr, int left_idx, int right_idx) {
    if (left_idx >= right_idx) {
        return;
    }

    int mid = left_idx + (right_idx - left_idx) / 2;
    merge_sort_recursive(arr, left_idx, mid);
    merge_sort_recursive(arr, mid + 1, right_idx);

    // merge the two sorted arrays together on they way up the call stack
    merge(arr, left_idx, mid, right_idx);
}

int min(int x, int y) { return (x < y) ? x : y; }

void merge_sort_iterative(int* arr, int len) {
    // merge subarrays of size 1, then 2, then 4, etc. from bottom up
    // until we reach the full array
    // each iteration is a pass through the array, merging subarrays
    // will preform log2(len) iterations
    for (int curr_size = 1; curr_size < len; curr_size = 2 * curr_size) {
        // merge all subarrays pair-wise from left to right
        // e.g. subarray 1 and 2, then 3 and 4, etc. each with size curr_size
        for (int left_idx = 0; left_idx < len - 1; left_idx += 2 * curr_size) {
            // must take min to avoid out of bounds access
            int mid = min(left_idx + curr_size - 1, len - 1);
            int right_idx = min(left_idx + 2 * curr_size - 1, len - 1);
            merge(arr, left_idx, mid, right_idx);
        }
    }
}

struct MergeSortState {
    int* arr;
    int len;
    int curr_size;
    int left_start;
    bool merge_done;
    bool first_merge_iter;
    bool done;
    int* a;
    int* b;
    int a_idx;
    int b_idx;
    int i;
};

void merge_step(struct MergeSortState* state) {
    if (state->a_idx >= state->len) {
        state->arr[state->i] = state->b[state->b_idx];
        state->b_idx++;
        state->i++;
        return;
    }

    if (state->b_idx >= state->len) {
        state->arr[state->i] = state->a[state->a_idx];
        state->a_idx++;
        state->i++;
        return;
    }

    if (state->a[state->a_idx] <= state->b[state->b_idx]) {
        state->arr[state->i] = state->a[state->a_idx];
        state->a_idx++;
    } else {
        state->arr[state->i] = state->b[state->b_idx];
        state->b_idx++;
    }
    state->i++;
}

void merge_sort_init(struct MergeSortState* state, int* arr, int len) {
    state->arr = arr;
    state->len = len;
    state->curr_size = 1;
    state->left_start = 0;
    state->done = false;
    state->merge_done = false;
    state->first_merge_iter = true;
}

void merge_sort_step(struct MergeSortState* state) {
    // analagous to merge_sort_iterative, reference that version
    // for better understanding of the algorithm, this is a cluterfuck
    if (state->left_start < state->len - 1) {
        int mid = min(state->left_start + state->curr_size - 1, state->len - 1);
        int right_end = min(state->left_start + 2 * state->curr_size - 1, state->len - 1);
        merge(state->arr, state->left_start, mid, right_end);
        state->left_start += 2 * state->curr_size;
    } else if (state->curr_size <= state->len - 1) {
        state->curr_size *= 2;
        state->left_start = 0;
    } else {
        state->done = true;
    }
}

#define ARR_LEN 20

int main() {
    int arr[ARR_LEN];
    for (int i = 0; i < ARR_LEN; i++) {
        arr[i] = rand() % 100;
    }

    printf("Original array: ");
    for (int i = 0; i < ARR_LEN; i++) {
        printf("%d, ", arr[i]);
    }

    clock_t start, end;

    start = clock();
    // merge_sort(arr, ARR_LEN);
    struct MergeSortState state;
    merge_sort_init(&state, arr, ARR_LEN);
    while (!state.done) {
        merge_sort_step(&state);
    }

    end = clock();

    printf("\nSorted array: ");
    for (int i = 0; i < ARR_LEN; i++) {
        printf("%d, ", arr[i]);
    }

    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nSorting took %f seconds", cpu_time_used);
    return 0;
}
