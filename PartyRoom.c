#include <stdio.h>
#include <pthread.h>

#define dean_waiting 0
#define dean_in_the_room 1
#define dean_left 2

int number_of_students;
int dean_status;

pthread_mutex_t mutex;
pthread_cond_t students_leaving, students_comming;

void* students_thread(void* ptr);
void* dean_thread(void* ptr);

int main(void) {

    number_of_students = 0;
    dean_status = -1;

    pthread_t dean, students;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&students_comming, NULL);
    pthread_cond_init(&students_leaving, NULL);

    pthread_create(&students, NULL, students_thread, NULL);
    pthread_create(&dean, NULL, dean_thread, NULL);

    pthread_join(dean, NULL);
    pthread_join(students, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&students_comming);
    pthread_cond_destroy(&students_leaving);

    return 0;
}

void* dean_thread(void* ptr) {

    pthread_mutex_lock(&mutex);

    if (number_of_students > 0 && number_of_students < 50) {
        dean_status = dean_waiting;
        printf("Dean is waiting\n");
        pthread_cond_wait(&students_comming, &mutex);
    }

    if (number_of_students >= 50) {
        dean_status = dean_in_the_room;
        printf("Dean enters the room\n");
        pthread_cond_wait(&students_leaving, &mutex);
    }
    else {
        printf("Dean is searching the room\n");
        dean_status = dean_in_the_room;
    }

    dean_status = dean_left;
    printf("Dean left the room\n");
    pthread_mutex_unlock(&mutex);
}


void* students_thread(void* ptr) {

    while (1) {

        pthread_mutex_lock(&mutex);

        if (number_of_students < 50 && dean_status != dean_in_the_room) {
            printf("Students are having a party\n");
            number_of_students++;
        }

        if (dean_status == dean_waiting && number_of_students == 50) {
            pthread_cond_signal(&students_comming);
        }

        if (dean_status == dean_in_the_room) {
            if (number_of_students > 0) {
                printf("Students are leaving\n");
                number_of_students--;
            }
            else {
                pthread_cond_signal(&students_leaving);
                pthread_mutex_unlock(&mutex);
                printf("End of the party\n");
                break;
            }
        }
        if (dean_status == dean_left && number_of_students == 50) {
            printf("Students are having a party all night long\n");
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
}