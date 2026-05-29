# Bubble Sort — [`bubble-sort.c`](bubble-sort.c)

> **Bubble Sort** = algoritma urut paling sederhana. Caranya: bandingkan tiap
> **pasangan elemen bersebelahan**, lalu **tukar** kalau urutannya salah. Setiap
> satu putaran penuh, elemen terbesar "menggelembung" (*bubble up*) ke ujung kanan —
> seperti gelembung udara naik ke permukaan air.

---

## 1. Konsep & Mekanisme

- Bandingkan `arr[j]` dengan `arr[j+1]`. Kalau `arr[j] > arr[j+1]` → **tukar**.
- Ulangi sepanjang array (satu *pass*). Setelah 1 pass, **1 elemen terbesar** pasti
  sudah di posisi akhir yang benar (paling kanan).
- Karena itu area yang perlu dicek menyusut tiap pass: `n - i - 1`.

### 🎞️ Diagram satu *pass* (data `5 2 8 1 9`)

```
[5  2] 8  1  9   → 5>2 tukar →  2  5  8  1  9
 2 [5  8] 1  9   → 5<8 aman  →  2  5  8  1  9
 2  5 [8  1] 9   → 8>1 tukar →  2  5  1  8  9
 2  5  1 [8  9]  → 8<9 aman  →  2  5  1  8  9
                                          ▲ 9 sudah di posisi akhir ✔
```

### Jalannya semua *pass*

```
Awal : 5 2 8 1 9
Pass1: 2 5 1 8 9   (9 fix)
Pass2: 2 1 5 8 9   (8 fix)
Pass3: 1 2 5 8 9   (5 fix)
Pass4: 1 2 5 8 9   tanpa tukaran → swapped=false → BERHENTI (early exit)
```

---

## 2. Penjelasan Code

### `bubbleSort(int arr[], int n)`
- **Loop luar `i`** (`0 .. n-2`): jumlah *pass*. Tiap pass menetapkan 1 elemen di kanan.
- **Loop dalam `j`** (`0 .. n-i-2`): bandingkan pasangan bersebelahan. Batas `n-i-1`
  karena `i` elemen di kanan **sudah** terurut, tak perlu dicek lagi.
- **Tukar (swap)** pakai variabel `temp` saat `arr[j] > arr[j+1]`.

### `[OPTIMASI]` — flag `swapped` (early exit)
```c
bool swapped = false;
... if (tukar) swapped = true; ...
if (!swapped) break;   // satu pass tanpa tukaran = sudah terurut
```
- Tanpa ini, Bubble Sort **selalu** O(n²) meski data sudah urut.
- Dengan ini, **kasus terbaik** (data sudah urut) jadi **O(n)** — cukup 1 pass.
  Inilah yang membuat Bubble Sort bersifat **adaptif**.

---

## 3. Sifat & Aturan Algoritma

| Sifat | Nilai | Penjelasan |
| ----- | ----- | ---------- |
| **Stabil?** | ✅ Ya | pakai `>` (bukan `>=`) → elemen bernilai sama **tidak** saling lewat |
| **In-place?** | ✅ Ya | hanya butuh 1 variabel bantu (`temp`), tanpa array tambahan → ruang O(1) |
| **Adaptif?** | ✅ Ya (versi ini) | berhenti lebih cepat kalau data hampir terurut, berkat flag `swapped` |
| **Berbasis perbandingan?** | ✅ Ya | hanya membandingkan & menukar elemen |

> **Aturan stabilitas (penting):** gunakan `arr[j] > arr[j+1]`. Kalau diganti `>=`,
> elemen yang **sama nilainya** akan ikut ditukar → urutan aslinya berubah →
> algoritma jadi **tidak stabil**. Untuk Bubble Sort, jaga tetap `>`.

---

## 4. Ringkasan Kompleksitas

| Kasus | Perbandingan | Tukaran | Waktu | Kapan terjadi |
| ----- | ------------ | ------- | ----- | ------------- |
| **Terbaik** | O(n) | 0 | **O(n)** | data sudah terurut (berkat early exit) |
| **Rata-rata** | O(n²) | O(n²) | **O(n²)** | data acak |
| **Terburuk** | O(n²) | O(n²) | **O(n²)** | data terurut **terbalik** |
| **Ruang** | — | — | **O(1)** | in-place |

---

## 5. Kapan Dipakai?

✅ **Cocok ketika:**
- **Belajar** konsep dasar sorting & swap (algoritma pengantar paling intuitif).
- Data **sangat kecil** (n ≤ ~10) atau **hampir terurut** (early exit bikin cepat).

❌ **Hindari ketika:**
- Data besar → O(n²) sangat lambat. Pakai [Merge Sort](../merge-sort/merge-sort.md) (O(n log n)) atau Quick Sort.
- Butuh performa produksi → hampir selalu ada pilihan lebih baik.

> **Bubble vs algoritma O(n²) lain:** Bubble Sort biasanya **paling lambat** secara
> praktik karena jumlah tukarannya banyak. Insertion Sort & Selection Sort umumnya
> lebih cepat walau sama-sama O(n²).

| Aspek | **Bubble** | [Merge Sort](../merge-sort/merge-sort.md) |
| ----- | ---------- | -------------- |
| Waktu rata-rata | O(n²) | O(n log n) |
| Ruang | O(1) | O(n) |
| Stabil | ✅ | ✅ |
| Cocok untuk | belajar / data kecil | data besar |

Lihat juga: [Merge Sort](../merge-sort/merge-sort.md).
