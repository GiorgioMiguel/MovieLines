/*
COP 3502 Assignment 3 "Movie Line"
Giorgio Torregrosa 3/5/2024
*/

//libraries
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#define totalQueues 12

//customer struct
typedef struct Attendee {
    char name[51];
    int tickets;
    int timeOfArrival;
} Attendee;

//linked list struct
typedef struct Node {
    Attendee customer;
    struct Node* next;
} Node;

//queue struct
typedef struct Queue {
    Node* front;
    Node* rear;
    int total;
} Queue;

//functions used in program
void Enqueue(Queue* queue, Attendee newAttendee);//enqueues a customer by dynamically allocating a node in a linked list
void Dequeue(Queue* queue);//dequeues and frees the associated memory for a node in a linked list
Node* Peekqueue(Queue* queue);//returns the information pertaining to the head of a particular queue
int Sizequeue(Queue* queue);//returns the size of a given queue
bool Emptyqueue(Queue* queue);//returns true if empty, false if not
void Initializequeue(Queue* newQueue);//intializes a queue
bool ProcessQueues(Queue* queues, int maxBoothIndex);//processes and prints the first person in a given booth.
void BuildQueues(Queue* Queues);//builds 1-12 linked list to hold customers of given names
void PrintandProcess(Queue* queues, int totalEmployees);//uses the preccess queue function and the total booths to print them in a neatly fashion

//global timer for easy time keeping
int globalTimer = 0;

//start main
int main(int argc, char* argv[]) {
    //creating a struct array containing 12 queue structs and intializing them along with two local variables to hold read information regarding total customers and total booths(employees)
    Queue Queues[totalQueues] = { 0 };
    for (int i = 0; i < totalQueues;i++) {
        Initializequeue(&Queues[i]);
    }
    int totalCustomers = 0;
    int totalEmployees = 0;
    if (scanf("%d%d", &totalCustomers, &totalEmployees) != 2) {
        return -1;//returning -1 if more than 2 numbers are read in initially.
    }
    //creating 1-12 linked lists containing customer nodes
    BuildQueues(Queues);

    //processing and printing this information based on time of arrival, when a node's information is printed it is also being deleted and freed. 
    PrintandProcess(Queues, totalEmployees);

    return 0;
}//end main

//This function creates a 12x3 matrix to hold 1-12 possible booth emplyees and information about them, 
//then finds the earliest time of arrival node within a given booths range to determine which node to print and dequeue
void PrintandProcess(Queue* Queues, int totalEmployees) {
    //this booth array describes 3 things about each booth: (booth #, total queues given to booth, and the maximum index in the queue array which the booth is allowed to access.
    int Booths[12][3] = { {1,0,0}, {2,0,0}, {3,0,0}, {4,0,0}, {5,0,0}, {6,0,0}, {7,0,0}, {8,0,0}, {9,0,0}, {10,0,0}, {11,0,0}, {12,0,0} };
    int totalLines = 0;
    int flag = 0;
    //determining the total non-empty queues
    for (int x = 0; x < totalQueues; x++) {
        if (Queues[x].front != NULL) {
            totalLines++;
        }
    }
    //distributing the queues based on the assignment guidelines
    int defaultLines = totalLines / totalEmployees;
    int extraLines = totalLines % totalEmployees;
    for (int i = 0; i < totalEmployees; i++) {
        Booths[i][1] = defaultLines;
    }
    for (int i = 0; i < extraLines; i++) {
        Booths[i][1]++;
    }
    //determining the maximum index that a given booth will access
    int maxIndex = 0;
    for (int i = 0; i < totalEmployees; i++) {
        flag = 0;
        while (flag < Booths[i][1]) {
            for (int j = maxIndex; j < totalQueues; j++) {
                if (Queues[j].front != NULL) {
                    flag++;
                    if (flag == Booths[i][1]) {
                        Booths[i][2] = j;
                        maxIndex = j + 1;
                        break;
                    }
                }
            }
        }
    }
    //calling PrintQueues based on how many booths and customers there are
    bool exitCon = true;
    for (int i = 0; i < totalEmployees; i++) {
        printf("\nBooth %d:\n", i + 1);
        globalTimer = 0;
        exitCon = true;
        while (exitCon != false) {
            exitCon = ProcessQueues(Queues, Booths[i][2]);
        }
    }
}

//this function is responsible for reading in information and build 1-12 linked lists to hold this information. 
//Every time a line is read a node in a linked list is created containing the information abouta customer
void BuildQueues(Queue* Queues) {
    //taking the input stream and creating 1-12 lines(queues) based on the first letter of their name
    Attendee tempAttendee = { 0 };
    while (scanf("%50s%d%d", tempAttendee.name, &tempAttendee.tickets, &tempAttendee.timeOfArrival) == 3) {

        if ((tempAttendee.name[0] % 13) - 1 >= 0) {//if there name doesn't start with an A or an N, queue them normally
            Enqueue(&Queues[(tempAttendee.name[0] % 13) - 1], tempAttendee);    
        }
        else {// if there name starts with an A or an N, queue them in the smallest nonempty qeueue from left to right
            int minIndex = 0;
            int minTotal = 0;
            //intializing minIndex and minTotal
            for (int i = 0; i < totalQueues; i++) {
                if (Queues[i].total != 0) {
                    minTotal = Queues[i].total;
                    minIndex = i;
                    break;
                }
            }
            //using minIndex and minTotal to queue customers with A or N inside the smallest queue
            for (int j = minIndex; j < totalQueues; j++) {
                if (Queues[j].total != 0 && Queues[j].total < minTotal) {
                    minTotal = Queues[j].total;
                    minIndex = j;
                }
            }
            Enqueue(&Queues[minIndex], tempAttendee);                                  
        }
    }
}
//the purpose of this function is to find the earliest person based on time of arrival within a given booths range within the queue array.
//once that person is found they are printed and dequeued the function will return true, if no person was found return false
bool ProcessQueues(Queue* queues, int maxBoothIndex) {
        bool processed = false;
        int minTimer = INT_MAX;
        int minIndex = 0;
        //initializing minTimer and minIndex
        for (int i = 0; i <= maxBoothIndex; i++) {
            if (queues[i].front != NULL) {
                if (queues[i].front->customer.timeOfArrival < minTimer) {
                    minTimer = queues[i].front->customer.timeOfArrival;
                    minIndex = i;
                }
            }
        }
        //using these two variables determine the global counter, if it should be reset or not, and the adjusts the value accordning to the processing time formula given in the assignment
        if (queues[minIndex].front != NULL) {
            if (queues[minIndex].front->customer.timeOfArrival > globalTimer) {
                globalTimer = queues[minIndex].front->customer.timeOfArrival;
            }
            globalTimer += (queues[minIndex].front->customer.tickets) * 5 + 30;
            printf("%s from line %d checks out at time %d\n", queues[minIndex].front->customer.name, minIndex + 1, globalTimer);
            Dequeue(&queues[minIndex]);
            processed = true;
        }
        //if someone was dequeued we return true, else we return false by default
        return processed;
}
//basic intialize queue function
void Initializequeue(Queue* newQueue) {
    newQueue->front = NULL;
    newQueue->rear = NULL;
    newQueue->total = 0;
}
//dynamically createts a node for a custoemrs information and links it with a given link list head pointer
void Enqueue(Queue* queue, Attendee attendee) {
    Node* newAttendee = (Node*)malloc(sizeof(Node));
    if (!newAttendee) {
        return;
    }
    newAttendee->customer = attendee;
    newAttendee->next = NULL;

    if (queue->front == NULL) {
        queue->front = queue->rear = newAttendee;
    }
    else {
        queue->rear->next = newAttendee;
        queue->rear = newAttendee;
    }
    queue->total += 1;
}
//deletes and frees a customers information
void Dequeue(Queue* queue) {
    if (queue->front == NULL) {
        return;
    }
    else {
        Node* tmp = queue->front;
        queue->front = queue->front->next;
        if (queue->front == NULL) {
            queue->rear = queue->front;
        }
        free(tmp);
        queue->total -= 1;
    }
}
//returns the node at the beginning of a linked list
Node* Peekqueue(Queue* queue) {
    return queue->front;
}
//returns the size of a given queue
int Sizequeue(Queue* queue) {
    return queue->total;
}
//returns true if the queue is empty, else return false
bool Emptyqueue(Queue* queue) {
    if (queue->total <= 0)
        return true;
    else
        return false;
}