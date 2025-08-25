#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 2048

/*---------------------------------------------------------*/
// To differentiate between AVL and BST
typedef enum {
  TREE_AVL = 0,
  TREE_BST = 1
} tree_t;

// Tree node
typedef struct Tree_Node {
  int data;
  int height;
  struct Tree_Node* left;
  struct Tree_Node* right;
} Tree_Node;

// Initializing tree with default = BST
static Tree_Node* root = NULL;
static tree_t current = TREE_BST;

// Find max of 2 numbers
int max_of_two(int num1, int num2) {
  if (num1 > num2) {
    return num1;
  } else {
    return num2;
  }
}

// Create new node
Tree_Node* create_node(int value) {
  Tree_Node* new_node = (Tree_Node*)malloc(sizeof(Tree_Node));
  new_node->data = value;
  new_node->height = 0;
  new_node->left = NULL;
  new_node->right = NULL;

  return new_node;
}

// Free memory of tree
void free_tree_memory(Tree_Node* root) {
  if (root == NULL) {
    return;
  }
  free_tree_memory(root->left);
  free_tree_memory(root->right);
  free(root);
}

// Get height of node
int height_of_node(Tree_Node* node) {
  if (node != NULL) {
    return node->height;
  } else {
    return -1;
  }
}

// Update height of node
void update_height(Tree_Node* node) {
  if (node == NULL) return;
  node->height = 1 + max_of_two(height_of_node(node->left), height_of_node(node->right));
}

/*---------------------------------------------------------*/
// STACK AND QUEUE FOR TRAVERSAL
typedef struct Link_Node {
  struct Tree_Node* ptr;
  struct Link_Node* next;
} Link_Node;

// STack
typedef struct Stack {
  Link_Node* top;
} Stack;

void st_init(Stack* st) { 
  st->top = NULL;
}

int st_is_empty(Stack* st) {
  return st->top == NULL; 
}

void push(Stack* st, Tree_Node* p) {
  if (p == NULL) return;
  Link_Node* new_node = (Link_Node*)malloc(sizeof(*new_node));
  new_node->ptr = p;
  new_node->next = st->top;
  st->top = new_node;
}

Tree_Node* pop(Stack* st) {
  if (st->top == NULL) return NULL;
  Link_Node* n = st->top;
  Tree_Node* p = n->ptr;
  st->top = n->next;
  free(n);
  return p;
}

// Queue
typedef struct Queue {
  Link_Node* front;
  Link_Node* rear;
} Queue;

void qu_init(Queue* qu) {
  qu->front = NULL;
  qu->rear = NULL;
}

int qu_empty(Queue* qu) {
  return qu->front == NULL;
}

void enqueue(Queue* qu, Tree_Node* p) {
  Link_Node* new_node = (Link_Node*)malloc(sizeof(*new_node));
  new_node->ptr = p;
  new_node->next = NULL;

  if (qu->rear == NULL) {
    qu->front = new_node;
    qu->rear = new_node;
  } else {
    qu->rear->next = new_node;
    qu->rear = new_node;
  }
}

Tree_Node* dequeue(Queue* qu) {
  if (qu->front == NULL) return NULL;

  Link_Node *n = qu->front;
  Tree_Node* p = n->ptr;
  qu->front = n->next;

  if (qu->front == NULL) {
    qu->rear = NULL;
  }
  free(n);
  return p;
}


/*---------------------------------------------------------*/
// BST TREE OPERATIONS
// Insert node
Tree_Node* bst_insert_node(Tree_Node* root, int value) {
  if (value <= 0) return root;
  if (root == NULL) return create_node(value);

  if (root->data < value) root->right = bst_insert_node(root->right, value);
  else if (root->data > value) root->left = bst_insert_node(root->left, value);
  else return root;

  update_height(root);
  return root;
}

// Delete node
Tree_Node* bst_delete_node(Tree_Node* root, int value) {
  if (value <= 0) return root;
  if (root == NULL) return root;

  if (root->data > value) {
    root->left = bst_delete_node(root->left, value);
  } else if (root->data < value) {
    root->right = bst_delete_node(root->right, value);
  } else {
    if (root->left == NULL && root->right == NULL) {
      free(root);
      return NULL;
    } else if (root->left != NULL && root->right == NULL) {
      Tree_Node* temp = root->left;
      free(root);
      return temp;
    } else if (root->left == NULL && root->right != NULL) {
      Tree_Node* temp = root->right;
      free(root);
      return temp;
    } else {
      Tree_Node* max_left_subtree = root->left;
      while (max_left_subtree != NULL && max_left_subtree->right != NULL) {
        max_left_subtree = max_left_subtree->right;
      }
      root->data = max_left_subtree->data;
      root->left = bst_delete_node(root->left, max_left_subtree->data);
    }
  }

  update_height(root);
  return root;
}

/*---------------------------------------------------------*/
// AVL TREE OPERATIONS
// Calculate node balance
int avl_tree_balance(Tree_Node* node) {
  if (node != NULL) {
    return height_of_node(node->left) - height_of_node(node->right);
  } else {
    return 0;
  }
}

// Left rotation
Tree_Node* avl_left_rotation(Tree_Node* node) {
  if (node == NULL || node->right == NULL) return node;

  Tree_Node* r = node->right;
  Tree_Node* t = r->left;
  r->left = node;
  node->right = t;

  update_height(node);
  update_height(r);

  return r;
}

// Right rotation
Tree_Node* avl_right_rotation(Tree_Node* node) {
  if (node == NULL || node->left == NULL) return node;

  Tree_Node* l = node->left;
  Tree_Node* t = l->right;
  l->right = node;
  node->left = t;

  update_height(node);
  update_height(l);

  return l;
}

// Insert node
Tree_Node* avl_insert_node(Tree_Node* root, int value) {
  if (value <= 0) return root;
  if (root == NULL) return create_node(value);

  if (root->data > value) root->left = avl_insert_node(root->left, value);
  else if (root->data < value) root->right = avl_insert_node(root->right, value);
  else return root;

  update_height(root);

  int balance = avl_tree_balance(root);

  if (balance > 1 && value < root->left->data)  return avl_right_rotation(root);
  if (balance < -1 && value > root->right->data) return avl_left_rotation(root);
  if (balance > 1 && value > root->left->data) {
    root->left = avl_left_rotation(root->left);
    return avl_right_rotation(root);
  }
  if (balance < -1 && value < root->right->data) {
    root->right = avl_right_rotation(root->right);
    return avl_left_rotation(root);
  }

  return root;
}

// Delete node
Tree_Node* avl_delete_node(Tree_Node* root, int value) {
  if (value <= 0) return root;
  if (root == NULL) return root;

  if (root->data > value) {
    root->left = avl_delete_node(root->left, value);
  } else if (root->data < value) {
    root->right = avl_delete_node(root->right, value);
  } else {
    if (root->left == NULL && root->right == NULL) {
      free(root);
      return NULL;
    } else if (root->left != NULL && root->right == NULL) {
      Tree_Node* temp = root->left;
      free(root);
      return temp;
    } else if (root->left == NULL && root->right != NULL) {
      Tree_Node* temp = root->right;
      free(root);
      return temp;
    } else {
      Tree_Node* max_left_subtree = root->left;
      while (max_left_subtree != NULL && max_left_subtree->right != NULL) {
        max_left_subtree = max_left_subtree->right;
      }
      root->data = max_left_subtree->data;
      root->left = avl_delete_node(root->left, max_left_subtree->data);
    }
  }

  update_height(root);

  int balance = avl_tree_balance(root);

  if (balance > 1 ) {
    int left_balance = avl_tree_balance(root->left);
    if (left_balance >= 0) root = avl_right_rotation(root);
    else {
      root->left = avl_left_rotation(root->left);
      root = avl_right_rotation(root);
    }
  } else if (balance < -1) {
    int right_balance = avl_tree_balance(root->right);
    if (right_balance <= 0) root = avl_left_rotation(root);
    else {
      root->right = avl_right_rotation(root->right);
      root = avl_left_rotation(root);
    }
  }

  return root;
}

/*---------------------------------------------------------*/
// FUNCTIONS TO WORK ON ANY TYPE OF TREE
// Setting type of tree
void set_type_of_tree(tree_t type) {
  current = type;
}

// Clear memory of tree
void tree_clean() {
  free_tree_memory(root);
  root = NULL;
}

// Insertion based on type of tree
void tree_insert_node(int value) {
  if (value <= 0) return;
  
  if (current == TREE_BST) root = bst_insert_node(root, value);
  else root = avl_insert_node(root, value);
}

// Deletion based on type of tree
void tree_delete_node(int value) {
  if (value <= 0) return;

  if (current == TREE_BST) root = bst_delete_node(root, value);
  else root = avl_delete_node(root, value);
}

// Search node (in any tree)
int tree_find_node(Tree_Node* root, int value) {
  if (value <= 0) return 0;
  if (root == NULL) return 0;

  if (root->data == value) return 1;
  if (root->data > value) return tree_find_node(root->left, value);
  else return tree_find_node(root->right, value);

  return 0;
}

/*---------------------------------------------------------*/
// TREE TRAVERSAL
// Inorder (recursive)
void inorder(Tree_Node* root, int* t) {
  if (root != NULL) {
    inorder(root->left, t);
    if (*t == 0) printf(" ");
    printf("%d", root->data);
    *t = 0;
    inorder(root->right, t);
  }
}

void inorder_traversal(Tree_Node* root) {
  int t = 1;
  inorder(root, &t);
  printf("\n");
}

// Postorder (recursive)
void postorder(Tree_Node* root, int* t) {
  if (root != NULL) {
    postorder(root->left, t);
    postorder(root->right, t);
    if (*t == 0) printf(" ");
    printf("%d", root->data);
    *t = 0;
  }
}

void postorder_traversal(Tree_Node* root) {
  int t = 1;
  postorder(root, &t);
  printf("\n");
}

// Preorder (iterative)
void preorder_traversal(Tree_Node* root) {
  Stack st;
  st_init(&st);
  int f = 1;

  if (root) {
    push(&st, root);
  }

  while (st_is_empty(&st) != 1) {
    Tree_Node* curr = pop(&st);

    if (f == 0) printf(" ");
    printf("%d", curr->data);
    f = 0;

    if (curr->right != NULL) push(&st, curr->right);
    if (curr->left != NULL) push(&st, curr->left);
  }

  printf("\n");
}

// Level order
void level_order_traversal(Tree_Node* root) {
  Queue qu;
  qu_init(&qu);
  int f = 1;

  if (root) {
    enqueue(&qu, root);
  }

  while (qu_empty(&qu) == 0) {
    Tree_Node* curr = dequeue(&qu);

    if (f == 0) printf(" ");
    printf("%d", curr->data);
    f = 0;

    if (curr->left != NULL) enqueue(&qu, curr->left);
    if (curr->right != NULL) enqueue(&qu, curr->right);
  }

  printf("\n");
}

// CALCULATIONS
// Number of nodes
int count_nodes(Tree_Node* root) {
  if (root == NULL) return 0;
  return 1 + count_nodes(root->left) + count_nodes(root->right);
}

// Count leaf nodes
int count_leaf_nodes(Tree_Node* root) {
  if (root == NULL) return 0;
  if (root->left == NULL && root->right == NULL) return 1;
  return count_leaf_nodes(root->left) + count_leaf_nodes(root->right);
}

// Calculate height
int tree_height(Tree_Node* root) {
  return root ? root->height : -1;
}

// Calculate width
int tree_width(Tree_Node* root) {
  Queue qu;
  qu_init(&qu);
  int max_width = 0;

  while(1) {
    int level = 0;
    for (Link_Node* i = qu.front; i != NULL; i = i->next) level++;
    if (level == 0) break;

    if (level > max_width) max_width = level;

    for (int i = 0; i < level; i++) {
      Tree_Node* curr = dequeue(&qu);
      if (curr->left) enqueue(&qu, curr->left);
      if (curr->right) enqueue(&qu, curr->right);
    }
  }

  return max_width;
}

// Calculate diameter
int tree_diameter(Tree_Node* root) {
  if (root == NULL) return 0;

  int l_height = height_of_node(root->left);
  int r_height = height_of_node(root->right);
  int total = l_height + r_height + 3;

  int l_diameter = tree_diameter(root->left);
  int r_diameter = tree_diameter(root->right);

  int max = total;
  if (l_diameter > max) max = l_diameter;
  if (r_diameter > max) max = r_diameter;

  return max;
}


// Calculate perimeter
void perimeter(Tree_Node* root) {
  
}

// MAIN
int main() {
  int Z;
  scanf("%d", &Z);
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF) {}

  char input[SIZE];
  for (int z = 0; z < Z; z++) {
    fgets(input, sizeof(input), stdin);

    char* token = strtok(input, " \t\n");
    char op = token[0];

    int args[SIZE];
    int m = 0;
    while ((token = strtok(NULL, " \t\n"))) {
      args[m++] = atoi(token);
    }

    switch (op)
    {
    case 'T': {
      tree_clean();
      set_type_of_tree(TREE_BST);
      for (int i = 0; i < m; i++) {
        if (args[i] > 0) tree_insert_node(args[i]);
        else if (args[i] < 0) tree_delete_node(-args[i]);
      }
    }
    break;

    case 'H': {
      tree_clean();
      set_type_of_tree(TREE_AVL);
      for (int i = 0; i < m; i++) {
        if (args[i] > 0) tree_insert_node(args[i]);
        else if (args[i] < 0) tree_insert_node(-args[i]);
      }
    }
    break;

    case 'A': {
      for (int i = 0; i < m; i++) {
        if (args[i] > 0) tree_insert_node(args[i]);
      }
    }
    break;

    case 'U': {
      for (int i = 0; i < m; i++) {
        if (args[i] > 0) tree_delete_node(args[i]);
      }
    }
    break;

    case 'F': {
      int x = (m >= 1 ? args[0] : -1);
      printf("%s\n", tree_find_node(root, x) ? "Yes" : "No");
    }
    break;

    case 'Q': printf("%d\n", count_leaf_nodes(root)); break;
    case 'N': printf("%d\n", count_nodes(root)); break;
    case 'P': preorder_traversal(root); break;
    case 'I': inorder_traversal(root); break;
    case 'S': postorder_traversal(root); break;
    case 'L': level_order_traversal(root); break;
    case 'D': printf("%d\n", tree_height(root)); break;
    case 'W': printf("%d\n", tree_width(root)); break;
    case 'X': printf("%d\n", tree_diameter(root)); break;
    
    default: break;
    }

  }
}