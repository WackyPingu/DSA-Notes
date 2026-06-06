#include <stdio.h>
#include <stdlib.h>

/*
 * Max-Heap = complete binary tree yang disimpan dalam ARRAY.
 * Aturan (heap property): tiap parent >= kedua anaknya,
 * jadi nilai TERBESAR selalu berada di akar (indeks 0).
 *
 * Rumus indeks (array mulai dari 0):
 *   parent(i) = (i - 1) / 2
 *   left(i)   = 2 * i + 1
 *   right(i)  = 2 * i + 2
 */

#define CAPACITY 100

typedef struct {
  int data[CAPACITY];
  int size; // jumlah elemen yang sedang dipakai
} Heap;

/* Prototype fungsi */
void initHeap(Heap *heap);
void swap(int *a, int *b);
void siftUp(Heap *heap, int i);
void siftDown(Heap *heap, int i);
void insert(Heap *heap, int value);
int pop(Heap *heap, int *found);
void printHeap(Heap *heap);

/* Inisialisasi heap kosong */
void initHeap(Heap *heap) { heap->size = 0; }

/* Tukar isi dua variabel */
void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

/* Naikkan elemen indeks i selama lebih besar dari parent (dipakai saat insert) */
void siftUp(Heap *heap, int i) {
  while (i > 0) {
    int parent = (i - 1) / 2;

    if (heap->data[i] <= heap->data[parent])
      break; // sudah <= parent, posisi sudah benar

    swap(&heap->data[i], &heap->data[parent]);
    i = parent; // lanjut naik
  }
}

/* Turunkan elemen indeks i selama lebih kecil dari anak (dipakai saat pop) */
void siftDown(Heap *heap, int i) {
  while (1) {
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int largest = i; // anggap parent yang terbesar dulu

    if (left < heap->size && heap->data[left] > heap->data[largest])
      largest = left;

    if (right < heap->size && heap->data[right] > heap->data[largest])
      largest = right;

    if (largest == i)
      break; // parent sudah >= kedua anak, berhenti

    swap(&heap->data[i], &heap->data[largest]);
    i = largest; // lanjut turun ke anak yang ditukar
  }
}

/* Menambahkan data ke heap */
void insert(Heap *heap, int value) {
  if (heap->size == CAPACITY) {
    printf("Heap penuh!\n");
    return;
  }

  heap->data[heap->size] = value; // taruh di daun paling akhir
  heap->size++;
  siftUp(heap, heap->size - 1); // naikkan ke posisi yang benar
}

/* Mengambil & menghapus nilai terbesar (akar) */
int pop(Heap *heap, int *found) {
  if (heap->size == 0) {
    *found = 0;
    return -1; // heap kosong
  }

  *found = 1;
  int root = heap->data[0]; // nilai terbesar ada di akar

  heap->size--;
  heap->data[0] = heap->data[heap->size]; // daun terakhir pindah ke akar
  siftDown(heap, 0); // turunkan sampai heap property pulih

  return root;
}

/* Cetak isi array heap (urutan level-order, bukan urut menaik) */
void printHeap(Heap *heap) {
  for (int i = 0; i < heap->size; i++)
    printf("%d ", heap->data[i]);
}

int main(void) {
  Heap heap;
  initHeap(&heap);

  int values[] = {30, 20, 50, 10, 40, 60, 15};

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    insert(&heap, values[i]);

  printf("Isi heap (array): ");
  printHeap(&heap);
  printf("\n");
  printf("Nilai terbesar (akar) = %d\n\n", heap.data[0]);

  /* Pop berulang -> keluar dari besar ke kecil (inti dari Heap Sort) */
  printf("=== Pop sampai kosong (hasilnya urut menurun) ===\n");
  int found;
  while (1) {
    int max = pop(&heap, &found);

    if (!found)
      break;

    printf("%d ", max);
  }
  printf("\n\n");

  printf("=== Pop saat heap sudah kosong ===\n");
  pop(&heap, &found);

  if (!found)
    printf("Heap kosong!\n");

  return 0;
}
