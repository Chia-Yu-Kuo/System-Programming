#include<stdio.h>
#include<stdlib.h>

void merge(int *arr, int start, int mid, int end){
	int temp_size = end - start + 1;
	int temp[temp_size];  //variable temp length stored in stack

	for(int i = 0; i< temp_size;i++)
		temp[i] = arr[i+start];

	int left_index = 0;
	int right_index = mid-start+1;
	int left_max = mid-start;
	int right_max = end-start;
	int arr_index = start;

	while(left_index <= left_max && right_index <= right_max){
		if(temp[left_index] <= temp[right_index]){
			arr[arr_index] = temp[left_index];
			arr_index++;
			left_index++;
		}
		else{
			arr[arr_index] = temp[right_index];
			arr_index++;
			right_index++;
		}
	}
    while(left_index <= left_max){
        arr[arr_index] = temp[left_index];
        arr_index++;
        left_index++;
    }
    while(right_index <= right_max){
        arr[arr_index] = temp[right_index];
        arr_index++;
        right_index++;
    }
}

void mergesort(int *arr, int start, int end){
	if(start < end){
		int mid = (end+start)/2;
		mergesort(arr, start, mid);
		mergesort(arr, mid+1, end);
		merge(arr, start, mid, end);
	}
}

int main(){
    int TEST1_SIZE = 32;
    int TEST2_SIZE = 15;
    int TEST3_SIZE = 27;
    int test1[TEST1_SIZE] = {16,2,4,16,4,10,12,2,14,8,4,14,6,4,2,10,12,6,10,2,14,14,6,8,16,8,16,6,12,10,8,12};
    int test2[TEST2_SIZE] = {470,405,225,197,126,122,56,33,-81,-275,-379,-409,-416,-496,-500};
    int test3[TEST3_SIZE] = {412,-474,443,171,-23,247,221,7,40,221,-90,61,-9,49,-80,-80,221,-379,-161,-397,-173,276,-197,221,-12,-145,101};

	mergesort(test1, 0, TEST1_SIZE-1);
    mergesort(test2, 0, TEST2_SIZE-1);
	mergesort(test3, 0, TEST3_SIZE-1);

	return 0;
}
