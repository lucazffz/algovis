#include "algorithms.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define COLUMN_MAX_HEIGHT 400
#define COLUMN_MIN_HEIGHT 20

#define DELAY_MS 10

#define NUM_COLUMNS SCREEN_WIDTH / 4 // screen with is divisable by 4

// Keyboard controls
// -- R:     reset
// -- SPACE: start/stop
// -- S:     step
// -- 1:     selection sort
// -- 2:     insert sort
// -- 3:     merge sort

enum AlgorithmType {
    SELECTION_SORT,
    INSERT_SORT,
    MERGE_SORT,
    QUICK_SORT,
    BUBBLE_SORT,
};

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
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
    struct ColumnDrawData draw_info;

    int rnd_values[NUM_COLUMNS];
    bool running;
};

const Color_t PRIMARY = {4, 191, 157, 255};
const Color_t SECONDARY = {210, 64, 31, 255};
const Color_t TERTIARY = {200, 180, 60, 255};
const Color_t LIGHT = {220, 220, 220, 255};
const Color_t DARK = {6, 10, 18, 255};

void draw_columns(SDL_Renderer* const renderer, struct ColumnDrawData data) {
    for (int i = 0; i < data.num_columns; i++) {
        SDL_Rect column =
            {data.x + i * data.w, data.y, data.w, -data.columns[i]};
        Color_t color = LIGHT;
        for (int j = 0; j < data.num_colored_columns; j++) {
            if (i == data.colored_columns_indices[j]) {
                color = data.colors[j];
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &column);
        SDL_SetRenderDrawColor(renderer, DARK.r, DARK.g, DARK.b, DARK.a);
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

    app->running = false;
    column_draw_data_init(&app->draw_info, app->rnd_values, NUM_COLUMNS);
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

    app->algorithm_type = SELECTION_SORT;

    reset(app);

    return true;
}

int main() {
    struct App app;

    if (!init(&app)) {
        return EXIT_FAILURE;
    }

    bool should_step = false;

    // --- Main loop ---
    while (true) {
        if (app.running || should_step) {
            should_step = false;
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
                    app.draw_info.num_colored_columns = 3;
                    app.draw_info.colored_columns_indices = ind;
                    app.draw_info.colors = colors;
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
                    app.draw_info.num_colored_columns = 2;
                    app.draw_info.colored_columns_indices = ind;
                    app.draw_info.colors = colors;
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
                    app.draw_info.num_colored_columns = 5;
                    app.draw_info.colored_columns_indices = ind;
                    app.draw_info.colors = colors;
                    break;
                }
            default:
                break;
            }
        }

        SDL_RenderClear(app.renderer);
        draw_columns(app.renderer, app.draw_info);
        SDL_RenderPresent(app.renderer);

        if (SDL_PollEvent(&app.event)) {
            if (app.event.type == SDL_QUIT) {
                break;
            }

            if (app.event.type == SDL_KEYDOWN) {
                switch (app.event.key.keysym.sym) {
                case SDLK_r:
                    reset(&app);
                    break;
                case SDLK_1:
                    app.algorithm_type = SELECTION_SORT;
                    reset(&app);
                    break;
                case SDLK_2:
                    app.algorithm_type = INSERT_SORT;
                    reset(&app);
                    break;
                case SDLK_3:
                    app.algorithm_type = MERGE_SORT;
                    reset(&app);
                    break;
                case SDLK_SPACE:
                    app.running = !app.running;
                    break;
                case SDLK_s:
                    app.running = false;
                    should_step = true;
                    break;
                default:
                    break;
                }
            }
        }

        SDL_Delay(DELAY_MS);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
