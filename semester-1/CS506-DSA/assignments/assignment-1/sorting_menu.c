#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void clear_terminal() {
  #ifdef _WIN32
    system("cls");
  #else
    system("clear");
  #endif
}

// Reverse array
void reverse_array(int array[], int size) {
  int l = 0, r = size-1;

  while (l < r) {
    int temp = array[l];
    array[l] = array[r];
    array[r] = temp;
    l++;
    r--;
  }
}

// Random array generator
void generate_array_random(int array[], int size) {
  for (int i = 0; i < size; i++) {
    int random_number = (rand() % 1000000) + 1;
    array[i] = random_number;
  }
}

// Almost sorted random array generator
void generate_array_almost_sorted(int array[], int size, double probability) {

  for (int i = 0; i < size; i++) {
    if (((double) rand() / RAND_MAX) < probability) {
      array[i] = i + 1;
    } else {
      int random_number = (rand() % 1000000) + 1;
      array[i] = random_number;
    }
  }
}

// Function to display(print) elements of array
void display_array(int array[], int array_size) {
  for (int i = 0; i < array_size; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
}

// Function of Bubble Sort
void bubble_sort(int array[], int array_size) {
  for (int i = 0; i < array_size-1; i++) {
    for (int j = 0; j < array_size-i-1; j++) {
      if (array[j] > array[j+1]) {
        int temp = array[j];
        array[j] = array[j+1];
        array[j+1] = temp;
      }
    }
    // display_array(array, array_size);
  }
}

// Function of Selection Sort
void selection_sort(int array[], int array_size) {
  for (int i = 0; i < array_size; i++) {
    int min_element = i;
    for (int j = i; j < array_size; j++) {
      if (array[min_element] > array[j]) {
        min_element = j;
      }
    }
    
    int temp = array[min_element];
    array[min_element] = array[i];
    array[i] = temp;

    // display_array(array, array_size);
  }
}

// Function of INsertion Sort
void insertion_sort(int array[], int array_size) {
  for (int i = 1; i < array_size; i++) {
    int current = array[i];
    int j = i - 1;
    while (j >= 0 && array[j] > current) {
      array[j+1] = array[j];
      j--;
    }

    array[j+1] = current;

    // display_array(array, array_size);
  }
}

// Function of Merge Sort + Merge
void merge(int array[], int start, int middle, int end) {
  int left_size = middle - start + 1;
  int right_size = end - middle;

  int* left_array = (int*)malloc(left_size * sizeof(int));
  int* right_array = (int*)malloc(right_size * sizeof(int));

  for (int i = 0; i < left_size; i++) left_array[i] = array[start+i];
  for (int i = 0; i < right_size; i++) right_array[i] = array[middle+1+i];

  int left = 0, right = 0, combined = start;
  
  while (left < left_size && right < right_size) {
    if (left_array[left] <= right_array[right]) {
      array[combined] = left_array[left];
      left++;
    } else {
      array[combined] = right_array[right];
      right++;
    }
    combined++;
  }

  while (left < left_size) {
    array[combined] = left_array[left];
    combined++;
    left++;
  }

  while (right < right_size) {
    array[combined] = right_array[right];
    combined++;
    right++;
  }
}

void merge_sort(int array[], int start, int end) {
  if (start >= end) return;

  int middle = (start + end) / 2;

  merge_sort(array, start, middle);
  merge_sort(array, middle+1, end);
  merge(array, start, middle, end);
}

// Function of Quick Sort
int partition(int array[], int start, int end) {
  // int pivot_index = start + (end - start) / 2;
  int pivot_index = start;
  int pivot = array[pivot_index];

  int left = start, right = end;

  while (1) {
    while (array[left] < pivot) left++;
    while (array[right] > pivot) right--;

    if (left > right) break;

    int temp = array[left];
    array[left] = array[right];
    array[right] = temp;
    left++;
    right--;
  }

  return left;
}

void quick_sort(int array[], int start, int end) {
  if (start < end) {
    int pivot_index = partition(array, start, end);
    quick_sort(array, start, pivot_index-1);
    quick_sort(array, pivot_index, end);
  }
}

int main(int argc, char const *argv[])
{
  clear_terminal();

  clock_t start;
  clock_t end;
  double total_time;

  int choice = 4;
  int test_cases;

  // scanf("%d", &choice);
  // scanf("%d", &test_cases);

  // for (int i = 0; i < test_cases; i++) {
    int array_sizes[] = {10, 100, 1000, 5000, 10000, 50000, 100000, 500000, 1000000};
    // int array_sizes[] = {10, 100, 1000, 5000, 10000, 50000, 100000, 120000, 150000};
    // int array_size;
    // scanf("%d", &array_size);
    // int array[array_size];

    // for (int j = 0; j < array_size; j++) {
    //   scanf("%d", &array[j]);
    // }

    switch (choice)
    {
      case 1:
        // Case 1 : Bubble Sort
        printf("\nBubble Sort-(On non-ascending sorted array)\n");
        for (int i = 0; i < 9; i++) {
          int test_case[array_sizes[i]];
          // generate_array_random(test_case, array_sizes[i]);
          // generate_array_almost_sorted(test_case, array_sizes[i], 0.5);
          generate_array_almost_sorted(test_case, array_sizes[i], 1.0);
          reverse_array(test_case, array_sizes[i]);
          
          start = clock();
          bubble_sort(test_case, array_sizes[i]);
          end = clock();

          total_time = ((double)(end - start)) / CLOCKS_PER_SEC;
          printf("\nInputs: %d\t=>\t Time (sec): %f", array_sizes[i], total_time);
        }
        printf("\n");
      break;

      case 2:
        // Case 2 : SElection Sort
        printf("\nSelection Sort-(On non-ascending sorted array)\n");
        for (int i = 0; i < 9; i++) {
          int test_case[array_sizes[i]];
          // generate_array_random(test_case, array_sizes[i]);
          // generate_array_almost_sorted(test_case, array_sizes[i], 0.5);
          generate_array_almost_sorted(test_case, array_sizes[i], 1.0);
          reverse_array(test_case, array_sizes[i]);
          
          start = clock();
          selection_sort(test_case, array_sizes[i]);
          end = clock();

          total_time = ((double)(end - start)) / CLOCKS_PER_SEC;
          printf("\nInputs: %d\t=>\t Time (sec): %f", array_sizes[i], total_time);
        }
        printf("\n");
      break;

      case 3:
        // Case 3 : Merge Sort
        printf("\nMerge Sort-(On non-ascending sorted array)\n");
        for (int i = 0; i < 9; i++) {
          int test_case[array_sizes[i]];
          // generate_array_random(test_case, array_sizes[i]);
          // generate_array_almost_sorted(test_case, array_sizes[i], 0.5);
          generate_array_almost_sorted(test_case, array_sizes[i], 1.0);
          reverse_array(test_case, array_sizes[i]);
          
          start = clock();
          merge_sort(test_case, 0, array_sizes[i]-1);
          end = clock();
          
          total_time = ((double)(end - start)) / CLOCKS_PER_SEC;
          printf("\nInputs: %d\t=>\t Time (sec): %f", array_sizes[i], total_time);
        }
        printf("\n");
      break;

      case 4:
        // Case 4 : Insertion Sort
        printf("\nInsertion Sort-(On non-ascending sorted array)\n");
        for (int i = 0; i < 9; i++) {
          int test_case[array_sizes[i]];
          // generate_array_random(test_case, array_sizes[i]);
          // generate_array_almost_sorted(test_case, array_sizes[i], 0.5);
          generate_array_almost_sorted(test_case, array_sizes[i], 1.0);
          // reverse_array(test_case, array_sizes[i]);
          
          start = clock();
          insertion_sort(test_case, array_sizes[i]);
          end = clock();
          
          total_time = ((double)(end - start)) / CLOCKS_PER_SEC;
          printf("\nInputs: %d\t=>\t Time (sec): %f", array_sizes[i], total_time);
        }
        printf("\n");
      break;

      case 5:
        // Case 5 : Quick Sort
        printf("\nQuick Sort-(On non-ascending sorted array)\n");
        for (int i = 0; i < 9; i++) {
          int test_case[array_sizes[i]];
          // generate_array_random(test_case, array_sizes[i]);
          // generate_array_almost_sorted(test_case, array_sizes[i], 0.5);
          generate_array_almost_sorted(test_case, array_sizes[i], 1.0);
          reverse_array(test_case, array_sizes[i]);
          
          start = clock();
          quick_sort(test_case, 0, array_sizes[i]-1);
          end = clock();
          
          total_time = ((double)(end - start)) / CLOCKS_PER_SEC;
          printf("\nInputs: %d\t=>\t Time (sec): %f", array_sizes[i], total_time);
        }
        printf("\n");
      break;

      default:
      break;
    }
  // }

  return 0;
}
