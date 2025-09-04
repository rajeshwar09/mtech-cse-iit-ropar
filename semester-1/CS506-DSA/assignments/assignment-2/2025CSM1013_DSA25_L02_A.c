#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  int data;
  struct Node* next;
} Node;

void insert_at_beginning(Node** head, int value) {
  Node* new_node = (Node*)malloc(sizeof(Node));
  new_node->data = value;
  new_node->next = NULL;

  if (*head == NULL) {
    *head = new_node;
    return;
  }

  new_node->next = *head;
  *head = new_node;
  return;
}

void insert_at_end(Node** head, int value) {
  Node* new_node = (Node*)malloc(sizeof(Node));
  new_node->data = value;
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

void delete_first_node(Node** head) {
  if (*head == NULL) {
    printf("E ");
    return;
  }

  Node* temp = *head;
  *head = temp->next;

  free(temp);
  temp->next = NULL;
}

void delete_last_node(Node** head) {
  if (*head == NULL) {
    printf("E ");
    return;
  }

  if ((*head)->next == NULL) {
    Node* temp = *head;
    *head = temp->next;

    free(temp);
    temp->next = NULL;
    return;
  }

  Node* current = (*head)->next;
  Node* previous = *head;

  while (current->next != NULL) {
    current = current->next;
    previous = previous->next;
  }

  previous->next = NULL;
  free(current);
  current->next = NULL;
}

void delete_specific_node(Node** head, int value) {
  if (*head == NULL) {
    printf("E ");
    return;
  }

  Node* current = *head;
  Node* previous = NULL;
  int found = 0;

  while (current != NULL) {
    if (current->data == value) {
      found = 1;
      break;
    }
    previous = current;
    current = current->next;
  }

  if (found) {
    if (current == *head) {
      *head = current->next;
      free(current);
      current = NULL;
      return;
    }

    previous->next = current->next;
    free(current);
    current = NULL;
    return;
  } else {
    printf("N ");
  }

} 

void find_node(Node* head, int value) {
  if (head == NULL) {
    printf("E ");
    return;
  }

  Node* current = head;
  int position = 0;
  int found = 0;

  while (current != NULL) {
    position++;
    if (current->data == value) {
      found = 1;
      break;
    }

    current = current->next;
  }

  if (found != 0) {
    int diff = (current - head);
    printf("%d (%d) ", position, diff);
  } else {
    printf("N ");
  }
}

void display_linked_list(Node* head) {
  if (head == NULL) {
    printf("E ");
    return;
  }

  Node* temp = head;

  printf("[");
  while (temp->next != NULL) {
    printf("%d-", temp->data);
    temp = temp->next;
  }

  printf("%d] ", temp->data);
}

void size(Node* head) {
  Node* temp = head;
  int count = 0;

  while (temp != NULL) {
    count++;
    temp = temp->next;
  }

  printf("%d ", count);
}

void free_memory(Node** head) {
  Node* temp;
  while (*head != NULL) {
    temp = *head;
    *head = (*head)->next;
    free(temp);
    temp = NULL;
  }
}

int main(int argc, char const *argv[])
{
  int close_all = 0;
  int T;
  scanf("%d", &T);

  for (int t = 1; t <= T && !close_all; t++) {
    int X;
    int data;
    scanf("%d", &X);

    Node* head = NULL;

    printf("#%d ", t);

    for (int i = 0; i < X; i++) {
      int choice;
      scanf("%d", &choice);

      if (choice == 0) {
        close_all = 1;
        break;
      }

      switch (choice) {
        case 1:
          scanf("%d", &data);
          insert_at_beginning(&head, data);
        break;

        case 2:
          scanf("%d", &data);
          insert_at_end(&head, data);
        break;

        case 3:
          delete_first_node(&head);
        break;

        case 4:
          delete_last_node(&head);
        break;

        case 5:
          scanf("%d", &data);
          delete_specific_node(&head, data);
        break;

        case 6:
          scanf("%d", &data);
          find_node(head, data);
        break;

        case 7:
          size(head);
        break;

        case 8:
          display_linked_list(head);
        break;
        
        default:
          printf("W ");
        break;
      }
    }

    if (close_all == 1) {
      break;
    }

    printf("\n");
    free_memory(&head);
  }
  
  return 0;
}
