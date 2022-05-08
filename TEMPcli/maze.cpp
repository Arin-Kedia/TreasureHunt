#include <iostream>
#include <stdio.h>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

void shuffle(int* dx, int* dy) {
    for (int i = 0; i < 3; i++) {
        int offset = (rand() % (3-i));
        int temp = dx[i];
        dx[i] = dx[i + offset];
        dx[i + offset] = temp;
        temp = dy[i];
        dy[i] = dy[i + offset];
        dy[i + offset] = temp;
    }
}

void generate(int** maze, int x, int y, int n) {
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    shuffle(dx, dy);
    for (int i = 0; i < 4; i++) {
        if (x + dx[i] * 2 >= 0 && x + dx[i] * 2 < n && y + dy[i] * 2 >= 0 
          && y + dy[i] * 2 < n && maze[x + dx[i] * 2][y + dy[i] * 2] == 1) {
              maze[x + dx[i] * 2][y + dy[i] * 2] = 0;
              maze[x + dx[i]][y + dy[i]] = 0;
              generate(maze, x + dx[i] * 2, y + dy[i] * 2, n);
        }
    }
}

int** maze(int n) {
    int** maze = new int* [n];
    for (int i = 0; i< n; i++){
        maze[i]=new int[n];
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            maze[i][j] = 1;
        }
    }
    maze[0][0] = 0;
    generate(maze, 0, 0, n);
    return maze;
}

int** tmaze(int n){
    int** mazer = maze(n);
    int* l = new int[n*n];
    int k=0;
    for (int i=0; i<n-1; i++)
    {
        for (int j=0; j<n-1; j++)
        {
            if (i==0){
                if (j==0){
                    continue;
                }
                else{
                    if ((mazer[i][j-1]==1) && (mazer[i+1][j]==1) && (mazer[i][j]==0)){
                        l[k] = i*n+j;
                        k++;
                    }
                    else if ((mazer[i][j+1]==1) && (mazer[i+1][j]==1) && (mazer[i][j]==0)){
                        l[k] = i*n+j;
                        k++;
                    }
                }
            }
            else{
                if (j==0){
                    if ((mazer[i-1][j]==1) && (mazer[i][j+1]==1) && (mazer[i][j]==0)){
                        l[k] = i*n+j;
                        k++;
                    }
                    else if ((mazer[i+1][j]==1) && (mazer[i][j+1]==1) && (mazer[i][j]==0)){
                        l[k] = i*n+j;
                        k++;
                    }
                }
                else{
                    if ((mazer[i-1][j]==1) && (mazer[i][j+1]==1) && (mazer[i+1][j]==1) && (mazer[i][j]==0)){
                        l[k] = i*n+j;
                        k++;
                    }
                    else if ((mazer[i-1][j]==1) && (mazer[i][j+1]==1) && (mazer[i][j-1]==1) && (mazer[i][j]==0)){
                        l[k] = i*n+j;
                        k++;
                    }
                    else if ((mazer[i+1][j]==1) && (mazer[i][j+1]==1) && (mazer[i][j-1]==1) && (mazer[i][j]==0)){
                        l[k] = i*n+j;
                        k++;
                    }
                    else if ((mazer[i+1][j]==1) && (mazer[i][j-1]==1) && (mazer[i-1][j]==1) && (mazer[i][j]==0)){
                        l[k] = i*n+j;
                        k++;
                    }
                }
            }
        }
    }

    int i = rand() % k;
    i = l[i];
    int j = i % n;
    i = i/n;
    mazer[i][j] = 2;

    return mazer;
}

void gen(int n, bool t)
{
    int** mazeres;
    if (t==true){
        mazeres = tmaze(n);
    }
    else {
        mazeres = maze(n);
    }
    string outfile = "maze.map";
    ofstream outf{outfile};

    for (int i=0; i<n+2; i++)
    {
        outf << "1 ";
    }
    outf << "\n";

    for(int i=0; i<n; i++)
    {
        outf << "1 ";
        for(int j=0; j<n; j++)
        {
            outf << mazeres[i][j] << " ";
        }
        outf << "1 \n";
    }

    for (int i=0; i<n+2; i++)
    {
        outf << "1 ";
    }
    outf << "\n";
}