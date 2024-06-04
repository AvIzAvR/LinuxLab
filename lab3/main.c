#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <locale.h>

int total_processes = 0;

typedef struct _STATS {
    int primary;
    int secondary;
} STATS;
STATS stats;

int process_statistics[4] = {0, 0, 0, 0};

bool collect_statistics = true;
bool display_statistics = true;
bool global_flag = true;

pid_t process_queue[100];
int queue_size = 0;

void terminate_all_processes();
void fetch_process_statistics();
void switch_stats();
void show_process_statistics();
void spawn_new_process();
void disable_display_statistics();
void enable_display_statistics();
void send_signal_to_process(int signal_num, int process_num);
void set_global_flag();

int main() {
    setlocale(LC_ALL, "");

    signal(SIGUSR1, disable_display_statistics);
    signal(SIGUSR2, enable_display_statistics);
    signal(SIGINT, show_process_statistics);
    signal(SIGALRM, set_global_flag);

    pid_t process_id;

    bool terminate_flag = false;
    int input_key;
    while (!terminate_flag) {
        input_key = (int)getchar();
        switch (input_key) {
            case '+': {
                process_id = fork();
                if (process_id == 0) {
                    signal(SIGALRM, fetch_process_statistics);
                    spawn_new_process();
                } else if (process_id == -1) {
                    perror("Error spawning new process.\n");
                } else {
                    process_queue[queue_size] = process_id;
                    queue_size++;
                    printf("New process is spawned C_%d\n", total_processes);
                    total_processes++;
                }
                break;
            }
            case '-': {
                if (total_processes == 0) {
                    printf("No process spawned.\n");
                } else {
                    kill(process_queue[queue_size - 1], SIGKILL);
                    queue_size--;
                    total_processes--;
                    printf("C_%d is successfully terminated. %d child are still alive\n", total_processes, total_processes);
                }
                break;
            }
            case 'l': {
                printf("Parent: %d\n", getppid());
                if (queue_size != 0) {
                    printf("Process queue: ");
                    for (int i = 0; i < queue_size; i++) {
                        printf("%d ", process_queue[i]);
                    }
                    printf("\n");
                }
                break;
            }
            case 'k': {
                if (queue_size == 0) {
                    printf("No process spawned.\n");
                } else {
                    queue_size = 0;
                    printf("All processes are terminated.\n");
                    total_processes = 0;
                }
                break;
            }
            case 's': {
                if (queue_size == 0) {
                    printf("No process spawned.\n");
                } else {
                    int process_num;
                    printf("Enter process number or 0 for all: ");
                    scanf("%d", &process_num);

                    if (process_num < 0 || process_num > total_processes) {
                        printf("No such process.\n");
                    } else {
                        send_signal_to_process(SIGUSR1, process_num);
                    }
                }
                break;
            }
            case 'g': {
                if (queue_size == 0) {
                    printf("No process spawned.\n");
                } else {
                    int process_num;
                    printf("Enter process number or 0 for all: ");
                    scanf("%d", &process_num);

                    if (process_num < 0 || process_num > total_processes) {
                        printf("No such process.\n");
                    } else {
                        global_flag = true;
                        send_signal_to_process(SIGUSR2, process_num);
                    }
                }
                break;
            }
            case 'p': {
                if (queue_size == 0) {
                    printf("No process spawned.\n");
                } else {
                    int process_num;
                    printf("Enter process number for fetching process statistics: ");
                    scanf("%d", &process_num);

                    if (process_num < 0 || process_num > total_processes) {
                        printf("No such process.\n");
                    } else {
                        send_signal_to_process(SIGUSR1, 0);

                        global_flag = false;
                        alarm(5);
                        send_signal_to_process(SIGINT, process_num);
                    }
                }
                break;
            }
            case 'q': {
                terminate_all_processes();
                terminate_flag = true;
                break;
            }
            default:
                break;
        }
    }

    return 0;
}

void terminate_all_processes() {
    for (int i = 0; i < queue_size; i++) {
        kill(process_queue[i], SIGKILL);
        total_processes--;
    }
    queue_size = 0;
}

void fetch_process_statistics() {
    collect_statistics = false;
    if(stats.primary == 0 && stats.secondary == 0) {
        process_statistics[0]++;
    }
    else if(stats.primary == 1 && stats.secondary == 1) {
        process_statistics[3]++;
    }
}

void switch_stats() {
    if(stats.primary == 0 && stats.secondary == 0) {
        stats.primary++;
        stats.secondary++;
    }
    else if(stats.primary == 1 && stats.secondary == 1) {
        stats.secondary--;
        stats.primary--;
    }
}

void disable_display_statistics() {
    display_statistics = false;
}

void enable_display_statistics() {
    display_statistics = true;
}

void show_process_statistics() {
    printf("Process statistics: C_%d\n", getpid());
    for (int i = 0; i < 4; i++) {
        printf("Process type %d: %d\n", i+1, process_statistics[i]);
    }
}

void spawn_new_process() {
    ualarm(500000, 500000);

    int cycle_restart = 10;
    while(true) {
        while(collect_statistics) {
            switch_stats();
        }
        collect_statistics = true;

        cycle_restart--;
        if(cycle_restart == 0) {
            if(display_statistics) {
                show_process_statistics();
            }
            cycle_restart = 10;
        }
    }
}

void send_signal_to_process(int signal_num, int process_num) {
    if (process_num == 0) {
        for (int i = 0; i < queue_size; i++) {
            kill(process_queue[i], signal_num);
        }
    } else {
        kill(process_queue[process_num - 1], signal_num);
    }
}

void set_global_flag() {
    if(!global_flag) {
        send_signal_to_process(SIGUSR2, 0);
    }

    global_flag = true;
}
