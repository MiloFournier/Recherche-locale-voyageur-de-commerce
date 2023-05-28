#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int iseed = 1;

int nextRand(int n){
    int i = 16807 * (iseed % 127773) - 2836 * (iseed / 127773);
    if (i > 0) iseed = i;
    else iseed = 2147483647 + i;
    return iseed % n;
}

int** createCost(int n, FILE* fd){
    int x[n], y[n];
    int max = 1000;
    int** cost;
    int iseed = 1;
    cost = (int**) malloc(n*sizeof(int*));
    fprintf(fd, "import turtle\n");
    fprintf(fd, "turtle.setworldcoordinates(0, 0, %d, %d)\n", max, max+100);
    for (int i=0; i<n; i++){
        x[i] = nextRand(max);
        y[i] = nextRand(max);
        fprintf(fd, "p%d=(%d,%d)\n", i, x[i], y[i]);
        cost[i] = (int*)malloc(n*sizeof(int));
    }
    for (int i=0; i<n; i++){
        cost[i][i] = max*max;
        for (int j=i+1; j<n; j++){
            cost[i][j] = (int)sqrt((x[i]-x[j])*(x[i]-x[j]) + (y[i]-y[j])*(y[i]-y[j]));
            cost[j][i] = cost[i][j];
        }
    }
    return cost;
}

int generateRandomTour(int n, int** cost, int seed, int* sol){
    int cand[n];
    for (int i=0; i<n; i++) cand[i] = i;
    sol[0] = nextRand(n);
    cand[sol[0]] = n-1;
    int total = 0;
    int nbCand = n-1;
    for (int i=1; i<n; i++){
        int j = nextRand(nbCand);
        sol[i] = cand[j];
        cand[j] = cand[--nbCand];
        total += cost[sol[i-1]][sol[i]];
    }
    total += cost[sol[n-1]][sol[0]];
    return *sol;
}

void print(int* sol, int n, int totalLength, FILE* fd){
    fprintf(fd, "turtle.clear()\n");
    fprintf(fd, "turtle.tracer(0,0)\n");
    fprintf(fd, "turtle.penup()\n");
    fprintf(fd, "turtle.goto(0,%d)\n", 1050);
    fprintf(fd, "turtle.write(\"Total length = %d\")\n", totalLength);
    fprintf(fd, "turtle.speed(0)\n");
    fprintf(fd, "turtle.goto(p%d)\n", sol[0]);
    fprintf(fd, "turtle.pendown()\n");
    for (int i=1; i<n; i++) fprintf(fd, "turtle.goto(p%d)\n", sol[i]);
    fprintf(fd, "turtle.goto(p%d)\n", sol[0]);
    fprintf(fd, "turtle.update()\n");
    fprintf(fd, "wait = input(\"Enter return to continue\")\n");
}

int greedyLS(int n, int* sol, int total, int** cost){
    int condition = 0;
    while(condition == 0) {
        int max = 0;
        int vi;
        int vj;
        for(int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n - 1; ++j) {
                int c = - cost[sol[i]][sol[j]] - cost[sol[i+1]][sol[j+1]] + cost[sol[i]][sol[i+1]] + cost[sol[j]][sol[j+1]];
                if(c > 0) {
                    if(c > max) {
                        max = c;
                        vi = i;
                        vj = j;
                    }
                }
            }
        }
        if(max == 0) {
            condition = 1;
        } else {
            int temp = sol[vj];
            sol[vj] = sol[vi+1];
            sol[vi+1] = temp;
        }
    }
    total = 0;
    for(int i = 0; i < n-1; ++i) {
        total += cost[sol[i]][sol[i+1]];
    }
    return total;
}

int *ILS(int **cost, int k, int l, int n) {
    int *sol = (int *)malloc(n*sizeof(int));
    clock_t t = clock();
    int initial = generateRandomTour(n, cost, 0, sol);
    int lSol = greedyLS(n, sol, 0, cost);
    printf("Initial tour length = %d; ", initial);
    float d = ((double) (clock() - t)) / CLOCKS_PER_SEC;
    printf("Tour length after GreedyLS = %d; CPU time = %.3fs\n", lSol, d);

    for(int i = 1; i < k; ++i) {
        int *c = sol;
        for(int j = 1; j < l; ++j) {
            int r1 = nextRand(n);
            int r2 = nextRand(n);
            int temp = c[r1];
            c[r1] = c[r2];
            c[r2] = temp;
        }
        int lC = greedyLS(n, c, 0, cost);
        if(lC < lSol) {
            
            float d = ((double) (clock() - t)) / CLOCKS_PER_SEC;
            printf("New best found at iteration %d; Tout length = %d; Time = %.3fs\n", i, lC, d);
            sol = c;
            lSol = lC;
        }
    }
    return sol;
}

/*int main() { //this main implements the ILS
    int k;
    printf("Number of iterations of ILS (k): "); fflush(stdout);
    scanf("%d",&k);
    int l;
    printf("Perturbation strength (l): "); fflush(stdout);
    scanf("%d",&l);
    int n;
    printf("Number of vertices: "); fflush(stdout);
    scanf("%d",&n);
    int nbTrials;

    FILE* fd  = fopen("script.py", "w");
    int** cost = createCost(n, fd);
    
    ILS(cost, k, l, n);
    
    return 0;
};*/

int main() { //this main "uncrosses" every vertices
    int n;
    printf("Number of vertices: "); fflush(stdout);
    scanf("%d",&n);
    int nbTrials;
    printf("Number of random tour constructions: "); fflush(stdout);
    scanf("%d",&nbTrials);
    FILE* fd  = fopen("script.py", "w");
    int** cost = createCost(n, fd);
    int sol[n];
    for (int i=0; i<nbTrials; i++){
        int total = generateRandomTour(n, cost, i, sol);
        printf("Trial %d: Initial tour length = %d; ", i, total);
        clock_t t = clock();
        total = greedyLS(n, sol, total, cost);
        float d = ((double) (clock() - t)) / CLOCKS_PER_SEC;
        printf("Tour length after GreedyLS = %d; CPU time = %.3fs\n", total, d);
        print(sol, n, total, fd);
    }
    return 0;
};
