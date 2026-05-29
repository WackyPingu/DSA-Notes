#include <stdio.h>
#include <stdlib.h>

typedef struct NodeTree {
  int data;
  struct NodeTree *left;
  struct NodeTree *right;
} Node;

/* Prototype fungsi */
Node *createNode(int value);
Node *insert(Node *root, int value);
Node *findMin(Node *root);
Node *deleteNode(Node *root, int value, int *found);
Node *pop(Node *root, int value);
void inorder(Node *root);

/* Membuat node baru */
Node *createNode(int value) {
  Node *newNode = (Node *)malloc(sizeof(Node));

  if (!newNode) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  newNode->data = value;
  newNode->left = NULL;
  newNode->right = NULL;

  return newNode;
}

/* Menambahkan data ke BST */
Node *insert(Node *root, int value) {
  if (!root)
    return createNode(value);

  if (value < root->data)
    root->left = insert(root->left, value); // masuk ke kiri
  else if (value > root->data)
    root->right = insert(root->right, value); // masuk ke kanan

  return root;
}

/* Mencari nilai terkecil pada subtree */
Node *findMin(Node *root) {
  if (!root)
    return NULL;

  while (root->left)
    root = root->left;

  return root;
}

/* Menghapus node dari BST */
Node *deleteNode(Node *root, int value, int *found) {
  if (!root)
    return NULL;

  if (value < root->data)
    root->left = deleteNode(root->left, value, found);

  else if (value > root->data)
    root->right = deleteNode(root->right, value, found);

  else {
    *found = 1; // data ditemukan

    /* Kasus 1: node daun */
    if (!root->left && !root->right) {
      free(root);
      return NULL;
    }

    /* Kasus 2: hanya anak kanan */
    if (!root->left) {
      Node *tmp = root->right;
      free(root);
      return tmp;
    }

    /* Kasus 3: hanya anak kiri */
    if (!root->right) {
      Node *tmp = root->left;
      free(root);
      return tmp;
    }

    /* Kasus 4: dua anak */
    Node *succ = findMin(root->right);

    root->data = succ->data;

    root->right = deleteNode(root->right, succ->data, found);
  }

  return root;
}

/* Wrapper delete */
Node *pop(Node *root, int value) {
  int found = 0;

  root = deleteNode(root, value, &found);

  if (!found)
    printf("Nilai %d tidak ditemukan!\n", value);

  return root;
}

/* Traversal inorder */
void inorder(Node *root) {
  if (!root)
    return;

  inorder(root->left);
  printf("%d ", root->data);
  inorder(root->right);
}

int main(void) {
  Node *root = NULL;

  int values[] = {50, 30, 70, 20, 40, 60, 80};

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    root = insert(root, values[i]);

  printf("Pohon awal: ");
  inorder(root);
  printf("\n\n");

  printf("=== Hapus daun: 20 ===\n");
  root = pop(root, 20);

  printf("Hasil: ");
  inorder(root);
  printf("\n\n");

  printf("=== Hapus node 1 anak: 30 ===\n");
  root = pop(root, 30);

  printf("Hasil: ");
  inorder(root);
  printf("\n\n");

  printf("=== Hapus node 2 anak: 50 ===\n");
  root = pop(root, 50);

  printf("Hasil: ");
  inorder(root);
  printf("\n\n");

  printf("=== Hapus nilai 99 ===\n");
  root = pop(root, 99);

  printf("Hasil: ");
  inorder(root);
  printf("\n");

  return 0;
}