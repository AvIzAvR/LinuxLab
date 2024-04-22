#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <locale.h>

#include "List.h"

#define GREEN_COLOR "\033[1;32m"
#define RED_COLOR "\033[1;31m"
#define RESET_COLOR "\033[0m"

#define CYCLE_RESTARTS 10
#define ALARM_CLOCK_MSEC 500000
#define P_TIME 5

int num_process = 0;

typedef struct _STAT {
    int first;
    int second;
} STAT;

STAT stat;
int statistic[4] = {0, 0, 0, 0};
bool is_collect = true;
bool is_print = true;

bool is_g = true;

List* head = NULL, *tail = NULL;

void get_statistic();
void stat_alternation();
void print_statistic();
void new_process();
void prohibit_print();
void allow_print();
void signal_to_process(int signum, int num);
void set_g();

int main() {
    setlocale(LC_ALL, "");

    signal(SIGUSR1, prohibit_print);
    signal(SIGUSR2, allow_print);
    signal(SIGINT, print_statistic);
    signal(SIGALRM, set_g);

    pid_t process;

    bool is_break = false;
    int key;
    while(!is_break) {
        key = (int)getchar();
        switch (key) {
            case '+': {
                process = fork();
                if(process == 0) {
                    signal(SIGALRM, get_statistic);
                    new_process();
                }
                else if(process == -1) {
                    perror(": error of created new process.\n");
                }
                else {
                    add_last(&head, &tail, process);
                    printf("New process is created "RED_COLOR"C_%d\n"RESET_COLOR, num_process);
                    num_process++;
                }
                break;
            }
            case '-': {
                if(num_process == 0) {
                    printf("Process is not created.\n");
                } else {
                    kill(tail->data, SIGKILL);
                    remove_last(&head, &tail);
                    num_process--;

                    printf(GREEN_COLOR"C_%d"RESET_COLOR " is successfully killed."RED_COLOR" %d"RESET_COLOR " child are still alive\n",
                            num_process , num_process);
                }
                break;
            }
            case 'l': {
                printf(GREEN_COLOR"Parent: %d\n"RESET_COLOR, getppid());
                print_list(head);
                break;
            }
            case 'k': {
                free_list(head);
                printf("All process is"RED_COLOR" killed.\n"RESET_COLOR);
                num_process = 0;
                break;
            }
            case 's': {
                if(num_process == 0) {
                    printf("Process is not created.\n");
                } else {
                    int num;
                    printf("Enter num of process or 0 for all: ");
                    scanf("%d", &num);

                    if(num < 0 || num > num_process) {
                        printf("No such process.\n");
                    } else {
                        signal_to_process(SIGUSR1, num);
                    }
                }
                break;
            }
            case 'g': {
                if(num_process == 0) {
                    printf("Process is not created.\n");
                } else {
                    int num;
                    printf("Enter num of process or 0 for all: ");
                    scanf("%d", &num);

                    if(num < 0 || num > num_process) {
                        printf("No such process.\n");
                    } else {
                        is_g = true;
                        signal_to_process(SIGUSR2, num);
                    }
                }
                break;
            }
            case 'p': {
                if(num_process == 0) {
                    printf("Process is not created.\n");
                } else {
                    int num;
                    printf("Enter num of process for get statistic: ");
                    scanf("%d", &num);

                    if(num < 0 || num > num_process) {
                        printf("No such process.\n");
                    } else {
                        signal_to_process(SIGUSR1, 0);

                        is_g = false;
                        alarm(P_TIME);
                        signal_to_process(SIGINT, num);
                    }
                }
                break;
            }
            case 'q': {
                free_list(head);
                is_break = true;
                break;
            }
            default: break;
        }
    }

    return 1;
}

void get_statistic() {
    is_collect = false;
    if(stat.first == 0 && stat.second == 0) {
        statistic[0]++;
    }
    else if(stat.first == 0 && stat.second == 1) {
        statistic[1]++;
    }
    else if(stat.first == 1 && stat.second == 0) {
        statistic[2]++;
    }
    else if(stat.first == 1 && stat.second == 1) {
        statistic[3]++;
    }
}

void stat_alternation() {
    if(stat.first == 0 && stat.second == 0
        || stat.first == 1 && stat.second == 0) {
        stat.second++;
    }
    else if(stat.first == 0 && stat.second == 1) {
        stat.second--;
        stat.first++;
    }
    else if(stat.first == 1 && stat.second == 1) {
        stat.second--;
        stat.first--;
    }
}

void prohibit_print() {
    is_print = false;
}

void allow_print() {
    is_print = true;
}

void print_statistic() {
    printf("Statistic of process "RED_COLOR"C_%d"RESET_COLOR" pid"GREEN_COLOR" %d"RESET_COLOR" : {0, 0} - %d; {0, 1} - %d; {1, 0} - %d; {1, 1} - %d\n",
            num_process ,getpid(), statistic[0], statistic[1], statistic[2], statistic[3]);
}

void new_process() {
    ualarm(ALARM_CLOCK_MSEC, ALARM_CLOCK_MSEC);

    int cycle_res = CYCLE_RESTARTS;
    while(true) {
        while(is_collect) {
            stat_alternation();
        }
        is_collect = true;

        cycle_res--;
        if(cycle_res == 0) {
            if(is_print) {
                print_statistic();
            }
            cycle_res = CYCLE_RESTARTS;
        }
    }
}

void signal_to_process(int signum, int num) {
    List* temp = head;
    int i = 1;
    while(temp != NULL) {
        if(num == 0 || i == num) {
            kill(temp->data, signum);
        }

        i++;
        temp = temp->next;
    }
}

void set_g() {
    if(!is_g) {
        signal_to_process(SIGUSR2, 0);
    }

    is_g = true;
}
