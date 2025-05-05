// compile with: gcc -o output mc1.c  
// run with: ./output
// Results will be saved in results.txt
// test.csv is a testing csv to see if everything works ok... i mean 73x73
// constraints.csv is the one that will be used in the algorithm and it is 72x72 as our courses...
// We must implement it as Stergiou said
// constraints types are (0,1,2,3,4)
// 0 = no constraint
// 1 = Xi != Xj
// 2 = Xi / 3 != Xj / 3
// 3 = abs(Xi / 3 - Xj / 3) > 2
// 4 = (Xi / 3 == Xj / 3 && Xi % 3 < Xj % 3)
// Do not care about what gets printed at output(just debugging)...check only .txt
// min-conflicts with random walk and random restarts
// Random restarts are about to be done here...at a new version upload 

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <string.h>

// Functions signature
void readConstraintsMatrix(const char *filename, int constraints[73][73]);
int satisfies(int *Xvalue, int numberofvariables, int numberofvalues);
int RandomVariableConflict(int *Xvalue, int numberofvariables, int numberofvalues);
int AlternativeAssignment(int *Xvalue, int numberofvariables, int variable, int numberofvalues);
void minConflicts(int maxTries, int maxChanges, int *Xvalue, int numberofvariables, int numberofvalues, FILE *outputFile, int *moves, int *bestCollisions, double p);

int main()
{
    int maxTries = 1;
    int maxChanges = 10;
    int numberofvariables = 73;
    int days = 10;
    int Xvalue[numberofvariables]; // X1, X2, ..., X70...values...Practically X1, X2, ..., X70
    int numberofvalues = days * 3; // Timeslots = days * 3
    int PrecedureRestarts = 2; // The whole procedure restarts

    FILE *outputFile = fopen("SECOND.txt", "w"); // Open file to save results
    if (outputFile == NULL)
    {
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

    for (int RestartsCounter = 0; RestartsCounter < PrecedureRestarts; RestartsCounter++)
    {
        int Xvalue[numberofvariables];
        int moves = 0;
        int bestCollisions = INT_MAX;

        // A := initial complete assignment of the variables in Problem
        for (int i = 0; i < numberofvariables; i++)
        {
            Xvalue[i] = rand() % numberofvalues + 1;
        }
        // Print initial assignment
        fprintf(outputFile, "INITIAL ASSIGNMENT:\n");
        for (int i = 0; i < numberofvariables; i++)
        {
            fprintf(outputFile, "X%d = %d\n", i, Xvalue[i]);
        }

        fprintf(outputFile, "RUN %d:\n", RestartsCounter);

        // Measure execution time
        clock_t start = clock();
        double p = 0.3; // e.g p = 0.2 = 20% probability for random walk
        minConflicts(maxTries, maxChanges, Xvalue, numberofvariables, numberofvalues, outputFile, &moves, &bestCollisions, p);
        clock_t end = clock();

        double executionTime = (double)(end - start) / CLOCKS_PER_SEC;

        fprintf(outputFile, "Execution Time: %.6f seconds\n", executionTime);
        fprintf(outputFile, "Moves: %d\n", moves);
        fprintf(outputFile, "Best Collisions: %d\n", bestCollisions);
        fprintf(outputFile, "----------------------------------------------\n");

        if (bestCollisions == 0)
        {
            SolutionsRate++;
        }

        TotalMoves += moves;
        totalBestCollisions += bestCollisions;
        TotalExecutionTime += executionTime;
    }

    double AverageMoves = (double)TotalMoves / PrecedureRestarts;
    double AverageBestCollisions = (double)totalBestCollisions / PrecedureRestarts;
    double avgExecutionTime = TotalExecutionTime / PrecedureRestarts;

    // Print statistics
    fprintf(outputFile, "\nSUMMARY:\n");
    fprintf(outputFile, "----------------------------------------------\n");
    fprintf(outputFile, "SOLUTIONS RATE: %d/%d\n", SolutionsRate, PrecedureRestarts);
    fprintf(outputFile, "AVERAGE MOVES: %.2f\n", AverageMoves);
    fprintf(outputFile, "AVERAGE BEST COLLISIONS: %.2f\n", AverageBestCollisions);
    fprintf(outputFile, "AVERAGE EXECUTION TIME: %.6f SECONDS\n", avgExecutionTime);
    fprintf(outputFile, "----------------------------------------------\n");

    fclose(outputFile);
    printf("RESULTS SAVED TO SECOND.txt\n");

    return 0;
}

// Read from CSV file
void readConstraintsMatrix(const char *filename, int constraints[73][73])
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("ERROR OPENING CSV FILE.\n");
        exit(1);
    }

    char buffer[2048];
    int row = 0;

    while (fgets(buffer, sizeof(buffer), file))
    {
        char *token = strtok(buffer, ",");
        int col = 0;

        while (token != NULL)
        {
            constraints[row][col] = atoi(token);
            token = strtok(NULL, ",");
            col++;
        }
        row++;
    }

    fclose(file);
}

// Function to check if constraints are satisfied
int satisfies(int *Xvalue, int numberofvariables, int numberofvalues)
{
    int conflicts = 0;
    int constraints[73][73] = {0}; // Adjust size to 73x73

    // Read the matrix from the CSV 
    readConstraintsMatrix("BetterCSVview.csv", constraints);

    // Check constraints...The four types of constraints we have 
    for (int i = 0; i < numberofvariables; i++)
    {
        for (int j = i+1; j < numberofvariables; j++)
        {

            int constraint = constraints[i][j];
            //printf("Checking constraint between X%d and X%d: %d\n", i, j, constraint);

            if (constraint == 1)
            {
                // Xi != Xj
                if (Xvalue[i] == Xvalue[j])
                {
                    //printf("Conflict: X%d == X%d\n", i, j);
                    conflicts++;
                }
            }
            else if (constraint == 2)
            {
                // abs(Xi / 3 - Xj / 3) > 2
                int diff = abs((Xvalue[i] / 3) - (Xvalue[j] / 3));
                if (diff < 2)
                {
                    //printf("Conflict: abs(X%d / 3 - X%d / 3) = %d <= 6\n", i, j, diff);
                    conflicts++;
                }
            }
            else if (constraint == 3)
            {
                // Xi / 3 != Xj / 3
                if ((Xvalue[i] / 3) == (Xvalue[j] / 3))
                {
                    //printf("Conflict: X%d / 3 == X%d / 3\n", i, j);
                    conflicts++;
                }
            }
            else if (constraint == 4)
            {
                // (Xi / 3 == Xj / 3 && Xi % 3 < Xj % 3)
                if ((Xvalue[i] / 3 == Xvalue[j] / 3) && (Xvalue[i] % 3 >= Xvalue[j] % 3))
                {
                    //printf("Conflict: X%d / 3 == X%d / 3 && X%d %% 3 >= X%d %% 3\n", i, j, i, j);
                    conflicts++;
                }
            }
        }
    }

    return conflicts; // Total number of conflicts
}

// Function for random variable with conflicts
int RandomVariableConflict(int *Xvalue, int numberofvariables, int numberofvalues)
{
    int VariableWithConflicts[numberofvariables]; // Size...can be all variables
    int Counter = 0;

    for (int i = 0; i < numberofvariables; i++)
    { // Check every variable
        if (satisfies(Xvalue, numberofvariables, numberofvalues) > 0)
        {
            VariableWithConflicts[Counter] = i; // Add variable to the list
            Counter++;
        }
    }

    if (Counter == 0){ // No conflicts
        return -1;
    }

    return VariableWithConflicts[rand() % Counter]; // Return a random variable with conflict
}

// Function for alternative value
int AlternativeAssignment(int *Xvalue, int numberofvariables, int variable, int numberofvalues)
{
    int BetterValue = Xvalue[variable];
    int TotalInitialConflicts = satisfies(Xvalue, numberofvariables, numberofvalues); // Number of total initial conflicts

    for (int NewValue = 0; NewValue < numberofvalues; NewValue++)
    { // Assign every value to every variable
        Xvalue[variable] = NewValue;

        // Calculate the number of conflicts with this value
        int NewConflicts = satisfies(Xvalue, numberofvariables, numberofvalues);

        // Fewer conflicts
        if (NewConflicts < TotalInitialConflicts)
        {
            TotalInitialConflicts = NewConflicts;
            BetterValue = NewValue;
        }
    }

    Xvalue[variable] = BetterValue;

    return BetterValue; // Return the alternative assignment of x which satisfies the maximum number of constraints
}

void minConflicts(int maxTries, int maxChanges, int *Xvalue, int numberofvariables, int numberofvalues, FILE *outputFile, int *moves, int *bestCollisions, double p)
{
        for (int j = 0; j < maxChanges; j++) // maxChanges
        {
            (*moves)++;

            // Calculate cost
            int currentCost = satisfies(Xvalue, numberofvariables, numberofvalues);
            fprintf(outputFile, "Change %d: Cost = %d\n", j, currentCost);

            if (currentCost < *bestCollisions)
            {
                *bestCollisions = currentCost;
            }

            // If A satisfies P, return A
            if (currentCost == 0)
            {
                fprintf(outputFile, "SOLUTION FOUND:\n");
                for (int k = 0; k < numberofvariables; k++)
                {
                    fprintf(outputFile, "X%d = %d\n", k + 1, Xvalue[k]);
                }
                return; // Solution found
            }

            // x := randomly chosen variable whose assignment is in conflict
            int x = RandomVariableConflict(Xvalue, numberofvariables, numberofvalues);

            int newAssignment;
            int randomNumber = rand() % 100 + 1; // Random number between 1 and 100
            fprintf(outputFile,"(Random Number: %d)\n", randomNumber);
            if (randomNumber <= (int)(p * 100)) // if probability p verified (e.g i give 10%...if randomNumber <= 10 then p is verified) 
            {
                // (x,a) := randomly chosen alternative assignment of x
                newAssignment = rand() % numberofvalues;
                // fprintf(outputFile, "(x,a) := randomly chosen alternative assignment of x\n"); // debugging...will be removed
                fprintf(outputFile, "X%d new random value is: %d\n", x + 1, newAssignment);
            }
            else
            {
                // (x,a) := the alternative assignment of x which satisfies the maximum number of constraints under the current assignment A
                newAssignment = AlternativeAssignment(Xvalue, numberofvariables, x, numberofvalues);
                // fprintf(outputFile, "(x,a) := the alternative assignment of x which satisfies the maximum number of constraints under the current assignment A\n"); // debugging...will be removed
                fprintf(outputFile, "X%d better value is: %d\n", x + 1, newAssignment);
            }

            // make the assignment (x, a)
            Xvalue[x] = newAssignment;
        }

    fprintf(outputFile, "NO SOLUTION FOUND.\n");
}