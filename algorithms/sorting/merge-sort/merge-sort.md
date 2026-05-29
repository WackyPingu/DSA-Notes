# Merge Sort — [`merge-sort.c`](merge-sort.c)

> **Merge Sort** = algoritma urut **Divide & Conquer** (pecah & taklukkan):
> **PECAH** array jadi dua terus-menerus sampai tersisa 1 elemen, lalu **GABUNG**
> (*merge*) potongan-potongan terurut itu kembali jadi satu yang terurut.
> Selalu **O(n log n)** — tidak peduli data acak atau terbalik.

---

## 1. Konsep & Mekanisme

Dua fase:
1. **Divide (pecah):** belah rentang di `mid` → urus kiri & kanan secara rekursif.
   Berhenti saat rentang tinggal **1 elemen** (`left == right`) — yang otomatis "terurut".
2. **Conquer / Merge (gabung):** satukan dua bagian yang **sudah terurut** menjadi satu.

> Kunci efisiensinya: menggabung dua bagian **yang sudah terurut** itu murah — cukup
> satu kali jalan (O(n)), karena tinggal membandingkan elemen terdepan tiap bagian.

### 🎞️ Diagram Pohon Rekursi (`12 11 13 5 6 7`)

```
                 [12 11 13 5 6 7]
                /                \
        [12 11 13]              [5 6 7]            ← DIVIDE (pecah)
        /        \             /      \
     [12]     [11 13]       [5]      [6 7]
              /     \                /    \
           [11]    [13]           [6]    [7]
─────────────────────────────────────────────────  (sudah 1 elemen, mulai naik)
           [11 13]                [6 7]            ← MERGE (gabung terurut)
        \        /             \      /
        [11 12 13]              [5 6 7]
                \                /
                 [5 6 7 11 12 13]                  ← hasil akhir terurut ✔
```

### 🎞️ Diagram satu `merge` (gabung `[11 12 13]` + `[5 6 7]`)

```
L: 11 12 13      R: 5 6 7
   i↑               j↑              bandingkan L[i] vs R[j], ambil yang kecil
─────────────────────────────────────────────
11 vs 5  → 5   |  11 vs 6 → 6  |  11 vs 7 → 7  |  R habis → sisa L: 11 12 13
hasil: 5 6 7 11 12 13
```

---

## 2. Penjelasan Code

### `mergeSort(arr, left, right)` — fase DIVIDE
```c
if (left < right) {                    // basis: 1 elemen sudah terurut, stop
  int mid = left + (right - left) / 2; // titik tengah
  mergeSort(arr, left, mid);           // urutkan separuh kiri
  mergeSort(arr, mid + 1, right);      // urutkan separuh kanan
  merge(arr, left, mid, right);        // gabung di jalur balik rekursi
}
```
- **`mid = left + (right - left) / 2`**, bukan `(left + right) / 2` → mencegah
  **integer overflow** saat `left + right` melebihi batas `int` (aman untuk array besar).

### `merge(arr, left, mid, right)` — fase GABUNG
1. Hitung ukuran dua bagian: `n1 = mid-left+1`, `n2 = right-mid`.
2. **Salin** ke array bantu `L[]` dan `R[]` (perlu salinan karena kita menimpa `arr`).
3. **Gabung**: pakai 3 indeks — `i`→L, `j`→R, `k`→posisi tulis di `arr`.
   Ambil elemen terkecil di antara `L[i]` & `R[j]`, tulis ke `arr[k]`.
4. **Salin sisa**: salah satu bagian pasti habis lebih dulu; sisanya disalin langsung.

> **`L[i] <= R[j]` (pakai `<=`, bukan `<`)** menjaga **STABILITAS**: kalau nilai sama,
> elemen dari **kiri** (`L`) diambil dulu → urutan relatif aslinya terjaga.

---

## 3. Sifat & Aturan Algoritma

| Sifat | Nilai | Penjelasan |
| ----- | ----- | ---------- |
| **Stabil?** | ✅ Ya | berkat `L[i] <= R[j]` (elemen sama, kiri didahulukan) |
| **In-place?** | ❌ Tidak | butuh array bantu `L`/`R` → ruang ekstra O(n) |
| **Adaptif?** | ❌ Tidak | tetap O(n log n) walau data sudah terurut |
| **Berbasis perbandingan?** | ✅ Ya | membandingkan elemen saat menggabung |

> **Aturan stabilitas:** jaga `<=` di pembanding merge. Kalau diganti `<`, elemen
> bernilai sama dari kanan bisa "menyalip" → tidak stabil lagi.

> ⚠️ **Catatan teknis:** `int L[n1], R[n2];` adalah **VLA** (Variable Length Array,
> fitur C99). Untuk array sangat besar ini berisiko *stack overflow*; versi
> produksi biasanya pakai `malloc`. Untuk belajar, VLA sudah cukup.

---

## 4. Ringkasan Kompleksitas

| Kasus | Waktu | Penjelasan |
| ----- | ----- | ---------- |
| **Terbaik** | **O(n log n)** | tetap memecah & menggabung penuh |
| **Rata-rata** | **O(n log n)** | sama |
| **Terburuk** | **O(n log n)** | sama → **konsisten**, ini kelebihan utamanya |
| **Ruang** | **O(n)** | array bantu + O(log n) tumpukan rekursi |

> **Kenapa `n log n`?** Ada `log n` level pemecahan (array dibelah dua tiap kali), dan
> tiap level total kerja menggabung = O(n). Jadi `n × log n`.

---

## 5. Kapan Dipakai?

✅ **Cocok ketika:**
- Butuh **jaminan O(n log n)** di semua kondisi (tak ada kasus terburuk O(n²) seperti Quick Sort).
- Butuh sorting **stabil** (mis. urutkan data per beberapa kunci berurutan).
- **Data besar** atau **linked list** (merge sort sangat pas untuk linked list — tanpa butuh akses indeks acak).
- Sorting eksternal (data terlalu besar untuk muat di RAM, diproses per blok).

❌ **Kurang ideal ketika:**
- Memori sangat terbatas → butuh ekstra O(n); pilih algoritma in-place (Quick Sort/Heap Sort).
- Data kecil → overhead rekursi tak sebanding; [Bubble Sort](../bubble-sort/bubble-sort.md)/Insertion Sort cukup.

| Aspek | [Bubble Sort](../bubble-sort/bubble-sort.md) | **Merge Sort** | Quick Sort |
| ----- | --------------- | -------------- | ---------- |
| Waktu rata-rata | O(n²) | O(n log n) | O(n log n) |
| Waktu terburuk | O(n²) | **O(n log n)** | O(n²) |
| Ruang | O(1) | O(n) | O(log n) |
| Stabil | ✅ | ✅ | ❌ (umumnya) |
| In-place | ✅ | ❌ | ✅ |

Lihat juga: [Bubble Sort](../bubble-sort/bubble-sort.md).
