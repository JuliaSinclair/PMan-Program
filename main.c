#include <string.h> //strcmp()
#include <stdbool.h> 
#include <stdlib.h>  //malloc()
#include <unistd.h>  //fork(), execvp()
#include <stdio.h>  //printf(), perror(), fscanf()
#include <sys/types.h> //pid_t
#include <sys/wait.h>  //waitpid()
#include <signal.h>  //kill(), SIGTERM, SIGKILL, SIGSTOP, SIGCOUNT
#include <errno.h>  //errno
#include "linked_list.h" 

Node* head = NULL;


/*
	Function: func_BG
	Arguments:
		cmd: User Input
	Purpose: Runs user inputted process in new child process 
*/
void func_BG(char **cmd){
  	pid_t pid = fork();
    // Failed Fork
    if (pid == -1) {
      printf("Error: fork() failed\n");
    }
    //Child Process
    else if (pid == 0) {
      if(execvp(cmd[1], &cmd[1]) < 0 ){
        perror("Error: execvp() failed\n");        
      }
      exit(EXIT_SUCCESS);
    }
    //Parent Process
    else {
      // Finding file path of inputted process
      FILE * file;
      if ((file = fopen(cmd[1], "r"))) {
        fclose(file);
        char * file_path = realpath(cmd[1], NULL);
        head = add_newNode(head, pid, file_path);
      }
      else {
        head = add_newNode(head, pid, "Not File");
      }
      sleep(1);
    }
}

/*
	Function: removeBackgroundProcesses
	Purpose: Iterates through linked list and removes nodes whos process id is not currently running
*/
void removeNonBackgroundProcesses(){
  Node * current;
  current = head;
  while(current != NULL) {
    int stat;
    waitpid(current->pid, &stat, WNOHANG);
    if(kill(current->pid, 0)==-1) {
      head = deleteNode(head, current->pid);
    }
    current = current->next;
  }
}

/*
	Function: func_BGlist
	Arguments:
		cmd: User Input
	Purpose: Prints list including process id and file path of background processes currently running
*/
void func_BGlist(char **cmd){
	removeNonBackgroundProcesses();
  int count = 0;
  Node* current = head;
  while (current != NULL) {
    count++;
    current = current->next;
  }

  printList(head);
  printf("Total process: %d\n", count);
}


/*
	Function: func_BGkill
	Arguments:
		str_pid: A string containing process id
	Purpose: Permenantly terminates background process with given process id
*/
void func_BGkill(char * str_pid){
  //Checks that PID is valid
	int pid = atoi(str_pid);
  if (!PifExist(head, pid)) {
    printf("Error: Process %d does not exist\n", pid);
    return;
  }

  int kill_result = kill(pid, SIGTERM);
  if (kill_result == -1) {
    printf("Error: kill failed for process %d\n", pid);
  }
  else {
    head = deleteNode(head, pid);
    printf("Success: Process %d killed\n", pid);
    sleep(1);
  }
}

/*
	Function: func_BGstop
	Arguments:
		str_pid: A string containing process id
	Purpose: Temporarily stops background process with given process id
*/
void func_BGstop(char * str_pid){
  //Checks that PID is valid
	int pid = atoi(str_pid);
  if (!PifExist(head, pid)) {
    printf("Error: Process %d does not exist\n", pid);
    return;
  }

  int kill_result = kill(pid, SIGSTOP);
  if (kill_result == -1) {
    printf("Error: stop failed for process %d\n", pid);
  }
  else {
    printf("Success: Process %d stopped\n", pid);
    sleep(1);
  }
}

/*
	Function: func_BGstart
	Arguments:
		str_pid: A string containing process id
	Purpose: Restarts background process that was temporarily stopped (using BGstop) with given process id
*/
void func_BGstart(char * str_pid){
  // Checks that PID is valid
	int pid = atoi(str_pid);
  if (!PifExist(head, pid)) {
    printf("Error: Process %d does not exist\n", pid);
    return;
  }

  int kill_result = kill(pid, SIGCONT);
  if (kill_result == -1) {
    printf("Error: start failed for process %d\n", pid);
  }
  else {
    printf("Success: Process %d started\n", pid);
    sleep(1);
  }
}


/* 
  Function: func_pstat
  Arguments: 
    str_pid: A string containing process id  
  Purpose: Prints out comm, state, utime, stime, rss, number of voluntary context switches 
  and number of involuntary context switches of process with given process id  
  Assumptions: 
    (1) The file names for /proc/[pid]/stat and /proc/[pid]/status are not longer than 128
    (2) One line of information in proc/[pid]/status is not longer than 128
*/

void func_pstat(char * str_pid){
   // Checks PID is valid
	int pid = atoi(str_pid);
  if (!PifExist(head, pid)) {
    printf("Error: Process %d does not exist\n", pid);
    return;
  }

  // Open pseudo-files and checks they are readable
  char proc_status[128];
  sprintf(proc_status, "/proc/%s/status", str_pid);
  FILE* status_file = fopen(proc_status, "r");
  char proc_stat[128];
  sprintf(proc_stat, "/proc/%s/stat", str_pid);
  FILE* stat_file = fopen(proc_stat, "r");
  if(proc_status != NULL && proc_stat != NULL) {

    // Information from /proc/pid/status pseudo-file 
    char status [128][128];
    int i = 0;
    while(fgets(status[i], 1000, status_file) != NULL) {
      i++;
    };
    char * voluntary_ctxt_switches = status[53];
    char * nonvoluntary_ctxt_switches = status[54];
    
    //Information from /proc/pid/stat pseudo-file
    char comm[128];
    char state;
    long unsigned int stat_utime;
    long unsigned int stat_stime;
    long int rss;
    fscanf(stat_file, "%*d %s %c %*d %*d %*d %*d %*d %*u %*d %*d %*d %*d %lu %lu %*d %*d %*d %*d %*d %*d %*d %*d %ld", comm, &state, &stat_utime, &stat_stime, &rss);
    
    // Dividing utime and stime by _SC_CLK_TCK
    float float_utime = (float) stat_utime;
    float float_stime = (float) stat_stime;
    float clock_ticks = (float) sysconf(_SC_CLK_TCK);
    float utime = float_utime/clock_ticks;
    float stime = float_stime/clock_ticks;

    // Printing pstat information
    printf("Success: pstat information for process %d\n", pid);
    printf("comm = \t\t\t\t%s\n", comm);
    printf("state = \t\t\t%c\n", state);
    printf("utime = \t\t\t%f\n", utime);
    printf("stime = \t\t\t%f\n", stime);
    printf("rss = \t\t\t\t%ld\n", rss);
    printf("%s", voluntary_ctxt_switches);
    printf("%s", nonvoluntary_ctxt_switches);

  }
  
  // If files are unreadable
  else {
    printf("Error: could not read file %s and/or %s", proc_stat, proc_status);
  }

  // Close pseudo-files
  fclose(status_file);
  fclose(stat_file);
}
 
/*
  Function: main
  Purpose: Runs Pman
  Source: From Fall 2022 CSC 360 Brightspace in Tutorial 2
  Added: I added the condition that lst[1] must not be null to the if statements for bgkill, bgstop, 
  bgstart and pstat to prevent segmentation faults

*/
int main(){
    char user_input_str[50];
    while (true) {
      printf("Pman: > ");
      fgets(user_input_str, 50, stdin);
      if((strlen(user_input_str) != 1)) {
        printf("User input: %s \n", user_input_str);
      }
      char * ptr = strtok(user_input_str, " \n");
      if(ptr == NULL){
        continue;
      }
      char * lst[50];
      int index = 0;
      lst[index] = ptr;
      index++;
      while(ptr != NULL){
        ptr = strtok(NULL, " \n");
        lst[index]=ptr;
        index++;
      }
      if (strcmp("bg",lst[0]) == 0){
        func_BG(lst);
      } else if (strcmp("bglist",lst[0]) == 0) {
        func_BGlist(lst);
      } else if (strcmp("bgkill",lst[0]) == 0 && lst[1] != NULL) {
        func_BGkill(lst[1]);
      } else if (strcmp("bgstop",lst[0]) == 0 && lst[1] != NULL) {
        func_BGstop(lst[1]);
      } else if (strcmp("bgstart",lst[0]) == 0 && lst[1] != NULL) {
        func_BGstart(lst[1]);
      } 
      else if (strcmp("pstat",lst[0]) == 0 && lst[1] != NULL) {
        func_pstat(lst[1]);
      } else if (strcmp("q",lst[0]) == 0) {
        printf("Bye Bye \n");
        exit(0);
      } else {
        printf("Invalid input\n");
      }
    }

  return 0;
}

