
#include "general_func.h"
#include "main_func.h"
int NUM_MESSAGES=10;
thread* consumers_array;
thread* producers_array;
int current_num_producers =0;
int current_num_consumers =0;
struct message null_message;
struct ring_buffer* shared_buffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full;
pthread_cond_t empty;


void sigint_handler(int signal){
    delete_all_and_exit();
}

int main() {
    srand(time(NULL));
    signal(SIGINT, sigint_handler);

    struct message null_message = create_null_message();
    
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);

    shared_buffer = (struct ring_buffer*)malloc(sizeof(struct ring_buffer));
    init_buffer(shared_buffer);

    print_menu();

    char* buf = (char*)malloc(sizeof(char)*10);
    while(1){
        OPTIONS num = get_option(buf,10);
        switch (num){
            case CREATE_PRODUCER_OPTION:
                create_producer();
                break;
            case CREATE_CONSUMER_OPTION:
                create_consumer();
                break;
            case DELETE_CONSUMER_OPTION:
                delete_last_consumer();
                break;
            case DELETE_PRODUCER_OPTION:
                delete_last_producer();
                break;
            case QUEUE_STAT_OPTION:
                print_queue_stat(shared_buffer);
                break;   
            case PRODUCER_STAT_OPTION:
                print_producers_stat();
                break;     
            case CONSUMER_STAT_OPTION:
                print_consumers_stat();
                break;
            case Q_PLUS_PLUS_OPTION:
                resize_queue(1);
                break;
            case PRINT_CONSUMERS_MESSAGE_STAT:
                print_consumers_message_stat();
                break;
            case PRINT_PRODUCER_MESSAGE_STAT:
                print_producers_message_stat() ;
                break;
            case Q_MINUS_MINUS_OPTON:
                resize_queue(-1);
                break;
            case Q_OPTION:
                delete_all_and_exit();
                break;          
        }        
    }   
    // Уничтожение условных переменных
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
    return 0;
}