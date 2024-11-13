#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

void merge_sort_help(int* arr, int left_idx, int right_idx) {
    if (left_idx >= right_idx) {
        return;
    }

    int mid = left_idx + (right_idx - left_idx) / 2;
    merge_sort_help(arr, left_idx, mid);
    merge_sort_help(arr, mid + 1, right_idx);

    // merge the two sorted arrays together on they way up the call stack
    merge(arr, left_idx, mid, right_idx);
}

void merge_sort(int* arr, int len) {
    merge_sort_help(arr, 0, len - 1);
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

    merge_sort(arr, ARR_LEN);
    printf("\nSorted array: ");
    for (int i = 0; i < ARR_LEN; i++) {
        printf("%d, ", arr[i]);
    }
    return 0;
}
