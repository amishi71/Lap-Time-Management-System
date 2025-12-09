/* INCLUDES */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* MACROS */
#define MAX_NAME_LEN 50

/* DATATYPES */
typedef struct
{
    float *sector_times;
    int sector_count;
} Lap;

typedef struct
{
    char name[MAX_NAME_LEN];
    Lap lap;
} Racer;

typedef struct node
{
    Racer *racer;
    struct node *next;
} node;

/* GLOBAL VARIABLES */
node *front = NULL;
node *rear = NULL;

/* FUNCTION PROTOTYPES */
node *Create(char *name, float *sector_times, int sector_count);
void Enqueue(char *name, float *sector_times, int sector_count);
int Dequeue();
void Display();
void Destroy();
float AvgLapTime();
void FastestLap();
void CompareLapTimes(char *racer1, char *r2);

/* MAIN */
int main()
{
    while (1)
    {
        int choice;
        char name[MAX_NAME_LEN];

        printf("\n<<<<<<<< Lap Time Manager >>>>>>>>\n");
        printf("1. Add Lap Time\n");
        printf("2. Display All Lap Times\n");
        printf("3. Show Average Lap Time\n");
        printf("4. Show Fastest Lap\n");
        printf("5. Compare Lap Times\n");
        printf("6. Remove Oldest Lap Time\n");
        printf("0. Exit\n");
        printf("Enter choice (0-6): ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            return 0;
        }
        getchar();

        switch (choice)
        {
        case 1:
            printf("Enter racer name: ");
            if (!fgets(name, MAX_NAME_LEN, stdin)) {
                printf("Input error.\n");
                break;
            }
            name[strcspn(name, "\n")] = '\0';

            {
                int sector_count;
                printf("Enter the number of sectors for this lap: ");
                if (scanf("%d", &sector_count) != 1 || sector_count <= 0)
                {
                    printf("Invalid number of sectors.\n");
                    while (getchar() != '\n')
                        ;
                    break;
                }
                while (getchar() != '\n')
                    ;

                float *sector_times = malloc(sector_count * sizeof(float));
                if (!sector_times)
                {
                    printf("Memory allocation failed!\n");
                    break;
                }

                for (int i = 0; i < sector_count; i++)
                {
                    printf("Enter time for sector %d: ", i + 1);
                    if (scanf("%f", &sector_times[i]) != 1)
                    {
                        printf("Invalid input for sector time.\n");
                        sector_times[i] = 0.0f;
                        while (getchar() != '\n')
                            ;
                    }
                    while (getchar() != '\n')
                        ;
                }

                Enqueue(name, sector_times, sector_count);
                free(sector_times);
            }
            break;

        case 2:
            Display();
            break;

        case 3:
            printf("Average Track Lap Time: %.2f seconds\n", AvgLapTime());
            break;

        case 4:
            FastestLap();
            break;
        case 5:
        {
            char raceracer1[MAX_NAME_LEN];
            char racer2[MAX_NAME_LEN];
            printf("Enter the name of the first racer: ");
            if (!fgets(raceracer1, MAX_NAME_LEN, stdin)) { printf("Input error.\n"); break; }
            raceracer1[strcspn(raceracer1, "\n")] = '\0';
            printf("Enter the name of the second racer: ");
            if (!fgets(racer2, MAX_NAME_LEN, stdin)) { printf("Input error.\n"); break; }
            racer2[strcspn(racer2, "\n")] = '\0';
            CompareLapTimes(raceracer1, racer2);
        }
        break;

        case 6:
        {
            int result = Dequeue();
            if (result == -1)
                printf("Queue is empty!\n");
            else
                printf("Removed successfully.\n");
        }
        break;

        case 0:
            printf("Exiting...\n");
            Destroy();
            return 0;

        default:
            printf("Invalid option entered, please try again\n");
        }
    }
}

/* FUNCTION IMPLEMENTATIONS */

/// @brief Creates a new node for the queue
/// @param name
/// @param sector_times
/// @param sector_count
/// @return Pointer to the newly created node
node *Create(char *name, float *sector_times, int sector_count)
{
    node *new_node = (node *)malloc(sizeof(node));
    if (!new_node) { return NULL; }
    new_node->racer = (Racer *)malloc(sizeof(Racer));
    if (!new_node->racer) { free(new_node); return NULL; }
    strncpy(new_node->racer->name, name, MAX_NAME_LEN - 1);
    new_node->racer->name[MAX_NAME_LEN - 1] = '\0';
    Lap *lap = (Lap *)malloc(sizeof(Lap));
    if (!lap) { free(new_node->racer); free(new_node); return NULL; }
    lap->sector_times = malloc(sector_count * sizeof(float));
    if (!lap->sector_times) { free(lap); free(new_node->racer); free(new_node); return NULL; }
    for (int i = 0; i < sector_count; i++)
    {
        lap->sector_times[i] = sector_times[i];
    }
    lap->sector_count = sector_count;
    new_node->racer->lap = *lap;
    free(lap);
    new_node->next = NULL;
    return new_node;
}

/// @brief Adds a lap time to the queue
/// @param name
/// @param sector_times
/// @param sector_count
void Enqueue(char *name, float *sector_times, int sector_count)
{
    node *new_node = Create(name, sector_times, sector_count);
    if (!new_node) { printf("Failed to allocate node.\n"); return; }

    if (front == NULL && rear == NULL) // If the queue was empty
    {
        front = rear = new_node;
    }
    else // Normal case
    {
        rear->next = new_node;
        rear = new_node;
    }
}

/// @brief Removes the oldest lap time from the queue
/// @return 0 if successful, -1 if queue is empty
int Dequeue()
{
    if (front == NULL) // If queue was empty
    {
        return -1;
    }

    struct node *ptr = front;
    // Free sector_times array
    if (ptr->racer && ptr->racer->lap.sector_times)
        free(ptr->racer->lap.sector_times);
    // Free Racer struct
    if (ptr->racer)
        free(ptr->racer);

    if (front == rear) // If only one node was present
    {
        front = rear = NULL;
        free(ptr);
        return 0;
    }
    else // Normal case
    {
        front = front->next;
        free(ptr);
        return 0;
    }
}

/// @brief Displays all lap times in the queue
void Display()
{
    if (front == NULL) // If queue is empty
    {
        printf("Queue is empty!\n");
        return;
    }

    printf("\n========================== RACETRACK SCOREBOARD ==========================\n");
    printf("| %-20s | %-12s | %-10s | %-20s |\n", "Racer Name", "Total Time", "Sectors", "Sector Times");
    printf("--------------------------------------------------------------------------\n");
    struct node *ptr = front;
    while (ptr != NULL)
    {
        float lap_time = 0.0f;
        for (int i = 0; i < ptr->racer->lap.sector_count; i++)
        {
            lap_time += ptr->racer->lap.sector_times[i];
        }
        printf("| %-20s | %10.2f s | %-10d | ", ptr->racer->name, lap_time, ptr->racer->lap.sector_count);
        for (int i = 0; i < ptr->racer->lap.sector_count; i++)
        {
            printf("%6.2f ", ptr->racer->lap.sector_times[i]);
        }
        printf("|");
        printf("\n--------------------------------------------------------------------------\n");
        ptr = ptr->next;
    }
}

/// @brief Destroys the entire queue and frees memory
void Destroy()
{
    if (front == NULL) // If queue is empty
    {
        printf("Queue is empty!\n");
        return;
    }
    while (front != NULL) // Recursively dequeuing all nodes
        Dequeue();
    printf("Queue destroyed.\n");
}

/// @brief Calculates the average lap time across all racers
/// @return Average lap time
float AvgLapTime()
{
    if (front == NULL) // If queue is empty
    {
        printf("Queue is empty!\n");
        return 0.0f;
    }

    float total_time = 0.0f;
    int lap_count = 0;
    struct node *ptr = front;

    while (ptr != NULL)
    {
        float lap_time = 0.0f;
        for (int i = 0; i < ptr->racer->lap.sector_count; i++)
        {
            lap_time += ptr->racer->lap.sector_times[i]; // Summing up all sector times
        }
        total_time += lap_time; // Summing up all lap times
        lap_count++;
        ptr = ptr->next;
    }

    return lap_count ? total_time / lap_count : 0.0f; // If more than 0 laps, return average
}

/// @brief Finds the fastest lap across all times
void FastestLap()
{
    if (front == NULL) // If queue is empty
    {
        printf("Queue is empty!\n");
        return;
    }

    float min_time = -1.0f;
    char best_name[MAX_NAME_LEN] = "";
    struct node *ptr = front;

    while (ptr != NULL)
    {
        float lap_time = 0.0f;
        for (int i = 0; i < ptr->racer->lap.sector_count; i++)
        {
            lap_time += ptr->racer->lap.sector_times[i]; // Summing up all sector times
        }

        if (min_time == -1.0f || lap_time < min_time) // Finding minimum lap time
        {
            min_time = lap_time;
            strncpy(best_name, ptr->racer->name, MAX_NAME_LEN - 1);
            best_name[MAX_NAME_LEN - 1] = '\0';
        }

        ptr = ptr->next;
    }

    printf("Fastest Lap: %s with %.2f seconds\n", best_name, min_time);
}

/// @brief Compares average lap times between two racers
/// @param racer1
/// @param racer2
void CompareLapTimes(char *racer1, char *racer2)
{
    node *temp = front;
    float sum1 = 0.0f, sum2 = 0.0f;
    int count1 = 0, count2 = 0;

    // Traversing the queue to find all lap times for both racers
    while (temp != NULL)
    {
        if (strcmp(temp->racer->name, racer1) == 0)
        {
            float lap_time = 0.0f;
            for (int i = 0; i < temp->racer->lap.sector_count; i++)
                lap_time += temp->racer->lap.sector_times[i];
            sum1 += lap_time;
            count1++;
        }
        if (strcmp(temp->racer->name, racer2) == 0)
        {
            float lap_time = 0.0f;
            for (int i = 0; i < temp->racer->lap.sector_count; i++)
                lap_time += temp->racer->lap.sector_times[i];
            sum2 += lap_time;
            count2++;
        }
        temp = temp->next;
    }

    // If either racer was not found
    if (count1 == 0)
    {
        printf("%s not found.\n", racer1);
        return;
    }
    if (count2 == 0)
    {
        printf("%s not found.\n", racer2);
        return;
    }

    // Calculating average lap times
    float avg_time1 = sum1 / count1;
    float avg_time2 = sum2 / count2;

    printf("Comparison of average lap times between %s and %s:\n", racer1, racer2);
    printf("%s: %.2f seconds (average over %d lap(s))\n", racer1, avg_time1, count1);
    printf("%s: %.2f seconds (average over %d lap(s))\n", racer2, avg_time2, count2);
    if (avg_time1 < avg_time2)
        printf("%s is faster by %.2f seconds\n", racer1, avg_time2 - avg_time1);
    else if (avg_time1 > avg_time2)
        printf("%s is faster by %.2f seconds\n", racer2, avg_time1 - avg_time2);
    else
        printf("Both racers have the same average time of %.2f seconds\n", avg_time1);
}
