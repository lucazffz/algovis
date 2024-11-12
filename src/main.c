#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define COLUMN_MAX_HEIGHT 400
#define COLUMN_MIN_HEIGHT 20

#define NUM_COLUMNS SCREEN_WIDTH / 4 // screen with divisable by 4

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

enum Algorithm {
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

const Color_t SECONDARY = {255, 0, 0, 255};
const Color_t PRIMARY = {0, 255, 0, 255};
const Color_t TERTIARY = {0, 0, 255, 255};
const Color_t WHITE = {255, 255, 255, 255};

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

int main() {
    // --- Initialize SDL2 ---
    printf("Initializing SDL2...\n");
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event window_event;

    int8_t success = SDL_CreateWindowAndRenderer(SCREEN_WIDTH,
                                                 SCREEN_HEIGHT,
                                                 0,
                                                 &window,
                                                 &renderer);
    if (success == -1) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // --- Initialize ---
    int rnd_values[NUM_COLUMNS];
    for (int i = 0; i < NUM_COLUMNS; i++) {
        rnd_values[i] =
            rand() % (COLUMN_MAX_HEIGHT - COLUMN_MIN_HEIGHT + 2) +
            COLUMN_MIN_HEIGHT;
    }

    // NOTE: the sorting algorithms will mutate rnd_values
    struct SelectionSortState selection_sort_state;
    selection_sort_init(&selection_sort_state, rnd_values, NUM_COLUMNS);

    struct InsertSortState insert_sort_state;
    insert_sort_init(&insert_sort_state, rnd_values, NUM_COLUMNS);

    struct ColumnDrawData draw_info;
    column_draw_data_init(&draw_info, rnd_values, NUM_COLUMNS);

    enum Algorithm algorithm = INSERT_SORT;

    // --- Main loop ---
    while (true) {
        switch (algorithm) {
        case SELECTION_SORT:
            if (!selection_sort_state.done) {
                selection_sort_step(&selection_sort_state);
            }

            {
                Color_t colors[] = {PRIMARY, SECONDARY, TERTIARY};
                int ind[] = {selection_sort_state.iter_idx,
                             selection_sort_state.inner_idx,
                             selection_sort_state.min_idx};
                draw_info.num_colored_columns = 3;
                draw_info.colored_columns_indices = ind;
                draw_info.colors = colors;
                break;
            }
        case INSERT_SORT:
            if (!insert_sort_state.done) {
                insert_sort_step(&insert_sort_state);
            }

            {
                Color_t colors[] = {PRIMARY, SECONDARY};
                int ind[] = {insert_sort_state.iter_idx + 1,
                             insert_sort_state.insert_idx};
                draw_info.num_colored_columns = 2;
                draw_info.colored_columns_indices = ind;
                draw_info.colors = colors;
                break;
            }
        default:
            break;
        }

        SDL_RenderClear(renderer);
        draw_columns(renderer, draw_info);
        SDL_RenderPresent(renderer);

        if (SDL_PollEvent(&window_event)) {
            if (window_event.type == SDL_QUIT) {
                break;
            }
        }

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
