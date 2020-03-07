/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-03-07 19:52
> Email        : 25915043@qq.com
> Filename     : shell_simulator.c
> Description  : a shell simulator 
*******************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>

#define MAX_LINE 80 
/* The maximum length command */
int init_args(char *input,char *args[],int num_of_args);

int main(void)
{
    /* command line arguments */
    char *args[MAX_LINE/2 + 1]; 
    char *cmd_args[MAX_LINE/2 + 1];
    char *first_args[MAX_LINE/2 + 1];
    char *second_args[MAX_LINE/2 + 1];
    char *out_args[MAX_LINE/2 + 1];
    char *in_args[MAX_LINE/2 + 1];

    int should_run = 1; 
    char *input;
    char *last_input;
    char *redirect_path;
    int num_of_args=0;
    int pipe_flag=0,pipe_index=0; 
    int in_redirect_flag=0,out_redirect_flag=0; 
    int wait_flag=1;
    int fd=0;
    pid_t pid;

    //initialization
    for (int i=0;i<MAX_LINE/2+1;i++) {
        args[i]=malloc(sizeof(char)*MAX_LINE);
        memset(args[i],0,sizeof(char)*MAX_LINE);
    }
    input=malloc(sizeof(char)*MAX_LINE);
    last_input=malloc(sizeof(char)*MAX_LINE);
    last_input[0]='\0';
    redirect_path=malloc(sizeof(char)*MAX_LINE);

    //run the command
    while (should_run) {
        num_of_args=0;
        pipe_flag=0;
        pipe_index=0;
        in_redirect_flag=0;
        out_redirect_flag=0;
        wait_flag=1;
        fd=0;

        printf("osh>");
        fflush(stdout);

        for (int i=0;i<MAX_LINE/2+1;i++) {
            memset(args[i],0,sizeof(char)*MAX_LINE);
        }

        //get the command and divide it into args[]
        fgets(input,MAX_LINE,stdin);
        num_of_args=init_args(input,args,num_of_args);
        
        //copy the args[] to cmd_args[]
        for (int i=0;i<num_of_args;i++)  {
            cmd_args[i]=malloc(sizeof(char)*MAX_LINE);
            cmd_args[i]=args[i];
        }
        cmd_args[num_of_args]=malloc(sizeof(char)*MAX_LINE);
        cmd_args[num_of_args]=NULL;

/*
        printf("%d\n",num_of_args);
        for (int i=0;i<num_of_args;i++) {
            printf("%s\n",cmd_args[i]);   
        }
        printf("\n");
*/      
  
        //handle with 'exit' and '!!'
        if(strcmp(args[num_of_args-1],"exit")==0&&num_of_args==1) {
            printf("Goodbye PsyduckLiu!\n");
            should_run = 0;
            break;
        }
        else {
            if (strcmp(args[num_of_args-1],"!!")==0&&num_of_args==1&&last_input[0]!='\0') {
                strcpy(input,last_input);
                num_of_args=init_args(input,args,num_of_args);
                
                printf("Last command is:");
                printf("%s",last_input);

                for (int i=0;i<num_of_args;i++)  {
                    cmd_args[i]=malloc(sizeof(char)*MAX_LINE);
                    cmd_args[i]=args[i];  
                }
                cmd_args[num_of_args]=malloc(sizeof(char)*MAX_LINE);
                cmd_args[num_of_args]=NULL;

/*
                printf("%d\n",num_of_args );
                for (int i=0;i<num_of_args;i++) {
                    printf("%s\n",cmd_args[i]);
                }
*/          
            }
            else {
                if (strcmp(args[num_of_args-1],"!!")==0&&num_of_args==1&&last_input[0]=='\0') {
                    printf("No commands in history!\n");
                    continue;
                }
            }
        }
        strcpy(last_input,input);

        //handle with '&' '>' '<' '|' init 
        if (strcmp(args[num_of_args-1],"&")==0) {
            wait_flag=0;
            cmd_args[num_of_args-1]=NULL;
        } 
            
        if (strcmp(args[num_of_args-2],">")==0) {
            out_redirect_flag=1;
            strcpy(redirect_path,args[num_of_args-1]);
            
            /* printf("%s\n",redirect_path );*/

            for (int i=0;i<num_of_args-2;i++) {
                out_args[i]=malloc(sizeof(char)*MAX_LINE);
                out_args[i]=args[i];
            }
            out_args[num_of_args-2]=malloc(sizeof(char)*MAX_LINE);
            out_args[num_of_args-2]=NULL;

        }
        if (strcmp(args[num_of_args-2],"<")==0) {
            in_redirect_flag=1;
            strcpy(redirect_path,args[num_of_args-1]);
            
            /*printf("%s\n",redirect_path );*/

            for (int i=0;i<num_of_args-2;i++) {
                in_args[i]=malloc(sizeof(char)*MAX_LINE);
                in_args[i]=args[i];   
            }
            in_args[num_of_args-2]=malloc(sizeof(char)*MAX_LINE);
            in_args[num_of_args-2]=NULL;

        }
    
        for (int i=0;i<num_of_args;i++) {
            if (strcmp(args[i],"|")==0) {
                
                for (int j=0;j<i;j++) {
                    first_args[j]=malloc(sizeof(char)*MAX_LINE);
                    first_args[j]=args[j];
                
                    /*printf("%s",first_args[j]);*/
                }
                /*printf("\n");*/
                first_args[i]=malloc(sizeof(char)*MAX_LINE);
                first_args[i]=NULL;

                for (int j=0;j<num_of_args-i-1;j++)  {
                    second_args[j]=malloc(sizeof(char)*MAX_LINE);
                    second_args[j]=args[i+j+1];

                    /*printf("%s",second_args[j]);*/
                }
                second_args[num_of_args-i-1]=malloc(sizeof(char)*MAX_LINE);
                second_args[num_of_args-i-1]=NULL;

                pipe_flag=1;
            }

            else 
                continue;
        }

        //handle with the command
        pid=fork();
        if (pid<0) {
            perror("Fork error!\n");
        }
        else {
            if (pid==0) {
                //child process

                //prepare file
                if (in_redirect_flag) {
                    fd=open(redirect_path,O_RDONLY,0644);
                    if (fd<0) {
                        perror("Open the redirect file failed!");
                        exit(1);
                    }
                    if (dup2(fd,0)<0) {
                        perror("dup2 error!");
                        exit(1);
                    }
                    close(fd);
                }

                if (out_redirect_flag) {
                    fd=open(redirect_path, O_WRONLY | O_TRUNC | O_CREAT,0644);
                    if(fd < 0)  {
                        perror("Open the redirect file failed!"); 
                        exit(1);
                    }
                    if(dup2(fd, 1  ) < 0) {
                        perror("dup2 error!"); 
                        exit(1);
                    }
                    close(fd);
                }

                //handle with the pipe
                if (!pipe_flag)  {
                    if (out_redirect_flag) {
                        execvp(out_args[0],out_args);
                    }
                    else {
                        if (in_redirect_flag) {
                            execvp(in_args[0],in_args);
                        }
                        else 
                            execvp(cmd_args[0],cmd_args);
                    }
                }
                else {
                    int channel[2];
                    int result=0;
                    pid_t ppid;

                    pipe(channel);
                    ppid=fork();

                    if (ppid<0) {
                        perror("Failed to create process!");
                        exit(1);
                    }
                    else if (ppid>0) {   
                        //child procress

                        close (channel[1]);
                        dup2(channel[0], STDIN_FILENO);
                        wait(NULL);
                        execvp(second_args[0],second_args );
                        close(channel[0]);
                        fflush(stdin);
                    }
                
                    else {
                        //grandchild process
                        
                        close (channel[0]);
                        dup2(channel[1], STDOUT_FILENO);
                        execvp(first_args[0],first_args );
                        close(channel[1]);
                        fflush(stdin);

                    }
                }
            }

            else {
                //father process

                if (wait_flag) wait(NULL);
               // else signal(SIGCHLD,SIG_IGN);
            }
        }
    }

    return 0;
}

int init_args(char *input,char *args[],int num_of_args)
{
    char *tmp_args;
    int flag=0,pre_flag=0;
    int index_of_args=0;
    int index_of_tmp_args=0;
    tmp_args=malloc(sizeof(char)*MAX_LINE);

    for (int i=0;i<MAX_LINE;i++) {
        if (input[i]=='\n') break;
        else {
            if (input[i]==' ') {
                pre_flag=flag;
                flag=0;
            }

            else {
                pre_flag=flag;
                flag=1;
                tmp_args[index_of_tmp_args]=input[i];
                index_of_tmp_args+=1;
            }
        }

        if (pre_flag==1&&flag==0) {
            tmp_args[index_of_tmp_args]='\0';
            strcpy(args[index_of_args],tmp_args);
            index_of_args+=1;
            index_of_tmp_args=0;
            memset(tmp_args, 0, sizeof(char) * MAX_LINE);
        }
    }
    if (flag==1) {
        tmp_args[index_of_tmp_args]='\0';
        strcpy(args[index_of_args],tmp_args);
        index_of_args+=1;
    }
    
    free(tmp_args);
    num_of_args=index_of_args;

    return num_of_args;
}
    

