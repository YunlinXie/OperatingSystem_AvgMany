/*
 For this assignment you will extend avg (from assignment #1) to average numbers from multiple files.

You will develop a program avg_many that is given a list of files on the command line. You will need to average up all of the floating point numbers in all of the files to produce a single average.

Assume that you have text files with numbers separated by newlines. You want to read the numbers in from all the files. Since you want to be flexible on the type and size of numbers your program can handle, you should use double floating point.

If a file appears multiple times on the command line, those numbers will be processed multiple times.

You should use fork() to start a process for each of the files so that we can compute sums and counts in parallel. To communicate between processes you will use pipe(). You need to wait for all the processes to finish using wait().

You must be able to handle files with any number of numbers (including no numbers). You can assume that the files are well formed: they will contain only valid numbers separated by newlines.

The code/ipc directory contains examples of using fork, pipe and wait, which you can consult for help.

For example, numbers.txt contains a list of numbers that average to 2.1. running
 **/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


/*
 * Define a struct
 * Act as a helper for calculating
 * cumulative sum and count of numbers
 **/
typedef struct s{
    int n;
    double sum;
} node;


/*
 * Define helper function avg
 * @param n: number of inputing files
 * @param file[][2]: 2D array for store sum and count
 * @param **argv: array of files
 * @postcondition 2D array will contain sums and
 * counts from all input files
 **/
void avg(int n, int file[][2], char *argv[]){
    // Base case
    if(n == 0)
        return;
    
    int pid = fork();
    
    // In child process
    if(pid == 0) {
        avg(n - 1, file, argv);
        FILE *f = fopen(argv[n], "r");
        
        // Check whether file is opened successfully
        if (f == NULL){
        printf("Cannot open file\n");
        exit(1);
        }
        
        double sum = 0;
        double num;
        int i = 0;
        int res;
        
        // Get the sum and count of the file
        while ((res = fscanf(f, "%lf", &num)) != EOF) {
            sum = sum + num;
            i++;
        }
        node r;
        r.sum = sum;
        r.n = i;
        // Put the sum and count into 2D array, can be read later
        write(file[n][1], &r, sizeof(r));
        exit(0);
    }
    // In parent process
    else{
        wait(NULL);
    }
}


/*
 * Main function
 * @param argc: number of arguments including
 * the executable fie of avg_many.c itself
 * @param **argv: array of input files including
 * the executable fie of avg_many.c itself
 * @postcondition: print out the final average
 **/
int main(int argc, char *argv[])
{
    // We need to minus one because argc includes the executable fie of avg_many.c itself 
    int number = argc -1;
    int res_of_file[argc][2];
    double sum;
    int count = 0;

    // Process each files
    for(int i = 1; i <= number; i++){
        pipe(res_of_file[i]);
    }
    
    // Each file(or process) will call avg function  
    avg(number, res_of_file, argv);

    // Loop to calculate the total sum and count of all numbers
    for(int i = 1; i <= number; i++){
        node r;
        // Get the sum and count of each file
        read(res_of_file[i][0], &r, sizeof(r));
        // Add sum and count to total sum and total count
        sum = sum + r.sum;
        count = count + r.n;
    }
    
    // avg = 0 if files are empty(count = 0) 
    if(count == 0){
        printf("%.4f\n", 0.0);
        printf("Average is zero because the total number of numbers is zero.\n");
        return 0;
    }
    
    double result = sum /count;
    // Round final answer to fourth decimal place
    printf("%.4f\n", result);
    
    return 0;
}
