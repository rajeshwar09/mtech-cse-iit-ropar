/* 
  Code information:
  working fast for n <= 70 on my laptop
  but taking considerable large time for higher cases

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KSIZE 4

// Linked list node to store digits
typedef struct Node {
  int digit;
  struct Node* next;
} Node;

// Added digit at end of linked list
void insert_at_end(Node** head, int value) {
  Node* new_node = (Node*)malloc(sizeof(Node));
  new_node->digit = value;
  new_node->next = NULL;

  if (*head == NULL) {
    *head = new_node;
    return;
  }

  Node* temp = *head;
  while (temp->next != NULL) {
    temp = temp->next;
  }

  temp->next = new_node;
  return;
}

// for displaying linked list (for debug purpose)
void display_list(Node* head) {
  Node* temp = head;

  if (temp == NULL) {
    printf("List is empty.\n");
    return;
  }

  while (temp->next != NULL) {
    printf("%d -> ", temp->digit);
    temp = temp->next;
  }

  printf("%d\n", temp->digit);
}

// Convert number to linked list (in reverse order)
void convert_num_to_ll(Node** head, int num) {
  while (num > 0) {
    int d = num % 10;
    num = num / 10;
    insert_at_end(head, d);
  }
}

// Free memory of the complete linked list
void free_memory(Node* head) {
  while (head) {
    Node* temp = head->next;
    free(head);
    head = temp;
  }
}

// Calculate length of linked list
int length_of_ll(Node* head) {
  int count = 0;
  Node* temp = head;
  while (temp != NULL) {
    count++;
    temp = temp->next;
  }

  return count;
}

// Remove extra zeros if remained in the linked list
void remove_extra_zero(Node** head) {
  Node* last_non_zero = NULL;
  Node* temp = *head;

  // to reach last non-zero number
  while (temp != NULL) {
    if (temp->digit != 0) {
      last_non_zero = temp;
    }
    temp = temp->next;
  }

  // removing zeros if exist
  temp = last_non_zero->next;
  last_non_zero->next = NULL;
  while (temp != NULL) {
    Node *node = temp->next;
    free(temp);
    temp = node;
  }
}

// Multiplication of two numbers
void multiplication(Node* ll1, Node* ll2, Node** result) {
  int len_ll1 = length_of_ll(ll1);
  int len_ll2 = length_of_ll(ll2);

  // storing zeros to result (prevent j-loop from breaking)
  for (int i = 0; i < len_ll1 + len_ll2; i++) {
    insert_at_end(result, 0);
  }

  // multiply each digit of ll1 with ll2 and store answer in result
  for (int i = 0; ll1; ll1 = ll1->next, i++) {
    int c = 0;
    Node* r = *result;
    // for multiplication shift
    for (int j = 0; j < i; j++) {
      r = r->next;
    }

    Node* ll2_temp = ll2;
    while (ll2_temp != NULL) {
      int sum = r->digit + (ll1->digit * ll2_temp->digit) + c;
      r->digit = sum % 10;
      c = sum / 10;
      r = r->next;
      ll2_temp = ll2_temp->next;
    }

    // add remaining carry to linked list
    while (c > 0) {
      int sum = r->digit + c;
      r->digit = sum % 10;
      c = sum / 10;
      r = r->next;
    }
  }

  remove_extra_zero(result);
}

// Count trailling zeros from head
int count_trailing_zeros(Node* head) {
  int count = 0;
  Node* temp = head;
  while (temp != NULL && temp->digit == 0) {
    count++;
    temp = temp->next;
  }

  return count;
}

// Display number as per requirement in assignment
void print_result(Node* head) {
  if(head == NULL) return;
  print_result(head->next);
  putchar('0' + head->digit);
}

// Check count of pattern (k) in linked list
int count_pattern(Node* head, char* k) {
  int k_len = strlen(k);
  if (k_len == 0) return 0;

  // reversing string for comparison
  char k_rev[KSIZE];
  for (int i = 0; i < k_len; i++) {
    k_rev[i] = k[k_len - 1 - i];
  }
  k_rev[k_len] = '\0';

  int count = 0;
  Node* s = head;
  
  while (s != NULL) {
    Node* h = s;
    int j = 0;

    while (h != NULL && j < k_len && h->digit == (k_rev[j] - '0')) {
      h = h->next;
      j++;
    }

    if (j == k_len) {
      count++;
    }
    s = s->next;
  }

  return count;
}

// Main function
int main()
{
  int T;
  scanf("%d", &T);  // Number of test cases

  for (int t = 0; t < T; t++) {
    int n;
    char k[4];
    scanf("%d %s", &n, k);

    Node* result = NULL;
    convert_num_to_ll(&result, 1);  // storing 1 to result;

    // calculating n# value as per assignment requirement
    // Already store 1 in result, so starting from i = 2
    for (int i = 2; i <= n; i++) {
      for (int j = 0; j < i; j++) {
        Node* x = NULL;
        convert_num_to_ll(&x, i);

        Node* temp = NULL;
        multiplication(result, x, &temp);
        
        free_memory(result);
        free_memory(x);

        result = temp;
        // free_memory(temp);
      }
    }

    // calculating trailing zeros & pattern count
    int p_count = count_pattern(result, k);
    int end_zeros = count_trailing_zeros(result);

    // printing output in given format
    printf("%d %d ", end_zeros, p_count);
    print_result(result);
    free_memory(result);
    printf("\n");
  }

  return 0;
}