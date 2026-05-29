#include <stdio.h>

/*
 * Merge Sort = strategi DIVIDE & CONQUER:
 *   1. PECAH array jadi dua bagian sampai tiap bagian tinggal 1 elemen.
 *   2. GABUNG (merge) dua bagian terurut menjadi satu bagian terurut.
 * Selalu O(n log n), tapi butuh array bantu (tidak in-place).
 */

/*
 * Gabungkan dua sub-array TERURUT yang bersebelahan:
 *   kiri  = arr[left..mid]
 *   kanan = arr[mid+1..right]
 * menjadi satu rentang arr[left..right] yang terurut.
 */
void merge(int arr[], int left, int mid, int right) {
  int n1 = mid - left + 1; // jumlah elemen bagian kiri
  int n2 = right - mid;    // jumlah elemen bagian kanan

  int L[n1], R[n2]; // array bantu (VLA C99) — salinan tiap bagian

  for (int i = 0; i < n1; i++)
    L[i] = arr[left + i]; // salin bagian kiri
  for (int j = 0; j < n2; j++)
    R[j] = arr[mid + 1 + j]; // salin bagian kanan

  // Gabung: bandingkan kepala L dan R, ambil yang lebih kecil dulu.
  int i = 0, j = 0, k = left; // i->L, j->R, k->posisi tulis di arr
  while (i < n1 && j < n2) {
    if (L[i] <= R[j])     // '<=' menjaga STABIL (elemen sama, kiri dulu)
      arr[k++] = L[i++];
    else
      arr[k++] = R[j++];
  }

  // Salin sisa yang belum sempat masuk (salah satu pasti sudah habis).
  while (i < n1)
    arr[k++] = L[i++];
  while (j < n2)
    arr[k++] = R[j++];
}

/* Pecah rekursif lalu gabung di jalur balik */
void mergeSort(int arr[], int left, int right) {
  if (left < right) {                    // basis: 1 elemen (left==right) sudah terurut
    int mid = left + (right - left) / 2; // hindari overflow (vs (left+right)/2)

    mergeSort(arr, left, mid);      // urutkan separuh kiri
    mergeSort(arr, mid + 1, right); // urutkan separuh kanan
    merge(arr, left, mid, right);   // gabungkan keduanya
  }
}

int main() {
  int arr[] = {12, 11, 13, 5, 6, 7};
  int n = sizeof(arr) / sizeof(arr[0]); // hitung jumlah elemen otomatis

  mergeSort(arr, 0, n - 1);

  for (int i = 0; i < n; i++)
    printf("%d ", arr[i]);
  printf("\n");

  return 0;
}
