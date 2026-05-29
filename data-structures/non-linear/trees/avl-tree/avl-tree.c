#include <stdio.h>
#include <stdlib.h>

/*
 * AVL Tree = BST yang selalu seimbang.
 * Tiap node punya "tinggi", dan selisih tinggi anak kiri & kanan
 * (balance factor) harus tetap di rentang -1, 0, +1.
 * Kalau melenceng, kita perbaiki dengan ROTASI.
 */

typedef struct NodeTree {
  int data;
  int height; // tinggi node (jumlah level dari node ini ke daun terjauh)
  struct NodeTree *left;
  struct NodeTree *right;
} Node;

/* Prototype fungsi */
Node *createNode(int value);
int height(Node *root);
int max(int a, int b);
void updateHeight(Node *root);
int balanceFactor(Node *root);
Node *rotateRight(Node *y);
Node *rotateLeft(Node *x);
Node *rebalance(Node *root);
Node *insert(Node *root, int value);
Node *findMin(Node *root);
Node *deleteNode(Node *root, int value, int *found);
Node *pop(Node *root, int value);
void inorder(Node *root);

/* Membuat node baru (daun selalu tinggi 1) */
Node *createNode(int value) {
  Node *newNode = (Node *)malloc(sizeof(Node));

  if (!newNode) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  newNode->data = value;
  newNode->height = 1;
  newNode->left = NULL;
  newNode->right = NULL;

  return newNode;
}

/* Tinggi node, aman untuk NULL (tinggi 0) */
int height(Node *root) { return root ? root->height : 0; }

int max(int a, int b) { return a > b ? a : b; }

/* Perbarui tinggi node dari tinggi anak-anaknya */
void updateHeight(Node *root) {
  root->height = 1 + max(height(root->left), height(root->right));
}

/* Balance factor = tinggi kiri - tinggi kanan */
int balanceFactor(Node *root) {
  return root ? height(root->left) - height(root->right) : 0;
}

/*
 * Rotasi kanan (untuk kasus berat di kiri / Left-Left):
 *
 *       y            x
 *      / \          / \
 *     x   T3  -->  T1  y
 *    / \              / \
 *   T1  T2          T2  T3
 */
Node *rotateRight(Node *y) {
  Node *x = y->left;
  Node *T2 = x->right;

  x->right = y;
  y->left = T2;

  updateHeight(y); // y di bawah, update duluan
  updateHeight(x);

  return x; // x jadi akar baru
}

/*
 * Rotasi kiri (untuk kasus berat di kanan / Right-Right):
 *
 *     x                y
 *    / \              / \
 *   T1  y     -->    x   T3
 *      / \          / \
 *     T2  T3       T1  T2
 */
Node *rotateLeft(Node *x) {
  Node *y = x->right;
  Node *T2 = y->left;

  y->left = x;
  x->right = T2;

  updateHeight(x);
  updateHeight(y);

  return y; // y jadi akar baru
}

/* Seimbangkan kembali node bila balance factor keluar dari [-1, 1] */
Node *rebalance(Node *root) {
  updateHeight(root);

  int bf = balanceFactor(root);

  /* Berat kiri */
  if (bf > 1) {
    if (balanceFactor(root->left) < 0)
      root->left = rotateLeft(root->left); // Left-Right -> jadikan Left-Left

    return rotateRight(root);
  }

  /* Berat kanan */
  if (bf < -1) {
    if (balanceFactor(root->right) > 0)
      root->right =
          rotateRight(root->right); // Right-Left -> jadikan Right-Right

    return rotateLeft(root);
  }

  return root; // sudah seimbang
}

/* Menambahkan data lalu seimbangkan di jalur balik rekursi */
Node *insert(Node *root, int value) {
  if (!root)
    return createNode(value);

  if (value < root->data)
    root->left = insert(root->left, value); // masuk ke kiri
  else if (value > root->data)
    root->right = insert(root->right, value); // masuk ke kanan
  else
    return root; // duplikat tidak disimpan

  return rebalance(root);
}

/* Mencari nilai terkecil pada subtree */
Node *findMin(Node *root) {
  if (!root)
    return NULL;

  while (root->left)
    root = root->left;

  return root;
}

/* Menghapus node lalu seimbangkan di jalur balik rekursi */
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

  return rebalance(root); // node bisa jadi tak seimbang setelah hapus
}

/* Wrapper delete */
Node *pop(Node *root, int value) {
  int found = 0;

  root = deleteNode(root, value, &found);

  if (!found)
    printf("Nilai %d tidak ditemukan!\n", value);

  return root;
}

/* Traversal inorder (hasilnya pasti urut menaik) */
void inorder(Node *root) {
  if (!root)
    return;

  inorder(root->left);
  printf("%d ", root->data);
  inorder(root->right);
}

int main(void) {
  Node *root = NULL;

  /* Sengaja masukkan urut menaik: di BST biasa jadi miring,
     di AVL otomatis seimbang lewat rotasi. */
  int values[] = {10, 20, 30, 40, 50, 25};

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    root = insert(root, values[i]);

  printf("Pohon awal (inorder): ");
  inorder(root);
  printf("\n");
  printf("Akar = %d, tinggi pohon = %d\n\n", root->data, height(root));

  printf("=== Hapus daun: 25 ===\n");
  root = pop(root, 25);

  printf("Hasil: ");
  inorder(root);
  printf("  (akar = %d)\n\n", root->data);

  printf("=== Hapus node 2 anak: 30 ===\n");
  root = pop(root, 30);

  printf("Hasil: ");
  inorder(root);
  printf("  (akar = %d)\n\n", root->data);

  printf("=== Hapus nilai 99 ===\n");
  root = pop(root, 99);

  printf("Hasil: ");
  inorder(root);
  printf("\n");

  return 0;
}
