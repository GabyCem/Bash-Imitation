// Gabriel Cembal 203289269
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#define MAXSIZE 100
#define FORMAT "%s"
#define JOBS "jobs"
#define HISTORY "history"
#define CD "cd"
#define EXIT "exit"
// this struct holds the process, the name of the command, and if its running or not.
typedef struct
{
    pid_t pid;
    char command_name[MAXSIZE];
    char *stat;
} pid_stat;

//this func checks if a process is currently running.
int isRunning(pid_t pid)
{
    // if waitpid is not 0 then the process is done.
    if (waitpid(pid, NULL, WNOHANG) != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int main()
{
    pid_t pid;
    int status;
    // this is an array of structs of commands.
    pid_stat *pid_stats[MAXSIZE];
    int pid_stat_len = 0;
    // an array of paths that were put in the command line.
    char paths[MAXSIZE][MAXSIZE];
    // the path of the home directory.
    char home[MAXSIZE];
    getcwd(home, MAXSIZE);
    int commands = 0;
    // here we store the input.
    char input[MAXSIZE + 1];
    int path_counter = 0;
    while (1)
    {
        // allocate room for the current command.
        pid_stat *stat_struc = malloc(sizeof(pid_stat));
        pid_stat_len++;
        printf("$ ");
        fflush(stdout);
        // get the input from the user.
        fgets(input, MAXSIZE, stdin);
        char enter = '\n';
        int index = 0;
        // to remove the enter char.
        while (input[index] != enter)
        {
            index++;
        }
        input[index] = 0;
        char reduced_input[index + 1];
        int i;
        for (i = 0; i < index; i++)
        {
            reduced_input[i] = input[i];
        }
        // we store in the last place of the array NULL.
        reduced_input[i] = 0;
        const char *s = " ";
        char *token;
        char *tokens[MAXSIZE];
        char temp[MAXSIZE];
        strcpy( temp , reduced_input);
        /* get the first token */
        token = strtok(temp, s);
        /* walk through other tokens */
        int counter = 0;
        // in this loop we parse the string when the delimiter is a space char.
        while (token != NULL)
        {
            tokens[counter] = token;
            token = strtok(NULL, s);
            counter++;
        }
        tokens[counter] = NULL;
        // get rid of not needed spaces.
        char *reduced_tokens[counter + 1];
        int p = 0;
        for (p; p < counter + 1; p++)
        {
            reduced_tokens[p] = tokens[p];
        }
        // if the input is "jobs".
        if (strcmp(reduced_input, JOBS) == 0)
        {
            int j = 0;
            for (j; j < commands; j++)
            {
                if (strcmp(pid_stats[j]->stat, "DONE") == 0)
                {
                    continue;
                }
                // if the process is running.
                else if (isRunning(pid_stats[j]->pid))
                {
                    pid_stats[j]->stat = "RUNNING";
                }
                else
                {
                    pid_stats[j]->stat = "DONE";
                }
            }
            // print the current running processes.
            int k = 0;
            for (k; k < commands; k++)
            {
                if (strcmp(pid_stats[k]->stat, "RUNNING") == 0)
                {
                    printf("%s\n", pid_stats[k]->command_name);
                }
            }
            // add the jobs process.
            stat_struc->pid = getpid();
            stat_struc->stat = "DONE";
            strcpy(stat_struc->command_name, "jobs");
            pid_stats[commands] = stat_struc;
            commands++;
        }
        // if the input is history.
        else if (strcmp(reduced_input, HISTORY) == 0)
        {
            int l = 0;
            for (l; l < commands; l++)
            {
                if (strcmp(pid_stats[l]->stat, "DONE") == 0)
                {
                    continue;
                }
                // if the process is running change the status.
                else if (isRunning(pid_stats[l]->pid) == 1)
                {
                    pid_stats[l]->stat = "RUNNING";
                }
                else
                {
                    pid_stats[l]->stat = "DONE";
                }
            }
            // add the history process.
            stat_struc->pid = getpid();
            stat_struc->stat = "RUNNING";
            strcpy(stat_struc->command_name, "history");
            pid_stats[commands] = stat_struc;
            commands++;
            // print the history process.
            int q = 0;
            for (q; q < commands; q++)
            {

                printf("%s %s\n", pid_stats[q]->command_name, pid_stats[q]->stat);
            }
            stat_struc->stat = "DONE";
        }
        // if the input is cd.
        else if (strcmp(tokens[0], CD) == 0)
        {
            if (counter > 2)
            {
                printf("Too many argument\n");
                continue;
            }
            // parse the string using '/' as delimiter.
            const char *delimiter = "/";
            // this is for the case we get only '/' as the flag. in that case we will go to the root directory.
            if (strlen(tokens[1]) == 1 && strcmp(tokens[1] , delimiter) == 0) {
                chdir("/");
                continue;
            }
            char *comm;
            char *comms[MAXSIZE];
            /* get the first token */
            comm = strtok(tokens[1], delimiter);
            /* walk through other tokens */
            int comm_counter = 0;
            while (comm != NULL)
            {
                comms[comm_counter] = comm;
                comm = strtok(NULL, delimiter);
                comm_counter++;
            }
            // put NULL in last array place.
            comms[comm_counter] = NULL;
            // get rid of not needed spaces.
            char *reduced_comms[comm_counter + 1];
            int m = 0;
            for (m; m < comm_counter + 1; m++)
            {
                reduced_comms[m] = comms[m];
            }
            // get the current path.
            getcwd(paths[path_counter], MAXSIZE);
            int n = 0;
            for (n; n < comm_counter; n++)
            {
                if (strcmp(reduced_comms[n], "..") == 0)
                {
                    if (chdir("..") == -1)
                    {
                        printf("chdir failed");
                        break;
                    }
                    // after we increase the counter we save the new path.
                    path_counter++;
                    getcwd(paths[path_counter], MAXSIZE);
                }
                // here we go the the home directory.
                else if (strcmp(reduced_comms[n], "~") == 0)
                {
                    path_counter++;
                    chdir(getenv("HOME"));
                    getcwd(paths[path_counter], MAXSIZE);
                }
                // here we go to the previous directory.
                else if (strcmp(reduced_comms[n], "-") == 0)
                {
                    if (path_counter > 0)
                    {
                        path_counter--;
                        chdir(paths[path_counter]);
                    }
                }
                else
                {
                    getcwd(paths[path_counter], MAXSIZE);
                    path_counter++;
                    chdir(reduced_comms[n]);
                }
            }
        }
        else if (strcmp(reduced_input, EXIT) == 0)
        {
            int o = 0;
            for (o; o < pid_stat_len; o++)
            {
                free(pid_stats[o]);
            }
            break;
        }
        else
        {
			// running in background.
            if (reduced_input[index - 1] == '&')
            {
                // this way we get rid of the " &".
                reduced_tokens[counter - 1] = 0;
                input[index - 2] = 0;
                // this is the child process.
                if ((pid = fork()) == 0)
                {
                    execvp(reduced_tokens[0], reduced_tokens);
                    printf("exec failed\n");
                    exit(0);
                }
                else if (pid < 0)
                {
                    printf("fork failed");
                }
                else
                {
                    if (isRunning(pid)){
                        stat_struc->pid = pid;
                        stat_struc->stat = "RUNNING";
                        strcpy(stat_struc->command_name, input);
                        pid_stats[commands] = stat_struc;
                        commands++;
                    }
                    else {
                        stat_struc->pid = pid;
                        stat_struc->stat = "DONE";
                        strcpy(stat_struc->command_name, input);
                        pid_stats[commands] = stat_struc;
                        commands++;
                    }
                    
                }
            }
            else
            {
                // this is the child process.
                if ((pid = fork()) == 0)
                {
                    execvp(reduced_tokens[0], reduced_tokens);
                    printf("exec failed\n");
                    exit(0);
                }
                if (pid < 0)
                {
                    printf("fork failed");
                }
                else
                {
                    waitpid(pid ,&status, 0);
                    if(WIFEXITED(status))
                    {
                    stat_struc->pid = pid;
                    stat_struc->stat = "DONE";
                    strcpy(stat_struc->command_name, input);
                    pid_stats[commands] = stat_struc;
                    commands++;
                    }   
                }
            }
        }
    }
}