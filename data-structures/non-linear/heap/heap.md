# Heap (Max-Heap) — [`heap.c`](heap.c)

> **Heap** = *complete binary tree* yang disimpan dalam **array**, dengan satu aturan:
> **Max-Heap** → tiap **parent ≥ kedua anaknya**, jadi nilai **terbesar selalu di akar**.
> (Versi **Min-Heap** kebalikannya: parent ≤ anak, nilai terkecil di akar.)
> Karena pohonnya selalu "rapat" (terisi dari kiri), ia bisa dipetakan ke array
> tanpa pointer — cukup rumus indeks.

---

## 1. Konsep & Mekanisme

Heap adalah pohon, tapi **tidak butuh pointer**. Pohon di bawah ini:

```
            60
          /    \
        40      50
       /  \    /  \
     10   20  30   15
```

disimpan dalam array **per level, dari kiri ke kanan**:

```
indeks:   0    1    2    3    4    5    6
data:   [ 60 | 40 | 50 | 10 | 20 | 30 | 15 ]
          ^akar
```

Hubungan parent–anak cukup dihitung dari indeks (array mulai `0`):

| Dari indeks `i` | Rumus         | Contoh (`i = 1`, nilai 40) |
| --------------- | ------------- | -------------------------- |
| **parent**      | `(i - 1) / 2` | `(1-1)/2 = 0` → 60         |
| **anak kiri**   | `2*i + 1`     | `2*1+1 = 3` → 10           |
| **anak kanan**  | `2*i + 2`     | `2*1+2 = 4` → 20           |

- **Heap ≠ BST.** Heap hanya menjamin **parent vs anak** (vertikal), **bukan**
  urutan kiri–kanan. Jadi heap **tidak** untuk pencarian nilai sembarang;
  kekuatannya adalah **selalu tahu nilai maksimum dalam O(1)** (lihat akar).
- Dua operasi inti memperbaiki heap dengan cara "menggeser" elemen ke posisi benar:
  - **`siftUp`** — naikkan elemen baru sampai parent ≥ dia (dipakai saat **insert**).
  - **`siftDown`** — turunkan elemen sampai dia ≥ kedua anak (dipakai saat **pop**).

### ⚠️ Kenapa "complete" itu penting
Karena pohon selalu terisi rapat dari kiri, **tidak ada lubang** di array. Tinggi
pohon dijamin **⌊log₂ n⌋**, sehingga `siftUp`/`siftDown` paling lama hanya
menempuh setinggi pohon → **O(log n)**.

---

## 2. Struktur Data

```c
#define CAPACITY 100

typedef struct {
  int data[CAPACITY];
  int size; // jumlah elemen yang sedang dipakai
} Heap;
```

> Tidak ada `left`/`right` seperti di [BST](../trees/binary-search-tree/binary-search-tree.md) —
> "anak" dan "parent" dihitung lewat rumus indeks di atas. `size` menandai sampai
> mana array yang valid; sisanya dianggap kosong.

---

## 3. Penjelasan Code per Fungsi

### `initHeap`
- Set `size = 0`. Heap dianggap kosong; isi `data` lama tidak perlu dibersihkan
  karena hanya dibaca sampai indeks `size - 1`.

### `swap`
- Menukar isi dua `int` lewat pointer. Dipakai `siftUp` & `siftDown` saat
  menggeser elemen.

### `siftUp` (dipakai saat insert)
- Elemen baru ditaruh di **daun paling akhir**, lalu dibandingkan dengan parent.
- Selama `data[i] > data[parent]` → **tukar**, lalu lanjut naik (`i = parent`).
- Berhenti kalau sudah `<=` parent **atau** sudah sampai akar (`i == 0`).

### `siftDown` (dipakai saat pop)
- Dari indeks `i`, cari **anak terbesar** di antara `left` & `right`
  (cek dulu `< size` supaya tidak keluar batas array).
- Kalau parent sudah `>=` anak terbesar (`largest == i`) → **berhenti**.
- Kalau tidak, **tukar** dengan anak terbesar lalu lanjut turun (`i = largest`).

### `insert`
- Tolak kalau `size == CAPACITY` (heap penuh).
- Taruh `value` di `data[size]`, naikkan `size`, lalu `siftUp` dari indeks terakhir.

### `pop` — ambil & hapus nilai terbesar (akar)
Pakai flag `found` (pola yang sama seperti `pop` di [BST](../trees/binary-search-tree/binary-search-tree.md)/[AVL](../trees/avl-tree/avl-tree.md)):

1. Kalau `size == 0` → `*found = 0`, kembalikan `-1` (heap kosong).
2. Simpan `root = data[0]` (nilai terbesar yang akan dikembalikan).
3. **Daun terakhir** dipindah ke akar (`data[0] = data[size-1]`), `size--`.
4. `siftDown(0)` untuk menurunkan elemen itu sampai heap property pulih.

#### 🎞️ Diagram Insert (`siftUp`) — masukkan `60`
Taruh di daun akhir (indeks 5), lalu naik selama > parent:
```
        50                 50                 60        ← 60 sampai ke posisi benar
       /  \               /  \               /  \
     40    30   ──▶     40    60   ──▶     40    50
    / \   /            / \   /            / \   /
  10  20 (60)        10  20 30          10  20 30
  60>30 → tukar      60>50 → tukar      akar, berhenti
```

#### 🎞️ Diagram Pop (`siftDown`) — buang akar `60`
Daun terakhir (`15`) naik ke akar, lalu turun selama < anak terbesar:
```
        60                 15                 50               50
       /  \               /  \               /  \             /  \
     40    50   ──▶     40    50   ──▶     40    15   ──▶   40    30
    / \   /            / \                / \              / \
  10  20 (15)        10  20             10  20           10  20
  akar dibuang,    15<50 → tukar      15<30 → tukar     beres
  15 naik          (anak kanan)       dgn anak kanan
```

### `printHeap`
- Cetak `data[0..size-1]` apa adanya → urutan **level-order**, **bukan** urut menaik.
- Untuk lihat data terurut, lakukan **`pop` berulang** (lihat `main`).

---

## 4. Aturan Heap (Invariant)

> **INVARIANT UTAMA (Max-Heap):** untuk setiap indeks `i > 0` →
> `data[(i-1)/2] >= data[i]` (parent selalu ≥ anaknya).

Aturan turunan yang harus dijaga tiap operasi:

1. **Shape property** — pohon selalu *complete*: array terisi rapat `0..size-1`,
   tanpa lubang. `insert` selalu menambah di ujung, `pop` selalu mengisi lubang
   akar dengan daun terakhir → bentuk rapat ini terjaga otomatis.
2. **Heap property** — dipulihkan oleh `siftUp` (saat insert) dan `siftDown`
   (saat pop). Keduanya hanya menggeser sepanjang **satu jalur** akar↔daun.
3. **Akar = ekstrem** — `data[0]` selalu nilai terbesar (Max-Heap). Inilah satu-
   satunya nilai yang dijamin posisinya; sisanya tidak terurut.

---

## 5. Contoh Alur di `main`

```
Insert: 30 20 50 10 40 60 15
Isi heap (array):     60 40 50 10 20 30 15   ← akar = 60 (terbesar)

Pop sampai kosong  →  60 50 40 30 20 15 10   ← keluar URUT MENURUN
Pop saat kosong    →  "Heap kosong!"
```

> **Heap Sort dalam satu kalimat:** masukkan semua data ke Max-Heap, lalu `pop`
> berulang → otomatis keluar terurut menurun (untuk menaik tinggal dibalik /
> pakai Min-Heap). Total **O(n log n)**, bandingkan dengan
> [Merge Sort](../../../algorithms/sorting/merge-sort/merge-sort.md).

---

## 6. Ringkasan Kompleksitas

| Operasi              | Waktu      | Keterangan                          |
| -------------------- | ---------- | ----------------------------------- |
| Lihat max (`data[0]`)| O(1)       | cukup baca akar                     |
| `insert` (`siftUp`)  | O(log n)   | paling jauh setinggi pohon          |
| `pop` (`siftDown`)   | O(log n)   | paling jauh setinggi pohon          |
| `printHeap`          | O(n)       | sekadar lewati array                |
| Bangun dari n data   | O(n log n) | n kali insert (Heap Sort)           |
| Ruang                | O(n)       | satu array, tanpa pointer per node  |

> Tinggi heap = ⌊log₂ n⌋ dan **dijamin** (karena selalu *complete*) — tidak ada
> kasus "miring" seperti [BST polos](../trees/binary-search-tree/binary-search-tree.md).

---

## 7. Kapan Dipakai?

✅ **Cocok ketika:**
- Sering butuh **ambil nilai min/max** secara berulang → **Priority Queue**
  (penjadwalan task, Dijkstra, Prim, event simulation).
- Butuh **k elemen terbesar/terkecil** dari aliran data besar (top-k).
- Mau **sorting in-place** dengan jaminan O(n log n) → **Heap Sort**.

❌ **Hindari / pakai struktur lain ketika:**
- Butuh **cari nilai sembarang** atau data **terurut penuh** → heap tak bisa;
  pakai [BST](../trees/binary-search-tree/binary-search-tree.md) /
  [AVL](../trees/avl-tree/avl-tree.md) / [Red-Black Tree](../trees/red-black-tree/red-black-tree.md).
- Butuh successor/predecessor atau *range query* → pakai pohon terurut.

Lihat juga: [Binary Search Tree](../trees/binary-search-tree/binary-search-tree.md) (pohon **terurut**, beda tujuan), [Merge Sort](../../../algorithms/sorting/merge-sort/merge-sort.md) (sama-sama O(n log n)).
