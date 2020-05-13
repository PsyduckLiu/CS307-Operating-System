/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-05-13 23:27
> Email        : 25915043@qq.com
> Filename     : banker.c
> Description  : banker algorithm
*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>

#define NUMBER_OF_CUSTOMERS 5 
#define NUMBER_OF_RESOURCES 4
#define MAX_COMMAND_LENGTH 20
#define true 1

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
char cmd_buffer[MAX_COMMAND_LENGTH];
int customer_num; 
int cmd_parameter[NUMBER_OF_RESOURCES];

void read_max_file()
{
    FILE *fp=NULL;
    int customer=0,resource=0;
	int count=0;
	char ch,data[10];

    if((fp=fopen("max.txt","r"))==NULL) {
		perror("open file error\n");
		exit(1) ;
	}

	ch=fgetc(fp);
	while(ch!=EOF) {
		if (ch=='\n') {
            data[count]='\0';
            maximum[customer][resource]=atoi(data);
            count=0;

			customer++;
			resource=0;
		}
		else {
			if (ch==',') {
				data[count]='\0';
				maximum[customer][resource]=atoi(data);
				count=0;
				resource++;
			}

			else {
				data[count]=ch;
				count++;
			}
		}

		ch=fgetc(fp);
	}

    data[count]='\0';
    maximum[customer][resource]=atoi(data);

    fclose(fp);
}

void update_need() {
	int i,j;
	
	for(i=0;i<NUMBER_OF_CUSTOMERS;i++) {
		for(j=0;j<NUMBER_OF_RESOURCES;j++) {
			need[i][j]=maximum[i][j]-allocation[i][j];
		}
	}

}

void preprocess(){
    int i,j=0;
	int count=0,space_num=0;
	char data[10];
    int pos=0;

    for(i=0;i<MAX_COMMAND_LENGTH;i++){
        if(cmd_buffer[i]=='\n') {
            data[count]='\0';
            cmd_parameter[j]=atoi(data);
            break;
        }

        if(cmd_buffer[i]==' '){
			space_num++;

			if(space_num==1) {
				count=0;
			}

			if(space_num==2) {
				data[count]='\0';
				customer_num=atoi(data);
				count=0;
			}

			if(space_num>2) {
				data[count]='\0';
				cmd_parameter[j]=atoi(data);
				count=0;
				j++;
			}
        }
		else {
			cmd_buffer[pos]=cmd_buffer[i];
			data[count]=cmd_buffer[i];
			count++;
			pos++;
		}
    }

}

int request_resources(int customer_num, int request[]) {
	int i,j;
	int times=NUMBER_OF_CUSTOMERS;
	int count=0;
	int pre_available[NUMBER_OF_RESOURCES];
	int pre_allocation[NUMBER_OF_RESOURCES];
	int served[NUMBER_OF_CUSTOMERS];
	
	for(i=0;i<NUMBER_OF_RESOURCES;i++) {
		if(request[i]>available[i]) {
			printf("Request exceed available and would be denied.\n");
			return -1;
		}
	}
	for(i=0;i<NUMBER_OF_RESOURCES;i++) {
		if(request[i]>maximum[customer_num][i]) {
			printf("Request exceed maximum and would be denied.\n");
			return -1;
		}
	}
	
	for(i=0;i<NUMBER_OF_RESOURCES;i++){	
		pre_available[i]=available[i];
	}
	for(i=0;i<NUMBER_OF_RESOURCES;i++){	
		pre_allocation[i]=allocation[customer_num][i];
	}
	
	for(i=0;i<NUMBER_OF_RESOURCES;i++) {
		allocation[customer_num][i]+=request[i];
	}
	for(i=0;i<NUMBER_OF_RESOURCES;i++){	
		available[i]-=request[i];
	}
	
	update_need();
	
	for(i=0;i<NUMBER_OF_CUSTOMERS;i++){	
		served[i]=0;
	}
	
	while(times>0){
		for(i=0;i<NUMBER_OF_CUSTOMERS;i++){
			if(served[i]==0){
				for(j=0;j<NUMBER_OF_RESOURCES;j++){
					if(need[i][j]>available[j])
						break;
				}
				
				if(j==NUMBER_OF_RESOURCES){
					served[i]=1;
					count++;
					for(j=0;j<NUMBER_OF_RESOURCES;j++)
						available[j]+=allocation[i][j];
				}
			}
		}
		
		if(count==NUMBER_OF_CUSTOMERS) 
			break;
		
		times--;
	}

	if(count==NUMBER_OF_CUSTOMERS){
		for(i=0;i<NUMBER_OF_RESOURCES;i++){	
			available[i]=pre_available[i]-request[i];
		}
		return 0;
	}
	else {
		for(i=0;i<NUMBER_OF_RESOURCES;i++)
			available[i]=pre_available[i];
		for(i=0;i<NUMBER_OF_RESOURCES;i++)
			allocation[customer_num][i]=pre_allocation[i];
		
		update_need();
		
		return -1;
	}
}

int release_resources(int customer_num,int release[]) {
	int i;
	
	for(i=0;i<NUMBER_OF_RESOURCES;i++) {
		if(release[i]>allocation[customer_num][i]) {
			printf("Release exceed allocation would be denied.\n");
			return -1;
		}
	}
	for(i=0;i<NUMBER_OF_RESOURCES;i++) {
		allocation[customer_num][i]-=release[i];
		available[i]+=release[i];
	}
	
	update_need();

    return 0;
}

void print_all()
{
	int i,j;
	
	printf("The avaliable array is:\n");
	for(i=0;i<NUMBER_OF_RESOURCES;i++) {
		printf("%d\t",available[i]);
	}
	printf("\n");

	printf("The maximum array is:\n");
	for(i=0;i<NUMBER_OF_CUSTOMERS;i++) {
		printf("T%d\t", i);
		for(j=0;j<NUMBER_OF_RESOURCES;j++) {
			printf("%d\t", maximum[i][j]);
		}
		printf("\n");
	}

	printf("The allocation array is:\n");
	for(i=0;i<NUMBER_OF_CUSTOMERS;i++) {
		printf("T%d\t", i);
		for(j=0;j<NUMBER_OF_RESOURCES;j++) {
			printf("%d\t", allocation[i][j]);
		}
		printf("\n");
	}

	printf("The need array is:\n");
	for(i=0;i<NUMBER_OF_CUSTOMERS;i++) {
		printf("T%d\t", i);
		for(j=0;j<NUMBER_OF_RESOURCES;j++) {
			printf("%d\t", need[i][j]);
		}
		printf("\n");
	}

}

int main(int argc, char *argv[])
{
	int init_resource[NUMBER_OF_RESOURCES];
	int i,j;
	
	if(argc!=NUMBER_OF_RESOURCES+1) {
		printf("Invalid Input\n");
		return -1;
	}
	
	//初始化available
	for(i=1;i<argc;i++) {
		available[i-1]=atoi(argv[i]);
	}
	
	//初始化maximum
	read_max_file();
	printf("The maximum array is set as:\n");
	for(i=0;i<NUMBER_OF_CUSTOMERS;i++){
        for(j=0;j<NUMBER_OF_RESOURCES;j++)
            printf("%d\t", maximum[i][j]);
        printf("\n");
    }
	
	//更新need的状态
	update_need();
	
	while(true){
        printf("Please enter the command>");
		
        fgets(cmd_buffer,MAX_COMMAND_LENGTH,stdin);
        preprocess();
		
        if(cmd_buffer[0]=='e') {
            printf("Exit banker algorithm.\n");
            break;
        }
		else if(cmd_buffer[1]=='Q'){
            if(request_resources(customer_num,cmd_parameter)==0) {
				printf("Request command is safe and would be satisfied.\n");
			}
			else {
				printf("Request command is unsafe and would be denied.\n");
			}
        }
		else if(cmd_buffer[1]=='L'){
            if(release_resources(customer_num,cmd_parameter)==0) {
                printf("Release command is safe and would be satisfied.\n");
            }
            else {
                printf("Release command is unsafe and would be denied.\n");
            }
        }
		else if(cmd_buffer[0]=='*'){
            print_all();
        }
		else{
            printf("Invalid Input\n");
        }
    }
}
