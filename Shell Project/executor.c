/*Osaaretin Osa-Edokpolor,UID: 115651098*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include "command.h"
#include "executor.h"

#define FILE_PERMISSIONS 0664

/*static void print_tree(struct tree *t);*/ 
int execute_aux(struct tree *top, int parent_input_fd, int parent_output_fd );

int execute(struct tree *t) {
  return  execute_aux(t, STDIN_FILENO, STDOUT_FILENO);
}

int execute_aux(struct tree *top, int parent_input_fd, int parent_output_fd ){
   pid_t pid;
   int status;
   int fd_in = parent_input_fd;
   int fd_out = parent_output_fd;
   
   

   if (top->conjunction == NONE){

         if (strcmp(top->argv[0], "exit") == 0){

            exit(0);
         }else if(strcmp(top-> argv[0], "cd") == 0) {
   
            if (top -> argv[0] == NULL){

               if(chdir(getenv("HOME")) == -1){
                  perror("HOME");
                  return EXIT_FAILURE;
               }else{
                  return EXIT_SUCCESS;
               }
               
            }else{

               if(chdir(top-> argv[1]) == -1){
                  perror(top-> argv[1]);
                  return EXIT_FAILURE;
               }else{
                  return EXIT_SUCCESS;
               }
               
            } 
         }else{

            if ((pid = fork()) < 0){
               perror("fork");
               exit(EX_OSERR);
            }
            
            
            if (pid){ /*parent code*/
               wait(&status);
               return WEXITSTATUS(status);

            }else{/*child code*/
               int new_fd;
               int check_ambi = 0;

               if (top->input && fd_in) {
                 check_ambi = 1;
               } else if (top->input) {
                  if ((new_fd = open(top->input, O_RDONLY)) < 0) {
                     perror("can't open file");
                     exit(EX_OSERR);
                  }
                  if (dup2(new_fd, STDIN_FILENO) < 0) {
                     perror("dup2 error");
                     exit(EX_OSERR);
                  }
                  close(new_fd);
               } else if (fd_in) {
                  if (dup2(fd_in, STDIN_FILENO) < 0) {
                     perror("dup2 error");
                     exit(EX_OSERR);
                  }
               }
               if (top->output && fd_out) {
                  check_ambi = 2;
               } else if (top->output) {
                  if ((new_fd = open(top->output,O_WRONLY | O_CREAT | O_TRUNC,FILE_PERMISSIONS)) < 0) {
                     perror("can't open file");
                     exit(EX_OSERR);
                  }
                  if (dup2(new_fd, STDOUT_FILENO) < 0) {

                  perror("dup2 error");
                  exit(EX_OSERR);
                  }
                  close(new_fd);
               } else if (fd_out) {
                  if (dup2(fd_out, STDOUT_FILENO) < 0) {
                     perror("dup2 error");
                     exit(EX_OSERR);
                  }
               }

               if (check_ambi) {
                  if (check_ambi ==  2){
                     printf("Ambiguous output redirect.\n");
                     exit(EX_OSERR); 
                  }else{
                     printf("Ambiguous input redirect.\n");
                     exit(EX_OSERR); 
                  } 
                }
               execvp(top->argv[0], top->argv);
               fprintf(stderr, "Failed to execute %s", top->argv[0]);
               fflush(stdout);
               exit(EX_OSERR);
            }
         }
   }else if (top->conjunction == AND){

      if (!execute_aux(top->left, fd_in, fd_out)) {
         return execute_aux(top->right, fd_in, fd_out);
      }

      return EXIT_FAILURE;
   }else if (top->conjunction == SUBSHELL) {


         
      if ((pid = fork()) < 0) {   
         perror("fork");
         exit(EX_OSERR);        
      } 
      if (pid){   
           
         wait(&status);
         return WEXITSTATUS(status);

      }else{   
         exit (execute_aux(top->left, fd_in, fd_out));
            
      }

   }else if (top->conjunction == PIPE) {


   
         pid_t pid_two;
         int pipe_fd[2];
      if (pipe(pipe_fd) < 0) {
         perror("pipe error");
      }

      if ((pid_two = fork()) < 0) {   
         perror("fork");
         exit(EX_OSERR);        

      } 
      if (pid_two){
         int status_two, res;
         close(pipe_fd[1]);

         res = execute_aux(top->right,pipe_fd[0],fd_out);
         close(pipe_fd[0]);
         wait(&status_two);
         exit(WEXITSTATUS(status_two) && res);
      }else{  
         int status_twoch;
         close(pipe_fd[0]);
         status_twoch = execute_aux(top->left,fd_in,pipe_fd[1]);
         close(pipe_fd[1]);
         exit (status_twoch);
      }
   }
      

   return EXIT_FAILURE;
}
/*
static void print_tree(struct tree *t) {
   if (t != NULL) {
      print_tree(t->left);  

      if (t->conjunction == NONE) {
         printf("NONE: %s, ", t->argv[0]);
      } else {
         printf("%s, ", conj[t->conjunction]);
      }
      printf("IR: %s, ", t->input);
      printf("OR: %s\n", t->output);

      print_tree(t->right);
   }
}
*/