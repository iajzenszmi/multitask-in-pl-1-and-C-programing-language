#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>

// Mutexes for task synchronization
pthread_mutex_t mux1, mux2;

// Terminal size
int disp_row = 24;
int disp_col = 80;

// Task functions
void *display_task(void *arg);
void *timer_task(void *arg);

// Utility functions
void clear_screen();
void move_cursor(int row, int col);
void set_color(int color);
void reset_color();
int get_random(int limit);

int main() {
    // Initialize mutexes
    pthread_mutex_init(&mux1, NULL);
    pthread_mutex_init(&mux2, NULL);

    // Clear the screen and set initial terminal state
    clear_screen();
    move_cursor(1, 1);

    // Create task threads
    pthread_t t1, t2, t3, t4, t5;
    pthread_create(&t1, NULL, display_task, (void *)1); // Red
    pthread_create(&t2, NULL, display_task, (void *)2); // Green
    pthread_create(&t3, NULL, display_task, (void *)3); // Blue
    pthread_create(&t4, NULL, display_task, (void *)4); // Yellow
    pthread_create(&t5, NULL, timer_task, NULL);        // Timer

    // Wait for threads to finish (they never will, unless interrupted)
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);

    // Clean up mutexes
    pthread_mutex_destroy(&mux1);
    pthread_mutex_destroy(&mux2);

    return 0;
}

// Task to display a colored block at random positions
void *display_task(void *arg) {
    int color = (int)arg;
    while (1) {
        pthread_mutex_lock(&mux1);

        // Generate random row and column
        int row = get_random(disp_row) + 1;
        int col = get_random(disp_col) + 1;

        // Move the cursor and print a colored block
        move_cursor(row, col);
        set_color(color);
        printf("■");
        fflush(stdout);

        reset_color();
        pthread_mutex_unlock(&mux1);

        // Sleep for 250ms
        usleep(250000);
    }
    return NULL;
}

// Task to display the elapsed time at the bottom left of the screen
void *timer_task(void *arg) {
    time_t start_time = time(NULL);

    while (1) {
        time_t current_time = time(NULL);
        int elapsed = (int)(current_time - start_time);

        int hours = elapsed / 3600;
        int minutes = (elapsed % 3600) / 60;
        int seconds = elapsed % 60;

        pthread_mutex_lock(&mux2);

        // Move the cursor and print the elapsed time
        move_cursor(disp_row, 1);
        printf("%02d:%02d:%02d", hours, minutes, seconds);
        fflush(stdout);

        pthread_mutex_unlock(&mux2);

        // Sleep for 1 second
        sleep(1);
    }
    return NULL;
}

// Utility function to clear the terminal screen
void clear_screen() {
    printf("\033[2J"); // ANSI escape code to clear the screen
}

// Utility function to move the cursor to (row, col)
void move_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col); // ANSI escape code to move cursor
}

// Utility function to set terminal text color
void set_color(int color) {
    switch (color) {
        case 1: printf("\033[31m"); break; // Red
        case 2: printf("\033[32m"); break; // Green
        case 3: printf("\033[34m"); break; // Blue
        case 4: printf("\033[33m"); break; // Yellow
    }
}

// Utility function to reset terminal text color
void reset_color() {
    printf("\033[0m"); // ANSI escape code to reset color
}

// Utility function to generate a random number between 0 and limit-1
int get_random(int limit) {
    return rand() % limit;
}

