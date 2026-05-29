#include <stdio.h>
#include <stdlib.h>

typedef struct NodeTrain {
  int id;                 // data node
  struct NodeTrain *next; // pointer ke node berikutnya
} Node;

Node *createNode(int val) {
  Node *newnode = (Node *)malloc(sizeof(Node)); // alokasi memori
  if (!newnode) {                               // [FIX] cek jika malloc gagal
    printf("Alokasi memori gagal!\n");
    exit(1);
  }
  newnode->id = val;    // isi data
  newnode->next = NULL; // next kosong
  return newnode;
}

void pushTail(Node **head, Node **tail, int val) {
  Node *newnode = createNode(val); // buat node baru
  if (!*head)
    *head = *tail = newnode; // jika kosong, head & tail ke node baru
  else {
    (*tail)->next = newnode; // tail lama menunjuk ke node baru
    *tail = newnode;         // tail pindah ke node baru
  }
}

void pushHead(Node **head, Node **tail, int val) {
  Node *newnode = createNode(val); // buat node baru
  if (!*head)
    *head = *tail = newnode; // jika kosong
  else {
    newnode->next = *head; // node baru menunjuk ke head lama
    *head = newnode;       // head pindah ke node baru
  }
}

void viewNode(Node *head) {
  if (!head) {
    printf("Kosong\n");
    return;
  } // jika kosong
  Node *temp = head; // mulai dari head
  while (temp) {
    printf("%d ", temp->id);
    temp = temp->next;
  } // traversal
  printf("\n"); // [FIX] newline di akhir output
}

void pop(Node **head, Node **tail, int val) {
  if (!*head)
    return; // jika kosong

  Node *temp = *head, *prev = NULL; // temp = current, prev = sebelumnya

  while (temp && temp->id != val) {
    prev = temp;
    temp = temp->next;
  } // cari node

  if (!temp) {
    printf("Not Found");
    return;
  } // tidak ketemu

  if ((*head)->id == val) { // jika di head
    *head = temp->next;     // head maju ke next
    if (!*head)
      *tail = NULL; // jika jadi kosong
  } else {
    prev->next = temp->next; // skip node yang dihapus
    if (temp == *tail)
      *tail = prev; // jika hapus tail
  }

  free(temp); // hapus memori
}

// [TAMBAHAN] Bebaskan semua node — penting untuk menghindari memory leak
void freeAll(Node **head, Node **tail) {
  Node *temp = *head;
  while (temp) {
    Node *next = temp->next;
    free(temp);
    temp = next;
  }
  *head = *tail = NULL;
}

int main() {
  int N;
  scanf("%d", &N);

  Node *head = NULL, *tail = NULL;

  // Ganti isi loop sesuai kebutuhan programmu.
  for (int i = 0; i < N; i++) {
    int val;
    scanf("%d", &val);
    pushHead(&head, &tail, val); // tambah depan (node baru jadi head)
    pushTail(&head, &tail, val); // tambah belakang (node baru jadi tail)
    pop(&head, &tail, val);      // hapus node pertama dengan nilai val
  }

  viewNode(head);        // tampilkan isi list
  freeAll(&head, &tail); // [TAMBAHAN] bebaskan semua memori
  return 0;
}
