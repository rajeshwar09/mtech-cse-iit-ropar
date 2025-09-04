#include <stdio.h>
#include <stdlib.h>

#define SIZE 35

int ways[SIZE][SIZE];
int visit[SIZE][SIZE];

typedef struct Node {
  int data;
  struct Node* next;
} Node;

typedef struct Stack {
  Node* top;
  int size;
} Stack;

void init(Stack* st) {
  st->top = NULL;
  st->size = 0;
}

int is_empty(Stack* st) {
  return st->top == NULL;
}

void push(Stack* st, int value) {
  Node* new_node = (Node*)malloc(sizeof(Node));
  new_node->data = value;
  new_node->next = st->top;
  st->top = new_node;
  st->size++;
}

int pop(Stack* st) {
  Node* new_node = st->top;
  int value = new_node->data;
  st->top = new_node->next;
  free(new_node);
  st->size--;
  
  return value;
}

void clear_memory(Stack* st) {
  while (!is_empty(st)) {
    pop(st);
  }
}

long long catalan_number(int num) {
  long long result = 1;
  for (int i = 0; i < num; i++) {
    result = result * (2 * num - i) / (i + 1);
  }

  result = result / (num + 1);

  return result;
}

int count_completions(int inputs_left, int in_stack) {
  if (inputs_left == 0) {
    return 1;
  }

  if (visit[inputs_left][in_stack]) {
    return ways[inputs_left][in_stack];
  }

  visit[inputs_left][in_stack] = 1;

  int total = 0;

  if (in_stack > 0) {
    total = total + count_completions(inputs_left, in_stack - 1);
  }

  total += count_completions(inputs_left - 1, in_stack + 1);

  ways[inputs_left][in_stack] = total;

  return total;
}

int calculate_kth_permutation(int N, int k, int output[]) {
  if (k < 1 || k > catalan_number(N)) {
    return 0;
  }

  for (int i = 0; i <= N; i++) {
    for (int j = 0; j <= N; j++) {
      visit[i][j] = 0;
      ways[i][j] = 0;
    }
  }

  Stack st;
  init(&st);
  int produced = 0;
  int next_value = 1;
  int inputs_left = N;
  int in_stack = 0;

  while (produced < N) {
    if (in_stack > 0) {
      int count = count_completions(inputs_left, in_stack - 1);
      if (k <= count) {
        output[produced++] = pop(&st);
        in_stack--;
        continue;
      }
      k = k - count;
    }

    for (int i = 1; i <= inputs_left; i++) {
      int count = count_completions(inputs_left - i, in_stack + i - 1);
      if (k <= count) {
        for (int j = 0; j < i; j++) {
          push(&st, next_value + j);
        }

        output[produced++] = pop(&st);
        next_value = next_value + i;
        inputs_left = inputs_left - i;
        in_stack = in_stack + i - 1;
        break;
      }
      k = k - count;
    }
  }
  clear_memory(&st);
  return 1;
}

int main(int argc, char const *argv[])
{
  int T;
  scanf("%d", &T);

  for (int t = 0; t < T; t++) {
    int N, k;
    scanf("%d %d", &N, &k);

    long long total_combinations = catalan_number(N);
    int output[SIZE];

    if (calculate_kth_permutation(N, k, output)) {
      printf("%lld", total_combinations);
      for (int i = 0; i < N; i++) {
        printf(" %d", output[i]);
      }
      printf("\n");
    } else {
      printf("%lld -1\n", total_combinations);
    }
  }
  return 0;
}
