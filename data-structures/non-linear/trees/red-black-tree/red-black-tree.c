#include <stdio.h>
#include <stdlib.h>

/*
 * Red-Black Tree = BST seimbang dengan aturan WARNA.
 * Aturannya cuma 5:
 *   1. Tiap node merah (RED) atau hitam (BLACK).
 *   2. Akar selalu BLACK.
 *   3. Daun NULL dianggap BLACK (di sini diwakili sentinel NIL).
 *   4. Node RED tidak boleh punya anak RED (tidak ada merah beruntun).
 *   5. Tiap jalur dari satu node ke daun punya jumlah node BLACK yang sama.
 *
 * Selama 5 aturan ini terjaga, tinggi pohon dijamin O(log n).
 * Kalau insert/delete melanggar, kita perbaiki dengan ROTASI + GANTI WARNA.
 */

typedef enum { RED, BLACK } Color;

typedef struct NodeTree {
  int data;
  Color color;
  struct NodeTree *left;
  struct NodeTree *right;
  struct NodeTree *parent; // perlu parent agar fixup bisa naik ke atas
} Node;

/*
 * NIL = satu node sentinel hitam pengganti semua NULL.
 * Trik ini menghindari banyak cek "if (x != NULL)" saat fixup.
 */
Node NIL_NODE = {0, BLACK, NULL, NULL, NULL};
Node *NIL = &NIL_NODE;

/* Prototype fungsi */
Node *createNode(int value);
void rotateLeft(Node **root, Node *x);
void rotateRight(Node **root, Node *x);
void insertFixup(Node **root, Node *z);
void insert(Node **root, int value);
Node *findMin(Node *root);
void transplant(Node **root, Node *u, Node *v);
void deleteFixup(Node **root, Node *x);
void deleteNode(Node **root, int value);
void pop(Node **root, int value);
void inorder(Node *root);

/* Membuat node baru, selalu lahir RED dengan anak NIL */
Node *createNode(int value) {
  Node *newNode = (Node *)malloc(sizeof(Node));

  if (!newNode) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  newNode->data = value;
  newNode->color = RED; // node baru selalu merah dulu
  newNode->left = NIL;
  newNode->right = NIL;
  newNode->parent = NIL;

  return newNode;
}

/*
 * Rotasi kiri pada x: anak kanan (y) naik, x turun jadi anak kiri y.
 *
 *     x                y
 *    / \              / \
 *   a   y     -->    x   c
 *      / \          / \
 *     b   c        a   b
 */
void rotateLeft(Node **root, Node *x) {
  Node *y = x->right;

  x->right = y->left; // anak kiri y pindah jadi anak kanan x
  if (y->left != NIL)
    y->left->parent = x;

  y->parent = x->parent; // y mengganti posisi x
  if (x->parent == NIL)
    *root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  y->left = x; // x turun jadi anak kiri y
  x->parent = y;
}

/* Rotasi kanan: cermin dari rotateLeft */
void rotateRight(Node **root, Node *x) {
  Node *y = x->left;

  x->left = y->right;
  if (y->right != NIL)
    y->right->parent = x;

  y->parent = x->parent;
  if (x->parent == NIL)
    *root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;

  y->right = x;
  x->parent = y;
}

/*
 * Perbaiki pelanggaran setelah insert.
 * Masalahnya cuma satu: z (merah) mungkin punya parent merah (langgar aturan 4).
 * Kita selesaikan dengan melihat warna "paman" (uncle).
 */
void insertFixup(Node **root, Node *z) {
  while (z->parent->color == RED) {
    if (z->parent == z->parent->parent->left) {
      Node *uncle = z->parent->parent->right;

      if (uncle->color == RED) {
        /* Kasus 1: paman merah -> cukup tukar warna, lalu naik */
        z->parent->color = BLACK;
        uncle->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) {
          /* Kasus 2: bentuk zig-zag -> luruskan dulu */
          z = z->parent;
          rotateLeft(root, z);
        }
        /* Kasus 3: bentuk lurus -> tukar warna lalu rotasi */
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        rotateRight(root, z->parent->parent);
      }
    } else {
      /* Cermin: parent ada di kanan kakek */
      Node *uncle = z->parent->parent->left;

      if (uncle->color == RED) {
        z->parent->color = BLACK;
        uncle->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          rotateRight(root, z);
        }
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        rotateLeft(root, z->parent->parent);
      }
    }
  }

  (*root)->color = BLACK; // aturan 2: akar selalu hitam
}

/* Menambahkan data: insert ala BST biasa, lalu panggil fixup */
void insert(Node **root, int value) {
  Node *z = createNode(value);

  Node *parent = NIL;
  Node *cur = *root ? *root : NIL;

  while (cur != NIL) {
    parent = cur;
    if (value < cur->data)
      cur = cur->left; // masuk ke kiri
    else if (value > cur->data)
      cur = cur->right; // masuk ke kanan
    else {
      free(z); // duplikat tidak disimpan
      return;
    }
  }

  z->parent = parent;
  if (parent == NIL)
    *root = z; // pohon tadinya kosong
  else if (value < parent->data)
    parent->left = z;
  else
    parent->right = z;

  insertFixup(root, z);
}

/* Mencari node nilai terkecil pada subtree */
Node *findMin(Node *root) {
  while (root->left != NIL)
    root = root->left;

  return root;
}

/* Mengganti subtree u dengan subtree v (memindah pointer parent saja) */
void transplant(Node **root, Node *u, Node *v) {
  if (u->parent == NIL)
    *root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
}

/*
 * Perbaiki pelanggaran setelah delete.
 * x membawa "ekstra hitam" yang harus diteruskan ke atas sampai
 * bisa diserap. Kita lihat warna "saudara" (sibling) x.
 */
void deleteFixup(Node **root, Node *x) {
  while (x != *root && x->color == BLACK) {
    if (x == x->parent->left) {
      Node *w = x->parent->right; // saudara

      if (w->color == RED) {
        /* Kasus 1: saudara merah -> ubah jadi kasus saudara hitam */
        w->color = BLACK;
        x->parent->color = RED;
        rotateLeft(root, x->parent);
        w = x->parent->right;
      }

      if (w->left->color == BLACK && w->right->color == BLACK) {
        /* Kasus 2: kedua anak saudara hitam -> warnai saudara merah, naik */
        w->color = RED;
        x = x->parent;
      } else {
        if (w->right->color == BLACK) {
          /* Kasus 3: anak kanan saudara hitam -> putar dulu */
          w->left->color = BLACK;
          w->color = RED;
          rotateRight(root, w);
          w = x->parent->right;
        }
        /* Kasus 4: anak kanan saudara merah -> selesai dengan rotasi */
        w->color = x->parent->color;
        x->parent->color = BLACK;
        w->right->color = BLACK;
        rotateLeft(root, x->parent);
        x = *root; // selesai
      }
    } else {
      /* Cermin: x adalah anak kanan */
      Node *w = x->parent->left;

      if (w->color == RED) {
        w->color = BLACK;
        x->parent->color = RED;
        rotateRight(root, x->parent);
        w = x->parent->left;
      }

      if (w->right->color == BLACK && w->left->color == BLACK) {
        w->color = RED;
        x = x->parent;
      } else {
        if (w->left->color == BLACK) {
          w->right->color = BLACK;
          w->color = RED;
          rotateLeft(root, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = BLACK;
        w->left->color = BLACK;
        rotateRight(root, x->parent);
        x = *root;
      }
    }
  }

  x->color = BLACK;
}

/* Menghapus node bernilai value (cara CLRS dengan transplant) */
void deleteNode(Node **root, int value) {
  /* Cari node target dulu */
  Node *z = *root ? *root : NIL;
  while (z != NIL && z->data != value) {
    if (value < z->data)
      z = z->left;
    else
      z = z->right;
  }

  if (z == NIL) {
    printf("Nilai %d tidak ditemukan!\n", value);
    return;
  }

  Node *y = z;             // y = node yang benar-benar dilepas dari posisinya
  Node *x;                 // x = node yang naik menggantikan y
  Color yOriginal = y->color;

  if (z->left == NIL) {
    /* Tidak ada anak kiri: x = anak kanan */
    x = z->right;
    transplant(root, z, z->right);
  } else if (z->right == NIL) {
    /* Tidak ada anak kanan: x = anak kiri */
    x = z->left;
    transplant(root, z, z->left);
  } else {
    /* Dua anak: ambil successor (terkecil di subtree kanan) */
    y = findMin(z->right);
    yOriginal = y->color;
    x = y->right;

    if (y->parent == z) {
      x->parent = y; // penting saat x kebetulan NIL
    } else {
      transplant(root, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }

    transplant(root, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color; // y mewarisi warna z
  }

  free(z);

  /* Kalau yang terlepas tadi hitam, aturan ke-5 bisa rusak -> fixup */
  if (yOriginal == BLACK)
    deleteFixup(root, x);
}

/* Wrapper delete */
void pop(Node **root, int value) { deleteNode(root, value); }

/* Traversal inorder, sekalian tampilkan warna tiap node */
void inorder(Node *root) {
  if (root == NIL || root == NULL)
    return;

  inorder(root->left);
  printf("%d(%s) ", root->data, root->color == RED ? "M" : "H");
  inorder(root->right);
}

int main(void) {
  Node *root = NULL;

  int values[] = {10, 20, 30, 40, 50, 25};

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    insert(&root, values[i]);

  printf("Keterangan: (M) = merah, (H) = hitam\n\n");

  printf("Pohon awal (inorder): ");
  inorder(root);
  printf("\nAkar = %d\n\n", root->data);

  printf("=== Hapus daun: 25 ===\n");
  pop(&root, 25);

  printf("Hasil: ");
  inorder(root);
  printf("  (akar = %d)\n\n", root->data);

  printf("=== Hapus node 2 anak: 30 ===\n");
  pop(&root, 30);

  printf("Hasil: ");
  inorder(root);
  printf("  (akar = %d)\n\n", root->data);

  printf("=== Hapus nilai 99 ===\n");
  pop(&root, 99);

  printf("Hasil: ");
  inorder(root);
  printf("\n");

  return 0;
}
