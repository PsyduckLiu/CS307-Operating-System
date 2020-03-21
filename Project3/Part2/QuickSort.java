import java.util.concurrent.*;
import java.util.Scanner;
import java.util.Arrays;

public class QuickSort extends RecursiveAction
{
	static int SIZE = 1000;
    static int THRESHOLD = 100;

    private int begin;
    private int end;
    private int[] array;
	
	public QuickSort(int begin, int end, int[] array){
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
			System.out.println("Sort method is quick sort");
			int low = begin, high = end;
            int value = array[low];
			int mid;

            while(low!=high) {
                while(low < high && array[high] >= value) high--;
                if(low < high) { 
					array[low] = array[high]; 
					low++;
				}
                
				while(low < high && array[low] <= value)  low++;
                if(low < high) {
					array[high] = array[low];
					high--; 
				}
            }
            array[low] = value;
			
			mid = low + 1;
			QuickSort left = new QuickSort(begin, mid - 1, array);
            QuickSort right = new QuickSort(mid + 1, end, array);

            left.fork();
            right.fork();

            left.join();
            right.join();
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
		
		QuickSort task = new QuickSort(0, SIZE-1, array);

		pool.invoke(task);

		System.out.println("After sorting, the final array is:");
		System.out.println(Arrays.toString(array));
    }
	
}