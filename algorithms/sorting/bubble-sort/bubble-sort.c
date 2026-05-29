#include <stdio.h>
#include <stdbool.h>

/*
 * Bubble Sort = bandingkan PASANGAN bersebelahan, tukar bila urutannya salah.
 * Tiap satu putaran luar, elemen terbesar "menggelembung" (bubble) ke kanan.
 */
void bubbleSort(int arr[], int n) {
  for (int i = 0; i < n - 1; i++) {       // n-1 putaran (pass)
    bool swapped = false;                 // [OPTIMASI] pantau ada tukaran atau tidak

    // setiap pass, elemen terbesar terdorong ke ujung kanan,
    // jadi area yang perlu dicek mengecil: (n - i - 1)
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {          // urutan salah (kiri > kanan) -> tukar
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
        swapped = true;
      }
    }

    // [OPTIMASI] jika satu pass penuh tanpa tukaran -> sudah terurut -> berhenti.
    // Membuat kasus terbaik (data sudah urut) jadi O(n), bukan O(n^2).
    if (!swapped)
      break;
  }
}

int main() {
  int arr[] = {5, 2, 8, 1, 9};
  int n = sizeof(arr) / sizeof(arr[0]); // hitung jumlah elemen otomatis

  bubbleSort(arr, n);

  for (int i = 0; i < n; i++)
    printf("%d ", arr[i]);
  printf("\n");

  return 0;
}
