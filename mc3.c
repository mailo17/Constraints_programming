
// compile with: gcc -o output mc3.c
// run with: ./output
// Results will be saved in results.txt
// test.csv is a testing csv to see if everything works ok... i mean 73x73
// constraints.csv is the one that will be used in the algorithm and it is 73x73
// as our courses... We must implement it as Stergiou said constraints types are
// (0,1,2,3,4) 0 = no constraint 1 = Xi != Xj 2 = Xi / 3 != Xj / 3 3 = abs(Xi /
// 3 - Xj / 3) > 6 4 = (Xi / 3 == Xj / 3 && Xi % 3 < Xj % 3) Do not care about
// what gets printed at output(just debugging)...check only .txt

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TABU_SIZE 10 // ADJUSTABLE
typedef struct {
  int variable; // X
  int value;    // a

} TabuEntry;
typedef struct {
  TabuEntry entries[TABU_SIZE];
  int front;
  int rear;
  int count;
} TabuQueue;
//
void addToTabuList(TabuQueue *queue, int value, int variable);
void initTabuQ(TabuQueue *queue);
int isInTabuList(TabuQueue *queue, int variable, int value);
//
int main(int argc, char *argv[]) {
  int days;
  int maxTries = 1;
  int maxChanges = 10;
  int numberofvariables = 5;
  int Xvalue[numberofvariables]; // X1, X2, ..., X70...values...Practically X1,
  // X2, ..., X70
  int numberofvalues = days * 3; // Timeslots = days * 3
  int PrecedureRestarts = 2;
  FILE *outputFile = fopen("THIRD.txt", "w"); // Open file to save results
  if (outputFile == NULL) {
    printf("ERROR OPENING TXT FILE.\n");
    return 1;
  }

  fprintf(outputFile, "RUN RESULTS:\n");
  fprintf(outputFile, "----------------------------------------------\n");

  // Random
  srand(time(NULL));

  int SolutionsRate = 0;
  int TotalMoves = 0;
  int totalBestCollisions = 0;
  double TotalExecutionTime = 0.0;
  for (int i = 0; i < numberofvariables; i++) {
    Xvalue[i] = rand() % numberofvalues + 1;
  }
  fprintf(outputFile, "INITIAL ASSIGNMENT : \n");
  for (int i = 0; i < numberofvariables; i++) {
    fprintf(outputFile, "X%d = %d \n", i, Xvalue[i]);
  }

  return EXIT_SUCCESS;
}
void initTabuQ(TabuQueue *queue) {
  queue->front = 0;
  queue->rear = -1;
  queue->count = 0;
} // ELEGXOS AN YPARXEI SOT TABU LIST
int isInTabuList(TabuQueue *queue, int variable, int value) {
  for (int i = 0; i < queue->count; i++) {
    int idx = (queue->front + i) % TABU_SIZE;
    if (queue->entries[idx].variable == variable &&
        queue->entries[idx].value == value)
      return 1;
  }
  return 0;
}
// ADDING TO TABUE LIST
void addToTabuList(TabuQueue *queue, int value, int variable) {
  if (queue->count == TABU_SIZE) {
    queue->front = (queue->front + 1) % TABU_SIZE;
    queue->count--;
  }
  queue->rear = (queue->rear + 1) % TABU_SIZE;
  queue->entries[queue->rear].variable = variable;
  queue->entries[queue->rear].value = value;
  queue->count++;
}
