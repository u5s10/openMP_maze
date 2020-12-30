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

#define rows 201
#define cols 201
int maze[rows][cols];
int increment = 1;
unsigned char color[rows][cols][3];

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
    file = fopen("huge_maze.txt", "r");
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
    maze[row][col] = level;
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
                maze[--row_inc][col_inc] = level;
                p = check_possible_paths(row_inc, col_inc);
            }else if(d == RIGHT){
                maze[row_inc][++col_inc] = level;
                p = check_possible_paths(row_inc, col_inc);
            }else if(d == DOWN){
                maze[++row_inc][col_inc] = level;
                p = check_possible_paths(row_inc, col_inc);
            }else if(d == LEFT){
                maze[row_inc][--col_inc] = level;
                p = check_possible_paths(row_inc, col_inc);
            }
        }
        else if(p.directions.size() > 1)
        {
            while(p.directions.size() != 1){
            direction d2 = p.directions.top();
            p.directions.pop();
            if(d2 == UP){
                travel(row_inc - 1,col_inc, ++increment);
            }else if(d2 == RIGHT){
                travel(row_inc,col_inc + 1, ++increment);                
            }else if(d2 == DOWN){
                travel(row_inc + 1,col_inc, ++increment);                
            }else if(d2 == LEFT){
                travel(row_inc,col_inc - 1, ++increment);                
            }
        }
//            p = check_possible_paths(row_inc, col_inc);
        }
    }
    return;
}

int main()
{
    // initialize maze from text file
    int a = init_maze();

    // start traversing the maze
    travel(1, 1, 1);

    //print maze to console

    // for (int i = 0; i < rows; i++)
    // {
    //     for (int j = 0; j < cols; j++)
    //     {
    //         if (maze[i][j] == -1)
    //         {
    //             printf("#");
    //         }
    //         else if (maze[i][j] == 0)
    //         {
    //             printf(" ");
    //         }
    //         else
    //         {
    //             printf("%d", maze[i][j]);
    //         }
    //     }
    //     printf("\n");
    // }

    // initializing array of random color for every path that was taken
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
