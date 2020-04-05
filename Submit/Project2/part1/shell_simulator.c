/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-04-01 19:52
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

#define MAX_LINE 80 

int init_args(char *input,char *args[],int num_of_args);

int main(void)
{
    // command line arguments 
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
    int pipe_flag=0;
	int pipe_index=0; 
    int in_redirect_flag=0;
	int out_redirect_flag=0; 
    int wait_flag=1;
    int flag1=0;
	int flag2=0;
    pid_t pid;

    //initialization
    for (int i=0;i<MAX_LINE/2+1;i++) {
        args[i]=malloc(sizeof(char)*MAX_LINE);
	cmd_args[i]=malloc(sizeof(char)*MAX_LINE);
	first_args[i]=malloc(sizeof(char)*MAX_LINE);
	second_args[i]=malloc(sizeof(char)*MAX_LINE);
	out_args[i]=malloc(sizeof(char)*MAX_LINE);
	in_args[i]=malloc(sizeof(char)*MAX_LINE);
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
        flag1=0;
		flag2=0;

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
            cmd_args[i]=args[i];
        }
        cmd_args[num_of_args]=malloc(sizeof(char)*MAX_LINE);
        cmd_args[num_of_args]=NULL;    
  
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
                    cmd_args[i]=args[i];  
                }
                cmd_args[num_of_args]=malloc(sizeof(char)*MAX_LINE);
                cmd_args[num_of_args]=NULL;     
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

            for (int i=0;i<num_of_args-2;i++) {
                out_args[i]=args[i];
            }
            out_args[num_of_args-2]=malloc(sizeof(char)*MAX_LINE);
            out_args[num_of_args-2]=NULL;
        }
		
        if (strcmp(args[num_of_args-2],"<")==0) {
            in_redirect_flag=1;
            strcpy(redirect_path,args[num_of_args-1]);

            for (int i=0;i<num_of_args-2;i++) {
                in_args[i]=args[i];   
            }
            in_args[num_of_args-2]=malloc(sizeof(char)*MAX_LINE);
            in_args[num_of_args-2]=NULL;
        }
    
        for (int i=0;i<num_of_args;i++) {
            if (strcmp(args[i],"|")==0) {
                
                for (int j=0;j<i;j++) {
                    first_args[j]=args[j];
                }
                first_args[i]=malloc(sizeof(char)*MAX_LINE);
                first_args[i]=NULL;

                for (int j=0;j<num_of_args-i-1;j++)  {
                    second_args[j]=args[i+j+1];
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
            fprintf(stderr,"Fork Failed");
			return 1;
        }
        else {
            if (pid==0) {
                //child process

                //prepare file
                if (in_redirect_flag) {
                    flag1=open(redirect_path,O_RDONLY,0644);
                    if (flag1<0) {
                        fprintf(stderr,"Open Failed");
						return 1;
                    }
					
					flag2=dup2(flag1,0);
                    if (flag2<0) {
                        fprintf(stderr,"Dup2 Failed");
						return 1;
                    }
                    
					close(flag1);
                }

                if (out_redirect_flag) {
                    flag1=open(redirect_path, O_WRONLY | O_CREAT | O_APPEND,0644);
                    if (flag1<0) {
                        fprintf(stderr,"Open Failed");
						return 1;
                    }
					
					flag2=dup2(flag1,1);
                    if(flag2<0) {
                        fprintf(stderr,"Dup2 Failed");
						return 1;
                    }
					
                    close(flag1);
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
                    int fd[2];
                    int result=0;
                    pid_t ppid;

                    if (pipe(fd)==-1) {
						fprintf(stderr,"Pipe Failed");
						return 1;
					}
					
                    ppid=fork();
                    if (ppid<0) {
                        fprintf(stderr,"Fork Failed");
						return 1;
                    }
                    else { 
						if (ppid>0) {   
							//child procress

							close(fd[1]);
							dup2(fd[0],0);
							
							wait(NULL);
							
							execvp(second_args[0],second_args);
							close(fd[0]);
						}
                
						else {
							//grandchild process
                        
							close(fd[0]);
							dup2(fd[1],1);
							
							execvp(first_args[0],first_args);
							close(fd[1]);
						}
					}
                }
            }

            else {
                //father process

                if (wait_flag) wait(NULL);
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
    

