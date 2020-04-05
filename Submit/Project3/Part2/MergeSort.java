/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-04-05 20:38
> Email        : 25915043@qq.com
> Filename     : MergeSort.java
> Description  : merge sort
*******************************************************/

import java.util.concurrent.*;
import java.util.Scanner;
import java.util.Arrays;

public class MergeSort extends RecursiveAction
{
	static int SIZE = 1000;
    static int THRESHOLD = 100;

    private int begin;
    private int end;
    private int[] array;
	
	public MergeSort(int begin, int end, int[] array){
        this.begin = begin;
        this.end = end;
        this.array = array;
    }
	
	protected void compute(){
        if (end - begin < THRESHOLD){
            System.out.println("Sort method is Insertion sort");
			int tmp;
			int i,j;
			
			for(i=1;i<SIZE;i++) {
				if (array[i]<array[i-1]){
					tmp = array[i];
					for (j=i;j>=0;j--) {
						if (j>0 && array[j-1]>tmp) {
							array[j]=array[j-1];
						}
						else {
							array[j]=tmp;
							break;
						}
					}
                }
				else continue;
			}
		}
		else {
			System.out.println("Sort method is merge sort");
			int mid = (begin + end) / 2;
            MergeSort lefttask = new MergeSort(begin, mid, array);
            MergeSort righttask = new MergeSort(mid + 1, end, array);
          
			lefttask.fork();
            righttask.fork();
            
			lefttask.join();
            righttask.join();
            

            merge(begin, end, array);
		}	
	}
	
	protected void merge(int begin, int end, int [] array){
        int mid = (begin+end)/2;
        int i=begin, j=mid+1, k=0;
        int [] tmp = new int[end-begin+1];
        
		while(i<=mid && j<=end){
            if(array[i]<array[j])   
				tmp[k++] = array[i++];
            else    
				tmp[k++] = array[j++];
        }
        while(j<=end)    
			tmp[k++] = array[j++];
        while(i<=mid)    
			tmp[k++] = array[i++];

        for(i=begin;i<=end;i++){
            array[i] = tmp[i-begin];
        }
    }
	
	public static void main(String [] args){
        ForkJoinPool pool = new ForkJoinPool();
		char type;
		int size;
		int i;
		Scanner  scan_input = new Scanner(System.in);
		
		System.out.println("Do you want to set the size of array or use the default size?(y/n,y for you set size and n for default size)");
		type =  scan_input.next().charAt(0);
		
		if (type=='y') {
			System.out.println("Please input the size of the array:");
			SIZE = scan_input.nextInt();
        }

		int [] array = new int[SIZE];
		java.util.Random rand = new java.util.Random();
		for(i=0;i<SIZE;i++){
			array[i] = rand.nextInt(SIZE);
		}
		
		System.out.println("Before sorting, the initial array is:");
		System.out.println(Arrays.toString(array));
		
		MergeSort task = new MergeSort(0, SIZE-1, array);

		pool.invoke(task);

		System.out.println("After sorting, the final array is:");
		System.out.println(Arrays.toString(array));
    }
	
}