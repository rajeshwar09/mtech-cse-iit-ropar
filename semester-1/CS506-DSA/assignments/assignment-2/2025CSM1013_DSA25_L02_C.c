#include <stdio.h>
#include <stdlib.h>

#define  SIZE 30

int N, K; // N = Numbers, K = kth permutation
int perm_counter = 0; // number of permutations created
int perm_found = 0; // if kth permutation found

// linked list node for stack
typedef struct Node {
  int data;
  struct Node* next;
} Node;

// stack
typedef struct Stack {
  Node* top;
  int size;
} Stack;

// initialize empty stack
void init_stack(Stack* st) {
  st->top = NULL;
  st->size = 0;
}

// check for empty stack
int is_empty(Stack* st) {
  return st->top == NULL;
}

// adding element to stack
void push(Stack* st, int value) {
  Node* temp = (Node*)malloc(sizeof(Node));
  temp->data = value;
  temp->next = st->top;
  st->top = temp;
  st->size++;
}

// removing element from stack
int pop(Stack* st) {
  Node* temp = st->top;
  int value = temp->data;
  st->top = temp->next;
  free(temp);
  st->size--;

  return value;
}

// free allocated memory
void free_memory(Stack* st) {
  while (!is_empty(st)) {
    pop(st);
  }
}

// total possible permutations
long long catalan(int n) {
  long long result = 1;
  for (int i = 0; i < n; i++) {
    result = result * (2*n - i) / (i + 1);
  }

  return result / (n + 1);
}

// to generate permutations
void permutation(Stack* st, int left, int current[], int result[]) {
  static int pos = 0; // position in current array
  static int next = 1;  // next number to push in stack

  // when one permutation completed
  if (pos == N) {
    perm_counter++;
    if (perm_counter == K) {
      for (int i = 0; i < N; i++) {
        result[i] = current[i];
      }
      perm_found = 1;
    }
    return;
  }

  //pop
  if (!is_empty(st)) {
    int temp = pop(st);
    current[pos++] = temp;

    permutation(st, left, current, result);

    pos--;
    push(st, temp);

    if (perm_found) {
      return;
    }
  }

  //push new values
  for (int i = 1; i <= left && !perm_found; i++) {
    for (int j = 0; j < i; j++) {
      push(st, next + j);
    }

    int temp = pop(st);
    current[pos++] = temp;

    next += i;
    permutation(st, left - i, current, result);
    next -= i;

    pos--;
    for (int j = 0; j < i - 1; j++) {
      pop(st);
    }

    if (perm_found) {
      return;
    }
  }
}

// find kth permutation
int kth_perm(int result[]) {
  Stack st;
  init_stack(&st);
  int temp[SIZE];
  perm_counter = 0;
  perm_found = 0;

  permutation(&st, N, temp, result);

  free_memory(&st);

  return perm_found;
}

// main function
int main() {
  int T;
  scanf("%d", &T);

  for (int t = 0; t < T; t++) {
    scanf("%d %d", &N, &K);
    
    long long cat = catalan(N);
    int result[SIZE];

    if (kth_perm(result)) {
      printf("%lld", cat);
      
      for (int i = 0; i < N; i++) {
        printf(" %d", result[i]);
      }
    } else {
      printf("%lld -1\n", cat);
    }

    printf("\n");
  }

  return 0;
}