#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include <array>
#include <random>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

using namespace std;

enum direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

struct path_check
{
    stack<direction> directions;
    int number_of_paths;
};

#define rows 101
#define cols 101
int maze[rows][cols];
int increment = 1;
unsigned char color[rows][cols][3];
omp_lock_t writelock;
omp_lock_t lock_array[rows][cols];

path_check check_possible_paths(int row, int col)
{
    struct path_check output_path;
    output_path.number_of_paths = 0;

    if (maze[row - 1][col] == 0)
    { // UP
        output_path.directions.push(UP);
        output_path.number_of_paths++;
    }
    if (maze[row + 1][col] == 0)
    { // DOWN
        output_path.directions.push(DOWN);
        output_path.number_of_paths++;
    }
    if (maze[row][col + 1] == 0)
    { // RIGHT
        output_path.directions.push(RIGHT);
        output_path.number_of_paths++;
    }
    if (maze[row][col - 1] == 0)
    { // LEFT
        output_path.directions.push(LEFT);
        output_path.number_of_paths++;
    }
    return output_path;
}

int init_maze()
{
    FILE *file;
    file = fopen("mazes_txt_files/medium_maze.txt", "r");
    char ch;

    if (file == NULL)
    {
        return -1;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols + 1; j++)
        {
            fscanf(file, "%c", &ch);
            if (ch == '#')
            {
                maze[i][j] = -1;
                color[i][j][0] = 0;
                color[i][j][1] = 0;
                color[i][j][2] = 0;
            }
            else if (ch == ' ')
            {
                maze[i][j] = 0;
                color[i][j][0] = 255;
                color[i][j][1] = 255;
                color[i][j][2] = 255;
            }
        }
    }
    fclose(file);
    return 0;
}

void travel(int row, int col, int level)
{
    if (maze[row][col] != 0)
    {
        printf("WALL\n");
        return;
    }
    // mutex for writing to maze array
    omp_set_lock(&(lock_array[row][col]));
    if(maze[row][col] == 0){
        maze[row][col] = level;
    }else{
        return;
    }
    omp_unset_lock(&(lock_array[row][col]));
    
    struct path_check p = check_possible_paths(row, col);
    int row_inc = row;
    int col_inc = col;
    while (p.directions.size() >= 1)
    {
        if (p.directions.size() == 1)
        {
            direction d = p.directions.top();
            p.directions.pop();
            if(d == UP){
                row_inc--;

                // mutex for writing to maze array
                omp_set_lock(&(lock_array[row][col]));
                if(maze[row_inc][col_inc] == 0){
                    maze[row_inc][col_inc] = level;
                }else{
                    return;
                }
                omp_unset_lock(&(lock_array[row][col]));

                p = check_possible_paths(row_inc, col_inc);
            }else if(d == RIGHT){
                col_inc++;

                // mutex for writing to maze array
                omp_set_lock(&(lock_array[row][col]));
                if(maze[row_inc][col_inc] == 0){
                    maze[row_inc][col_inc] = level;
                }else{
                    return;
                }
                omp_unset_lock(&(lock_array[row][col]));

                p = check_possible_paths(row_inc, col_inc);
            }else if(d == DOWN){
                row_inc++;

                // mutex for writing to maze array
                omp_set_lock(&(lock_array[row][col]));
                if(maze[row_inc][col_inc] == 0){
                    maze[row_inc][col_inc] = level;
                }else{
                    return;
                }
                omp_unset_lock(&(lock_array[row][col]));

                p = check_possible_paths(row_inc, col_inc);
            }else if(d == LEFT){
                col_inc--;

                // mutex for writing to maze array
                omp_set_lock(&(lock_array[row][col]));
                if(maze[row_inc][col_inc] == 0){
                    maze[row_inc][col_inc] = level;
                }else{
                    return;
                }
                omp_unset_lock(&(lock_array[row][col]));

                p = check_possible_paths(row_inc, col_inc);
            }
        }
        else if(p.directions.size() > 1)
        {
            while(p.directions.size() != 1){
            direction d2 = p.directions.top();
            p.directions.pop();
            // mutex for increment value
            omp_set_lock(&writelock);
            increment++;
            omp_unset_lock(&writelock);
            if(d2 == UP){
                #pragma omp task firstprivate(row_inc, col_inc, increment)                
                travel(row_inc - 1,col_inc, increment);
            }else if(d2 == RIGHT){
                #pragma omp task firstprivate(row_inc, col_inc, increment)                                
                travel(row_inc,col_inc + 1, increment);                
            }else if(d2 == DOWN){
                #pragma omp task firstprivate(row_inc, col_inc, increment)                                
                travel(row_inc + 1,col_inc, increment);                
            }else if(d2 == LEFT){
                #pragma omp task firstprivate(row_inc, col_inc, increment)                                
                travel(row_inc,col_inc - 1, increment);                
            }
        }
        }
    }
    #pragma omp taskwait    
    return;
}



int main()
{
    // initialize maze from text file
    int a = init_maze();

    // start traversing the maze
    omp_init_lock(&writelock);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            omp_init_lock(&(lock_array[rows][cols]));
        }
        
    }

    omp_set_dynamic(0);
    omp_set_num_threads(4);
    #pragma omp parallel
    {
        #pragma omp single
        travel(1,1,1);
    }

    // initializing array of random colors for every path that was taken
    unsigned char path_colors[increment][3];    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0,255);

    for (int i = 0; i < increment; i++)
    {
        int a = distr(gen);
        int b = distr(gen);
        int c = distr(gen);
        path_colors[i][0] = a;
        path_colors[i][1] = b;
        path_colors[i][2] = c;
    }
    
    // saving maze to ppm
    FILE *fp;
    char *filename = "new1.ppm";
    char *comment = "# ";
    fp = fopen(filename, "wb");
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, cols, rows, 255);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            unsigned char test_temp[3] = {path_colors[maze[i][j]][0],
            path_colors[maze[i][j]][1],
            path_colors[maze[i][j]][2] };
            if(maze[i][j] == - 1){
                fwrite(color[i][j], 1, 3, fp);                
            }else{
                fwrite(test_temp, 1, 3, fp);                
            }

        }
    }
    fclose(fp);
    return 0;
}
