/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-05-25 14:50
> Email        : 25915043@qq.com
> Filename     : virtual_memory.c
> Description  : a virtual memory manager 
*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

#define PAGE_SIZE 256
#define PAGE_ENTRIES 256
#define PAGE_NUM_BITS 8
#define PAGE_OFFSET_BITS 8

#define FRAME_SIZE 256
#define FRAME_ENTRIES 128
#define FRAME_NUM_BITS 8
#define FRAME_OFFSET_BITS 8

#define TLB_ENTRIES 16
#define LRU_MAX 1000
#define MEM_SIZE (FRAME_SIZE*FRAME_ENTRIES)

#define BACKING_STORE "./BACKING_STORE.bin"
#define INPUT "./addresses.txt"
#define OUTPUT "./output.txt"
#define CORRECT "./correct.txt"

struct entry_item {
    int page_index;
    int frame_index;
    int lru_num;
};
struct entry_item tlb_table[TLB_ENTRIES];

int page_table[PAGE_ENTRIES];
int frame_table[FRAME_ENTRIES];
char memory[MEM_SIZE];
int memory_lru[FRAME_ENTRIES];
int tlb_hit=0;
int fault_num=0;
int addresses_num=0;

void update_tlb(int page_num){
    int i;

    for(i=0;i<TLB_ENTRIES;i++){
        if (tlb_table[i].page_index==page_num)
            tlb_table[i].lru_num=LRU_MAX;
        else
            tlb_table[i].lru_num--;
    }
}

void update_mem(int frame_num){
    int i;

    for(i=0;i<FRAME_ENTRIES;i++){
        if(i==frame_num)
            memory_lru[i]=LRU_MAX;
        else
            memory_lru[i]--;
    }
}



int main(int argc, char *argv[])
{
    int i;
    int address,offset;
    int page_num,frame_num=-1;
    int lru_min,lru_min_index;
    signed char data;
    signed char buffer[PAGE_SIZE];
    FILE *fp=NULL;
    FILE *fp_backstore=NULL;
    FILE *fp_out=NULL;

    printf("Page number is: %d, Page size is: %d\n",PAGE_ENTRIES,PAGE_SIZE);
    printf("Frame number is: %d, Frame size is: %d\n",FRAME_ENTRIES,FRAME_SIZE);
    
    if(argc!=2) {
        printf("Invalid Input\n");
		return -1;
    }
    
    for (i=0;i<PAGE_ENTRIES;i++) {
        page_table[i]=-1;
    }
    for (i=0;i<FRAME_ENTRIES;i++) {
        frame_table[i]=1;
        memory_lru[i]=LRU_MAX;
    }
    for (i=0;i<TLB_ENTRIES;i++) {
        tlb_table[i].page_index=-1;
        tlb_table[i].frame_index=-1;
        tlb_table[i].lru_num=LRU_MAX;
    }

    //fp=fopen(argv[1],"r");
    if ((fp=fopen(argv[1],"r"))==NULL) {
		perror("open addresses.txt file error\n");
		exit(1);
	}
    if ((fp_backstore=fopen("BACKING_STORE.bin","rb"))==NULL) {
		perror("open BACKING_STORE.bin file error\n");
		exit(1);
	}
    if ((fp_out=fopen("output.txt","w"))==NULL) {
		perror("open output.txt file error\n");
		exit(1);
	}

    fscanf(fp,"%d",&address);
    while(!feof(fp)){
        frame_num=-1;

        offset=address%PAGE_SIZE;
        page_num=(address/PAGE_SIZE)%PAGE_ENTRIES;

        for (i=0;i<TLB_ENTRIES;i++) {
            if (tlb_table[i].page_index==page_num) {
                frame_num=tlb_table[i].frame_index;
                update_tlb(page_num);
            }
        }

        //TLB hit
        if (frame_num!=-1) {
            data=memory[frame_num*FRAME_SIZE+offset];
            tlb_hit++;
        }
        //page table hit
        else if (page_table[page_num]!=-1) {
            lru_min=LRU_MAX;
            lru_min_index=0;

            frame_num=page_table[page_num];
            data=memory[frame_num*FRAME_SIZE+offset];

            for (i=0;i<TLB_ENTRIES;i++) {
                if (tlb_table[i].lru_num<lru_min) {
                    lru_min=tlb_table[i].lru_num;
                    lru_min_index=i;
                }
            }

            tlb_table[lru_min_index].page_index=page_num;
            tlb_table[lru_min_index].frame_index=frame_num;
            update_tlb(page_num);
        }
        //page table miss
        else {
            fseek(fp_backstore,page_num*PAGE_SIZE,SEEK_SET);
            fread(buffer,sizeof(char),PAGE_SIZE,fp_backstore);

            for (i=0;i<FRAME_ENTRIES;i++) {
                if(frame_table[i]==1) {
                    frame_num=i;
                    break;
                }
            }

            if (frame_num==-1) {
                lru_min=LRU_MAX;
                lru_min_index=0;

                for (i=0;i<FRAME_ENTRIES;i++) {
                    if (memory_lru[i]<lru_min) {
                        lru_min=memory_lru[i];
                        lru_min_index=i;
                    }
                }

                frame_num=lru_min_index;
                update_mem(frame_num);
            }

            for (i=frame_num*FRAME_SIZE;i<(frame_num+1)*FRAME_SIZE;i++) {
                memory[i]=buffer[i-frame_num*FRAME_SIZE];
            }
            frame_table[frame_num]=0;

            data=memory[frame_num*FRAME_SIZE+offset];

            page_table[page_num]=frame_num;
            lru_min=LRU_MAX;
            lru_min_index=0;
            for (i=0;i<TLB_ENTRIES;i++) {
                if (tlb_table[i].lru_num<lru_min) {
                    lru_min=tlb_table[i].lru_num;
                    lru_min_index=i;
                }
            }
            tlb_table[lru_min_index].page_index=page_num;
            tlb_table[lru_min_index].frame_index=frame_num;
            update_tlb(page_num);

            fault_num++;
        }

        addresses_num++;

        fprintf(fp_out,"Virtual address: %d Physical address: %d Value: %d\n",address,frame_num*FRAME_SIZE+offset,data);
        printf("Virtual address: %d Physical address: %d Value: %d\n",address,frame_num*FRAME_SIZE+offset,data);

        fscanf(fp,"%d",&address);
    }

    printf("Output saved to the file output.txt\n");
    printf("Page fault: %.5f%%\n",fault_num*100.0/addresses_num);
    printf("TLB hit: %.5f%%\n",tlb_hit*100.0/addresses_num);


    fclose(fp);
    fclose(fp_backstore);
    fclose(fp_out);

    return 0;
}
