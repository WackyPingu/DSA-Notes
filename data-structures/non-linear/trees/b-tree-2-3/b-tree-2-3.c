#include <stdio.h>
#include <stdlib.h>

/*
 * 2-3 Tree = B-Tree paling kecil (orde 3).
 * Aturannya:
 *   - Tiap node menyimpan 1 atau 2 kunci (key).
 *   - Node dengan 1 kunci punya 2 anak (disebut 2-node).
 *   - Node dengan 2 kunci punya 3 anak (disebut 3-node).
 *   - Semua DAUN ada di level yang sama -> pohon selalu seimbang sempurna.
 *
 * Trik utamanya:
 *   - INSERT: selalu masuk ke daun. Kalau daun jadi penuh (3 kunci),
 *     PECAH (split): kunci tengah naik ke parent. Bisa merembet ke atas
 *     sampai akar (di situ tinggi pohon bertambah).
 *   - DELETE: selalu hapus dari daun (kunci internal ditukar dulu dengan
 *     successor). Kalau daun jadi kosong (underflow), PINJAM dari saudara
 *     atau GABUNG (merge). Bisa merembet ke atas.
 */

typedef struct NodeTree {
  int keys[3];               // maks 2 kunci; slot ke-3 hanya sementara saat overflow
  int nKeys;                 // jumlah kunci terpakai (0, 1, atau 2)
  int leaf;                  // 1 jika daun
  struct NodeTree *child[4]; // maks 3 anak; slot ke-4 hanya sementara saat overflow
} Node;

/* Prototype fungsi */
Node *allocNode(int leaf);
Node *createNode(int value);
void insertKeyAt(Node *node, int key, Node *rightChild);
int insertRec(Node *node, int value, int *upKey, Node **upRight);
Node *insert(Node *root, int value);
void removeKeyAt(Node *node, int i);
void removeKeyAndChild(Node *node, int ki, int ci);
void fixUnderflow(Node *parent, int ci);
int deleteRec(Node *node, int value, int *found);
Node *pop(Node *root, int value);
void inorder(Node *root);

/* Membuat node kosong */
Node *allocNode(int leaf) {
  Node *newNode = (Node *)malloc(sizeof(Node));

  if (!newNode) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  newNode->nKeys = 0;
  newNode->leaf = leaf;
  for (int i = 0; i < 4; i++)
    newNode->child[i] = NULL;

  return newNode;
}

/* Membuat daun berisi satu kunci */
Node *createNode(int value) {
  Node *newNode = allocNode(1);

  newNode->keys[0] = value;
  newNode->nKeys = 1;

  return newNode;
}

/*
 * Sisipkan kunci ke node sambil menjaga urutan.
 * rightChild = anak baru di sisi kanan kunci (NULL untuk daun).
 */
void insertKeyAt(Node *node, int key, Node *rightChild) {
  int i = node->nKeys;

  while (i > 0 && node->keys[i - 1] > key) {
    node->keys[i] = node->keys[i - 1];     // geser kunci ke kanan
    node->child[i + 1] = node->child[i];   // geser anak ke kanan
    i--;
  }

  node->keys[i] = key;
  node->child[i + 1] = rightChild;
  node->nKeys++;
}

/*
 * Sisip rekursif. Mengembalikan 1 bila node PECAH; saat itu kunci tengah
 * dititipkan di *upKey dan node kanan baru di *upRight (untuk dipasang parent).
 */
int insertRec(Node *node, int value, int *upKey, Node **upRight) {
  if (node->leaf) {
    for (int j = 0; j < node->nKeys; j++)
      if (node->keys[j] == value)
        return 0; // duplikat tidak disimpan

    insertKeyAt(node, value, NULL);
  } else {
    int i = 0;
    while (i < node->nKeys && value > node->keys[i])
      i++;

    if (i < node->nKeys && value == node->keys[i])
      return 0; // duplikat

    int k;
    Node *r;
    if (!insertRec(node->child[i], value, &k, &r))
      return 0; // anak tidak pecah -> tidak ada yang naik

    insertKeyAt(node, k, r); // pasang kunci yang naik dari anak
  }

  if (node->nKeys < 3)
    return 0; // belum penuh, aman

  /* Overflow (3 kunci): pecah jadi dua, kunci tengah naik */
  *upKey = node->keys[1];

  Node *right = allocNode(node->leaf);
  right->nKeys = 1;
  right->keys[0] = node->keys[2];
  right->child[0] = node->child[2];
  right->child[1] = node->child[3];

  node->nKeys = 1; // node lama menyisakan keys[0] dengan child[0], child[1]

  *upRight = right;
  return 1;
}

/* Menambahkan data; jika akar pecah, buat akar baru (tinggi pohon naik) */
Node *insert(Node *root, int value) {
  if (!root)
    return createNode(value);

  int upKey;
  Node *upRight;

  if (insertRec(root, value, &upKey, &upRight)) {
    Node *newRoot = allocNode(0);
    newRoot->nKeys = 1;
    newRoot->keys[0] = upKey;
    newRoot->child[0] = root;
    newRoot->child[1] = upRight;
    root = newRoot;
  }

  return root;
}

/* Hapus kunci di indeks i (geser kunci sisanya ke kiri) */
void removeKeyAt(Node *node, int i) {
  for (int j = i; j < node->nKeys - 1; j++)
    node->keys[j] = node->keys[j + 1];

  node->nKeys--;
}

/* Hapus kunci ke-ki dan anak ke-ci dari node (dipakai saat merge) */
void removeKeyAndChild(Node *node, int ki, int ci) {
  for (int j = ki; j < node->nKeys - 1; j++)
    node->keys[j] = node->keys[j + 1];

  for (int j = ci; j < node->nKeys; j++)
    node->child[j] = node->child[j + 1];

  node->nKeys--;
}

/*
 * Perbaiki child[ci] yang kosong (0 kunci) setelah delete.
 * Urutan usaha: pinjam dari saudara kiri -> pinjam dari saudara kanan -> merge.
 */
void fixUnderflow(Node *parent, int ci) {
  Node *child = parent->child[ci];

  /* Pinjam dari saudara KIRI yang punya 2 kunci */
  if (ci > 0 && parent->child[ci - 1]->nKeys == 2) {
    Node *left = parent->child[ci - 1];

    child->child[1] = child->child[0];          // beri ruang di depan
    child->keys[0] = parent->keys[ci - 1];      // kunci parent turun ke child
    child->child[0] = left->child[left->nKeys]; // anak terkanan left pindah
    child->nKeys = 1;

    parent->keys[ci - 1] = left->keys[left->nKeys - 1]; // kunci kanan left naik
    left->nKeys--;
    return;
  }

  /* Pinjam dari saudara KANAN yang punya 2 kunci */
  if (ci < parent->nKeys && parent->child[ci + 1]->nKeys == 2) {
    Node *right = parent->child[ci + 1];

    child->keys[0] = parent->keys[ci];   // kunci parent turun ke child
    child->child[1] = right->child[0];   // anak terkiri right pindah
    child->nKeys = 1;

    parent->keys[ci] = right->keys[0];   // kunci kiri right naik
    right->keys[0] = right->keys[1];     // rapatkan right
    right->child[0] = right->child[1];
    right->child[1] = right->child[2];
    right->nKeys--;
    return;
  }

  /* Tidak bisa pinjam -> MERGE child dengan satu saudara (yang pasti 1 kunci) */
  if (ci > 0) {
    /* Gabung child ke saudara kiri */
    Node *left = parent->child[ci - 1];

    left->keys[1] = parent->keys[ci - 1]; // kunci parent turun
    left->child[2] = child->child[0];     // satu-satunya anak child ikut
    left->nKeys = 2;

    removeKeyAndChild(parent, ci - 1, ci);
    free(child);
  } else {
    /* Gabung saudara kanan ke child */
    Node *right = parent->child[ci + 1];

    child->keys[0] = parent->keys[ci]; // kunci parent turun
    child->keys[1] = right->keys[0];
    child->child[1] = right->child[0];
    child->child[2] = right->child[1];
    child->nKeys = 2;

    removeKeyAndChild(parent, ci, ci + 1);
    free(right);
  }
}

/*
 * Hapus rekursif. Mengembalikan 1 bila node ini jadi kosong (underflow)
 * sehingga parent perlu memperbaikinya lewat fixUnderflow.
 */
int deleteRec(Node *node, int value, int *found) {
  int i = 0;
  while (i < node->nKeys && value > node->keys[i])
    i++;

  if (i < node->nKeys && value == node->keys[i]) {
    *found = 1; // data ditemukan

    if (node->leaf) {
      removeKeyAt(node, i);
    } else {
      /* Internal: tukar dengan successor (kunci terkecil di subtree kanan) */
      Node *succ = node->child[i + 1];
      while (!succ->leaf)
        succ = succ->child[0];

      node->keys[i] = succ->keys[0];

      if (deleteRec(node->child[i + 1], node->keys[i], found))
        fixUnderflow(node, i + 1);
    }
  } else {
    if (node->leaf)
      return 0; // tidak ketemu di daun

    if (deleteRec(node->child[i], value, found))
      fixUnderflow(node, i);
  }

  return node->nKeys == 0;
}

/* Wrapper delete; tangani kasus akar jadi kosong (tinggi pohon turun) */
Node *pop(Node *root, int value) {
  if (!root) {
    printf("Nilai %d tidak ditemukan!\n", value);
    return NULL;
  }

  int found = 0;
  deleteRec(root, value, &found);

  if (!found)
    printf("Nilai %d tidak ditemukan!\n", value);

  if (root->nKeys == 0) {
    Node *tmp = root->leaf ? NULL : root->child[0];
    free(root);
    root = tmp;
  }

  return root;
}

/* Traversal inorder (hasilnya pasti urut menaik) */
void inorder(Node *root) {
  if (!root)
    return;

  for (int i = 0; i < root->nKeys; i++) {
    inorder(root->child[i]); // subtree sebelum kunci ke-i
    printf("%d ", root->keys[i]);
  }

  inorder(root->child[root->nKeys]); // subtree paling kanan
}

int main(void) {
  Node *root = NULL;

  int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35};

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    root = insert(root, values[i]);

  printf("Pohon awal (inorder): ");
  inorder(root);
  printf("\n\n");

  printf("=== Hapus dari daun: 10 ===\n");
  root = pop(root, 10);

  printf("Hasil: ");
  inorder(root);
  printf("\n\n");

  printf("=== Hapus kunci internal: 30 ===\n");
  root = pop(root, 30);

  printf("Hasil: ");
  inorder(root);
  printf("\n\n");

  printf("=== Hapus hingga merge: 60 ===\n");
  root = pop(root, 60);

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
