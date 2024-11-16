#include "algorithms.h"

// ================== Selection sort ==================
void selection_sort_init(struct SelectionSortState* state, int arr[], int n) {
    state->arr = arr;
    state->len = n;
    state->iter_idx = 0;
    state->inner_idx = 0;
    state->min_idx = 0;
    state->done = false;
}

void selection_sort_step(struct SelectionSortState* s) {
    if (s->iter_idx < s->len - 1) {
        if (s->inner_idx < s->len) {
            if (s->arr[s->inner_idx] < s->arr[s->min_idx]) {
                s->min_idx = s->inner_idx;
            }
            s->inner_idx++;
        } else {
            int temp = s->arr[s->iter_idx];
            s->arr[s->iter_idx] = s->arr[s->min_idx];
            s->arr[s->min_idx] = temp;
            s->iter_idx++;
            s->inner_idx = s->iter_idx + 1;
            s->min_idx = s->iter_idx;
        }
    } else {
        s->done = true;
    }
}

// ================== Insertion sort ==================
void insert_sort_init(struct InsertSortState* state, int* arr, int n) {
    // NOTE: order of initialization matters
    state->arr = arr;
    state->iter_idx = 0;
    state->insert_idx = state->iter_idx - 1;
    state->len = n;
    state->value = state->arr[state->iter_idx];
    state->done = false;
}

void insert_sort_step(struct InsertSortState* s) {
    if (s->iter_idx < s->len) {
        if (s->insert_idx >= 0 && s->arr[s->insert_idx] > s->value) {
            s->arr[s->insert_idx + 1] = s->arr[s->insert_idx];
            s->insert_idx--;
        } else {
            s->arr[s->insert_idx + 1] = s->value;
            s->iter_idx++;
            s->value = s->arr[s->iter_idx];
            s->insert_idx = s->iter_idx - 1;
        }

    } else {
        s->done = true;
    }
}

// ================== Merge sort ==================
static int build_merge_stack(int* stack, int stack_ptr, int left_idx, int right_idx) {
    if (left_idx >= right_idx) {
        return stack_ptr;
    }

    stack[stack_ptr++] = left_idx;
    stack[stack_ptr++] = right_idx;

    int mid = left_idx + (right_idx - left_idx) / 2;
    stack_ptr = build_merge_stack(stack, stack_ptr, mid + 1, right_idx);
    return build_merge_stack(stack, stack_ptr, left_idx, mid);
}

static void merge_init(struct MergeSortState* state, bool should_realloc) {
    // reset merge step state to initial values
    state->merge_done = false;
    state->subarr_left_idx = 0;
    state->subarr_right_idx = 0;
    state->merge_iter = 0;

    // reallocate subarr_left and subarr_right to fit the new
    // subarray sizes and copy elements from arr
    int mid = state->merge_left_idx + (state->merge_right_idx - state->merge_left_idx) / 2;
    int left_len = mid - state->merge_left_idx + 1;
    int right_len = state->merge_right_idx - mid;
    if (should_realloc) {
        state->subarr_left = realloc(state->subarr_left, left_len * sizeof(int));
        state->subarr_right = realloc(state->subarr_right, right_len * sizeof(int));
    } else {
        state->subarr_left = malloc(left_len * sizeof(int));
        state->subarr_right = malloc(right_len * sizeof(int));
    }

    // copy elements from arr to left within interval [left_idx, mid]
    for (int i = 0; i < left_len; i++) {
        state->subarr_left[i] = state->arr[state->merge_left_idx + i];
    }

    // copy elements from arr to right within interval (mid, right_idx]
    for (int i = 0; i < right_len; i++) {
        state->subarr_right[i] = state->arr[mid + i + 1];
    }
}

void merge_sort_init(struct MergeSortState* state, int* arr, int len) {
    state->arr = arr;
    state->len = len;
    state->done = false;
    state->stack = malloc(1000 * sizeof(int)); // somewhat arbitrary size
    state->stack_ptr = build_merge_stack(state->stack, 0, 0, len - 1);
    state->merge_left_idx = 0;
    state->merge_right_idx = len - 1;

    merge_init(state, false);
}

static void merge_step(struct MergeSortState* state) {
    // just a couple of variables to make the code more readable
    int lo = state->merge_left_idx;
    int hi = state->merge_right_idx;
    int mid = lo + (hi - lo) / 2;

    int* a = state->subarr_left;
    int* b = state->subarr_right;
    int a_len = mid - lo + 1;
    int b_len = hi - mid;
    int a_idx = state->subarr_left_idx;
    int b_idx = state->subarr_right_idx;

    // NOTE: a is subarr_left, b is subarr_right
    // loop until we have traversed all elements in
    // subarr_left and subarr_right
    if (state->merge_iter < a_len + b_len) {
        // if we have traversed all elements in subarr_left,
        // then add the remaining elements from subarr_right
        if (a_idx >= a_len) {
            state->arr[lo + state->merge_iter] = b[b_idx];
            state->subarr_right_idx++;
            state->merge_iter++;
            return;
        }

        // if we have traversed all elements in subarr_right,
        // then add the remaining elements from subarr_left
        if (b_idx >= b_len) {
            state->arr[lo + state->merge_iter] = a[a_idx];
            state->subarr_left_idx++;
            state->merge_iter++;
            return;
        }

        // if element in subarr_left is less than or equal to element
        // in subarr_right, then add element in subarr_left to arr and
        // move to next element in subarr_left
        if (a[a_idx] <= b[b_idx]) {
            state->arr[lo + state->merge_iter] = a[a_idx];
            state->subarr_left_idx++;
        } else {
            state->arr[lo + state->merge_iter] = b[b_idx];
            state->subarr_right_idx++;
        }
        state->merge_iter++;
        return;
    } else {
        state->merge_done = true;
    }
}

void merge_sort_step(struct MergeSortState* state) {
    // analagous to merge_sort_iterative_v2 in the extras directory,
    // reference that version for better understanding of the algorithm,
    // this is a cluterfuck
    if (!state->merge_done) {
        merge_step(state);
    } else if (state->stack_ptr > 1) {
        // pop the next pair of subarrays to merge from the stack
        state->merge_right_idx = state->stack[--state->stack_ptr];
        state->merge_left_idx = state->stack[--state->stack_ptr];
        // reset merge step state to initial values
        merge_init(state, true);
    } else {
        state->done = true;
    }
}
