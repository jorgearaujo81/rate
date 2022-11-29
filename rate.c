#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MAX 99
#define FULL 'F'
#define HOLD 'H'
#define LOST 'L'
#define KILLED 'K'

struct stTask
{
    char name[MAX];
    int period;
    int cpu_burst;
    int lost_deadline;
    int complete_execution;
    int miss;
    int hold;
    int time;
};

FILE *file_read, *file_write;

void quicksort(struct stTask *task, int , int);

void main (int argc, char *argv[])
{
    char character[MAX];
    char *line;
    int number_tasks = 0;
    int TOTAL_TIME;
    
    if(argc < 2){
        fprintf(stderr, "required to enter the file name\n");
        exit(EXIT_FAILURE);

    }else if (argc > 2)
    {
        fprintf(stderr, "too many arguments\n");
        exit(EXIT_FAILURE);
    }
    
    file_read = fopen(argv[1], "r+b");

    if(file_read == NULL)
    {
        fprintf(stderr, "file cannot be opened\n");
        exit(EXIT_FAILURE);
    }
    
    while (!feof(file_read))
    {
        line = fgets(character, MAX, file_read);

        if(!number_tasks)
        {
            TOTAL_TIME = atoi(line);
        }
        number_tasks++;
    }

    number_tasks--;
    
    struct stTask task[number_tasks];

    fseek(file_read,0,SEEK_SET);

    int index = 0;

    while (!feof(file_read))
    {
        line = fgets(character, MAX, file_read);
        if(index)
        {
            strcpy(task[index-1].name, strtok(line, " "));
            task[index-1].period = atoi(strtok(NULL, " "));
            task[index-1].cpu_burst = atoi(strtok(NULL, " "));
            task[index-1].lost_deadline = 0;
            task[index-1].complete_execution = 0;
            task[index-1].miss = 0;
            task[index-1].hold = 0;
            task[index-1].time = 0;
        }
        
        index++;
    }
    fclose(file_read);
    
    quicksort(task, 0, number_tasks-1);

    int task_queue = 0;
    int time_idle = 0;
    int current = number_tasks - 1;
    int next = 0;
    int time_hold = 0;

    printf("EXECUTION BY RATE\n");
    for (int time = 0; time <= TOTAL_TIME; time++)
    {
        for (int i = number_tasks-1; i >=0 ; i--)
        {
            if (time == 0)
            {
                task[i].miss = task[i].cpu_burst;
                task[i].hold = task[i].cpu_burst;

                task_queue++;

                if (task[i].period != 0)
                {
                    current = i;
                    next = current;
                }
            }
            else
            {
                if (time % task[i].period == 0)
                {
                    if (task[current].miss > 0 && task[i].period < task[current].period)
                    {
                        task[current].time = task[current].hold - task[current].miss;
                        task[current].hold = task[current].miss;

                        next = i;
                    }
                    else if (task[current].miss == 0 && task[i].miss != 0)
                    {
                        task_queue--;
                        next = i;
                    }else if (task_queue == 0)
                    {
                        next = i;

                    }
                }
                else
                {
                    if(task[current].miss == 0 && task_queue != 0)
                    {
                        if (task[i].miss != 0)
                            next = i;
                    }   
                }           
            }
        }
        
        //printf("%s %d %d %d\n", task[current].name, task[current].miss, task_queue ,time);

        if ((time % task[next].period == 0 || time == TOTAL_TIME) && task_queue == 0)
            printf("idle for %d units\n", time_idle);
        else if (task[current].miss == 0 && task_queue != 0 && time <= TOTAL_TIME)
            printf("[%s] for %d units - %c\n", task[current].name, task[current].hold - task[current].miss, FULL);
        else if (time % task[next].period == 0 && task[next].period < task[current].period && time < TOTAL_TIME)
            printf("[%s] for %d units - %c\n", task[current].name, task[current].time, HOLD);
        else if (time % task[current].period == 0 && task[current].miss < task[current].cpu_burst && time < TOTAL_TIME)
            printf("[%s] for %d units - %c\n", task[current].name, task[current].miss, LOST);
        else if(task[current].cpu_burst - task[current].miss > 0 && time == TOTAL_TIME && task_queue != 0)
            printf("[%s] for %d units - %c\n", task[current].name, task[current].hold - task[current].miss, KILLED);

        if(task[current].miss == 0 && task_queue != 0)
        {
            task[current].complete_execution++;
            task_queue--;
        }

        for (int i = number_tasks - 1; i >= 0 ; i--)
        {
            if (time % task[i].period == 0 && time != 0)
            {
                task_queue++;

                if(task[i].period < task[current].period && task[i].miss != 0)
                    next = i;
                else if (task[i].miss < task[current].cpu_burst && task[current].miss != 0 && i == current)
                {
                    task[current].lost_deadline++;
                    task_queue--;
                    next = i;
                }
                task[i].miss = task[i].cpu_burst;
                task[i].hold = task[i].cpu_burst;
            }
        }

        current = next;

        if(!task_queue)
            time_idle++;
        else
        {
            time_idle = 0;
            task[current].miss--;
        }

       
    }

    fprintf(file_write,"\nLOST DEADLINES\n");
    for(int i = 0; i < number_tasks; i++)
        fprintf(file_write,"[%s] %d\n", task[i].name, task[i].lost_deadline);

    fprintf(file_write,"\nCOMPLETE EXECUTION\n");
    for(int i = 0; i < number_tasks; i++)
        fprintf(file_write, "[%s] %d\n", task[i].name, task[i].complete_execution);

    fprintf(file_write,"\nKILLED\n");
    for(int i = 0; i < number_tasks; i++){
        if(i != number_tasks - 1)
        {
            if(task[i].miss > 0)
                fprintf(file_write,"[%s] %d\n", task[i].name, 1);
            else
                fprintf(file_write,"[%s] %d\n", task[i].name, 0);
        }
        else
        {
            if(task[i].miss > 0)
                fprintf(file_write,"[%s] %d", task[i].name, 1);
            else
                fprintf(file_write,"[%s] %d", task[i].name, 0);
        }
    }

    fclose(file_write);
}

void quicksort(struct stTask *task, int first, int last)
{
    register int indexFirst, indexLast;
    int tempFirst, tempLast;
    char temp[MAX];

    indexFirst = first;
    indexLast = last;
    tempFirst = task[(first+last)/2].period;

    do{
        while(task[indexFirst].period < tempFirst && indexFirst < last) indexFirst++;

        while(tempFirst < task[indexLast].period && indexLast > first) indexLast--;

        if(indexFirst <= indexLast)
        {

            tempLast = task[indexFirst].period;
            task[indexFirst].period = task[indexLast].period;
            task[indexLast].period = tempLast;

            strcpy(temp, task[indexFirst].name);
            strcpy(task[indexFirst].name , task[indexLast].name);
            strcpy(task[indexLast].name , temp);

            tempLast = task[indexFirst].cpu_burst;
            task[indexFirst].cpu_burst = task[indexLast].cpu_burst;
            task[indexLast].cpu_burst = tempLast;

            indexFirst++;
            indexLast--;
        }
    }while(indexFirst <= indexLast);

    if(first < indexLast) quicksort(task, first, indexLast);

    if(indexFirst < last) quicksort(task, indexFirst, last);
}
