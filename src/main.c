#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define COLUMN_MAX_HEIGHT 400
#define COLUMN_MIN_HEIGHT 20

#define NUM_COLUMNS SCREEN_WIDTH / 4 // screen with is divisable by 4

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

enum AlgorithmType {
    SELECTION_SORT,
    INSERT_SORT,
    MERGE_SORT,
    QUICK_SORT,
    BUBBLE_SORT,
};

typedef struct Color {
    int r;
    int g;
    int b;
    int a;
} Color_t;

struct ColumnDrawData {
    // coordinate system: (0, 0) is the top-left corner,
    // x increases to the right, y increases downwards
    int x; // x-coordinate of the first column
    int y; // y-coordinate of the first column
    int w; // width of a column
    int num_columns;
    int num_colored_columns;
    int* columns;
    // the color at color[i] will be applied to the column
    // at colored_column_indices[i]
    int* colored_columns_indices;
    Color_t* colors;
};

struct App {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    enum AlgorithmType algorithm_type;
    struct SelectionSortState selection_sort_state;
    struct InsertSortState insert_sort_state;
    struct MergeSortState merge_sort_state;

    int rnd_values[NUM_COLUMNS];
};

const Color_t SECONDARY = {255, 0, 0, 255};
const Color_t PRIMARY = {0, 255, 0, 255};
const Color_t TERTIARY = {0, 0, 255, 255};
const Color_t WHITE = {255, 255, 255, 255};

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
int build_merge_stack(int* stack, int stack_ptr, int left_idx, int right_idx) {
    if (left_idx >= right_idx) {
        return stack_ptr;
    }

    stack[stack_ptr++] = left_idx;
    stack[stack_ptr++] = right_idx;

    int mid = left_idx + (right_idx - left_idx) / 2;
    stack_ptr = build_merge_stack(stack, stack_ptr, mid + 1, right_idx);
    return build_merge_stack(stack, stack_ptr, left_idx, mid);
}

void merge_init(struct MergeSortState* state, bool should_realloc) {
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

void merge_step(struct MergeSortState* state) {
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

// ================== Main ==================
void draw_columns(SDL_Renderer* const renderer, struct ColumnDrawData data) {
    for (int i = 0; i < data.num_columns; i++) {
        SDL_Rect column =
            {data.x + i * data.w, data.y, data.w, -data.columns[i]};
        Color_t color = WHITE;
        for (int j = 0; j < data.num_colored_columns; j++) {
            if (i == data.colored_columns_indices[j]) {
                color = data.colors[j];
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &column);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}

void column_draw_data_init(struct ColumnDrawData* data,
                           int* columns,
                           int num_columns) {
    data->x = 0;
    data->y = SCREEN_HEIGHT;
    data->w = SCREEN_WIDTH / num_columns;
    data->num_columns = num_columns;
    data->columns = columns;
    data->num_colored_columns = 0;
    data->colored_columns_indices = NULL;
    data->colors = NULL;
}

// reset app to initial state
void reset(struct App* app) {
    for (int i = 0; i < NUM_COLUMNS; i++) {
        app->rnd_values[i] =
            rand() % (COLUMN_MAX_HEIGHT - COLUMN_MIN_HEIGHT + 2) +
            COLUMN_MIN_HEIGHT;
    }

    selection_sort_init(&app->selection_sort_state, app->rnd_values, NUM_COLUMNS);
    insert_sort_init(&app->insert_sort_state, app->rnd_values, NUM_COLUMNS);
    merge_sort_init(&app->merge_sort_state, app->rnd_values, NUM_COLUMNS);
}

// initializes SDL2 and create a window among other things
bool init(struct App* app) {
    printf("Initializing SDL2...\n");
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH,
                                    SCREEN_HEIGHT,
                                    0,
                                    &app->window,
                                    &app->renderer) < 0) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderClear(app->renderer);
    SDL_RenderPresent(app->renderer);

    app->algorithm_type = MERGE_SORT;

    reset(app);

    return true;
}

int main() {
    struct App app;

    if (!init(&app)) {
        return EXIT_FAILURE;
    }

    struct ColumnDrawData draw_info;
    column_draw_data_init(&draw_info, app.rnd_values, NUM_COLUMNS);

    // --- Main loop ---
    while (true) {
        switch (app.algorithm_type) {
        case SELECTION_SORT:
            if (!app.selection_sort_state.done) {
                selection_sort_step(&app.selection_sort_state);
            }

            {
                Color_t colors[] = {PRIMARY, SECONDARY, TERTIARY};
                int ind[] = {app.selection_sort_state.iter_idx,
                             app.selection_sort_state.inner_idx,
                             app.selection_sort_state.min_idx};
                draw_info.num_colored_columns = 3;
                draw_info.colored_columns_indices = ind;
                draw_info.colors = colors;
                break;
            }
        case INSERT_SORT:
            if (!app.insert_sort_state.done) {
                insert_sort_step(&app.insert_sort_state);
            }

            {
                Color_t colors[] = {PRIMARY, SECONDARY};
                int ind[] = {app.insert_sort_state.iter_idx + 1,
                             app.insert_sort_state.insert_idx};
                draw_info.num_colored_columns = 2;
                draw_info.colored_columns_indices = ind;
                draw_info.colors = colors;
                break;
            }
        case MERGE_SORT:
            if (!app.merge_sort_state.done) {
                merge_sort_step(&app.merge_sort_state);
            }

            {
                Color_t colors[] = {SECONDARY, SECONDARY, TERTIARY, PRIMARY, PRIMARY};
                int left_idx = app.merge_sort_state.merge_left_idx;
                int right_idx = app.merge_sort_state.merge_right_idx;
                int mid = left_idx + (right_idx - left_idx) / 2;
                int a_ind = left_idx + app.merge_sort_state.subarr_left_idx;
                int b_ind = mid + app.merge_sort_state.subarr_right_idx + 1;
                int i = left_idx + app.merge_sort_state.merge_iter;
                int ind[] = {a_ind, b_ind, i, left_idx, right_idx};
                draw_info.num_colored_columns = 5;
                draw_info.colored_columns_indices = ind;
                draw_info.colors = colors;
                break;
            }
        default:
            break;
        }

        SDL_RenderClear(app.renderer);
        draw_columns(app.renderer, draw_info);
        SDL_RenderPresent(app.renderer);

        if (SDL_PollEvent(&app.event)) {
            if (app.event.type == SDL_QUIT) {
                break;
            }
        }

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
