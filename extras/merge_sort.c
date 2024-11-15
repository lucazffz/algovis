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

void merge_sort_iterative_v1(int* arr, int len) {
    // pairwise merge entire array split into subarrays of size 1, then 2,
    // then 4, etc. from bottom up in a level like fasion
    // until we reach the full array

    // each iteration is a pass through the array, merging subarrays
    // will preform log2(len) iterations

    // NOTE: this implementation will merge on a different order
    // from a traditional recursive merge sort implementation and is therefore
    // not appropriate for visualizing, but the result will be the same

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

int build_merge_stack(int* stack, int stack_ptr, int left_idx, int right_idx) {
    if (left_idx >= right_idx) {
        return stack_ptr;
    }

    stack[stack_ptr++] = left_idx;
    stack[stack_ptr++] = right_idx;

    int mid = left_idx + (right_idx - left_idx) / 2;
    stack_ptr = build_merge_stack(stack, stack_ptr, left_idx, mid);
    return build_merge_stack(stack, stack_ptr, mid + 1, right_idx);
}

void merge_sort_iterative_v2(int* arr, int len) {
    // we are only interested of visualizing the mergeing
    // and since that happens on the way up the call stack
    // we can recursively create a stack of all indicies to merge
    // between beforehand and then iterate over the stack and merge
    // each index pair

    // some arbitary size for the stack, enoough for arrays of up to size 2^50
    // since we ony store log2(len)*2 elements on the stack
    int stack[100] = {0};
    int stack_ptr = build_merge_stack(stack, 0, 0, len - 1);

    // iterate over the stack and merge each pair of subarrays
    while (stack_ptr > 1) {
        int right_idx = stack[--stack_ptr];
        int left_idx = stack[--stack_ptr];
        int mid = left_idx + (right_idx - left_idx) / 2;
        merge(arr, left_idx, mid, right_idx);
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
    merge_sort_recursive(arr, 0, ARR_LEN - 1);
    end = clock();

    printf("\nSorted array: ");
    for (int i = 0; i < ARR_LEN; i++) {
        printf("%d, ", arr[i]);
    }

    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nSorting took %f seconds", cpu_time_used);
    return 0;
}
