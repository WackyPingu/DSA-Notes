# 2-3 Tree (B-Tree orde 3) — [`b-tree-2-3.c`](b-tree-2-3.c)

> **2-3 Tree** = B-Tree paling kecil. Tiap node boleh punya **1 atau 2 kunci**,
> dan **semua daun berada di level yang sama** → pohon **selalu seimbang sempurna**.
> Berbeda dari [AVL](../avl-tree/avl-tree.md)/[Red-Black Tree](../red-black-tree/red-black-tree.md) yang menyeimbangkan lewat rotasi, 2-3 Tree
> menyeimbangkan dengan **memecah (split)** dan **menggabung (merge)** node.

---

## 1. Konsep & Mekanisme

### Dua jenis node

```
   2-node (1 kunci, 2 anak)        3-node (2 kunci, 3 anak)
        ┌───┐                        ┌───┬───┐
        │ M │                        │ A │ B │
        └─┬─┴─┬─┘                    └┬──┴┬──┴┬┘
       <M │   │ >M               <A │ A<x<B│ >B
                                    │      │
```

| Jenis    | Jumlah kunci | Jumlah anak | Aturan urutan |
| -------- | ------------ | ----------- | ------------- |
| **2-node** | 1 (`M`)     | 2           | kiri `< M` < kanan |
| **3-node** | 2 (`A < B`) | 3           | `< A` ‖ `A..B` ‖ `> B` |

### Dua trik penyeimbang

- **INSERT** → selalu masuk ke **daun**. Kalau daun jadi penuh (3 kunci = *overflow*),
  **PECAH (split)**: kunci **tengah naik** ke parent. Bisa merembet ke atas; kalau
  sampai akar pecah, **tinggi pohon bertambah**.
- **DELETE** → selalu hapus dari **daun** (kunci internal ditukar dulu dengan
  *successor*). Kalau daun jadi kosong (*underflow*), **PINJAM** dari saudara atau
  **GABUNG (merge)**. Bisa merembet ke atas; kalau akar jadi kosong, **tinggi turun**.

> **Karena daun selalu sejajar**, pencarian/insert/hapus **dijamin O(log n)** —
> tak ada kasus terburuk miring seperti [BST](../binary-search-tree/binary-search-tree.md).

---

## 2. Struktur Data

```c
typedef struct NodeTree {
  int keys[3];          // maks 2 kunci; slot ke-3 hanya SEMENTARA saat overflow
  int nKeys;            // jumlah kunci terpakai: 0, 1, atau 2
  int leaf;             // 1 jika daun
  struct NodeTree *child[4]; // maks 3 anak; slot ke-4 SEMENTARA saat overflow
} Node;
```

> **Kenapa array berukuran 3 & 4?** Saat overflow, sebuah node sempat menampung
> **3 kunci & 4 anak** sebelum dipecah. Slot ekstra itu ruang kerja sementara.

---

## 3. Penjelasan Code per Fungsi

### `allocNode(leaf)` & `createNode(value)`
- `allocNode`: node kosong (`nKeys = 0`, semua `child = NULL`).
- `createNode`: daun berisi satu kunci (`keys[0] = value`, `nKeys = 1`).

### `insertKeyAt(node, key, rightChild)` — sisip terurut
- Geser kunci & anak yang lebih besar ke kanan, lalu tempatkan `key` di posisi benar.
- `rightChild` = anak baru di **kanan** kunci (NULL untuk daun; terisi saat kunci naik dari split).

### `insertRec` — sisip rekursif (return 1 bila node PECAH)
1. **Daun**: cek duplikat → kalau baru, `insertKeyAt(value, NULL)`.
2. **Internal**: cari anak yang tepat (`i`), rekursi turun.
   - Kalau anak **tidak pecah** (`return 0`) → tak ada yang naik, selesai.
   - Kalau anak **pecah** → pasang kunci naik `k` dan anak kanan `r` via `insertKeyAt`.
3. **Cek overflow**: kalau `nKeys < 3` → aman (`return 0`).
4. **Split (overflow, 3 kunci):**
   - Kunci tengah `keys[1]` dititip ke `*upKey` (akan naik).
   - Node kanan baru menampung `keys[2]` (+ `child[2]`, `child[3]`).
   - Node lama menyisakan `keys[0]` (+ `child[0]`, `child[1]`), `nKeys = 1`.
   - `*upRight = right`, `return 1`.

```
   [10 | 20 | 30]   overflow!
         │ split (tengah = 20 naik)
         ▼
        [20]
        /   \
     [10]   [30]
```

### `insert` — wrapper, tangani akar pecah
- Pohon kosong → `createNode`.
- Kalau `insertRec` mengembalikan 1 (akar pecah) → buat **akar baru** berisi `upKey`,
  dengan anak kiri = root lama, anak kanan = `upRight`. **Tinggi pohon naik 1.**

### `removeKeyAt(node, i)` & `removeKeyAndChild(node, ki, ci)`
- `removeKeyAt`: hapus kunci ke-`i`, geser sisanya ke kiri.
- `removeKeyAndChild`: hapus kunci **dan** satu pointer anak (dipakai saat merge).

### `fixUnderflow(parent, ci)` — perbaiki `child[ci]` yang kosong
Urutan usaha: **pinjam kiri → pinjam kanan → merge**.

| Strategi | Syarat | Yang terjadi |
| -------- | ------ | ------------ |
| **Pinjam saudara KIRI** | saudara kiri punya 2 kunci | kunci parent **turun** ke child; kunci terkanan saudara kiri **naik** ke parent (rotasi) |
| **Pinjam saudara KANAN**| saudara kanan punya 2 kunci | kunci parent **turun** ke child; kunci terkiri saudara kanan **naik** ke parent |
| **MERGE** | kedua saudara cuma 1 kunci | gabung child + 1 saudara + 1 kunci parent jadi satu 3-node; parent kehilangan 1 kunci (bisa underflow lagi → merembet) |

> **Kenapa pinjam dulu sebelum merge?** Pinjam hanya memindah 1 kunci (murah,
> tak mengurangi kunci parent). Merge mengurangi kunci parent → bisa memicu
> underflow beruntun ke atas, jadi dipakai sebagai pilihan terakhir.

#### 🎞️ Diagram PINJAM (rotasi lewat parent)

Child kosong, saudara kiri punya 2 kunci → "putar" lewat parent:
```
        [30 | 60]                       [25 | 60]
       /    |    \                     /    |    \
   [10 25] [  ]  [80]    ───▶     [10]   [30]   [80]
              ↑ kosong               ↑      ↑
   30 turun ke child, 25 (kunci kanan saudara kiri) naik isi parent
```

#### 🎞️ Diagram MERGE (saat tak ada yang bisa dipinjam)

Child kosong, kedua saudara cuma 1 kunci → gabung child + 1 saudara + 1 kunci parent:
```
        [30 | 60]                    [60]            ← parent kehilangan 1 kunci
       /    |    \                  /    \
    [10]  [  ]  [80]   ───▶    [10 30]   [80]
            ↑ kosong            └─ 30 turun dari parent, gabung jadi 3-node
```
> Kalau parent ikut kosong setelah merge → underflow **merembet ke atas**. Bila akar
> sampai kosong, anak tunggalnya jadi akar baru → **tinggi pohon turun 1**.

### `deleteRec` — hapus rekursif (return 1 bila node jadi kosong)
1. Cari posisi `i`.
2. **Ketemu di node ini** (`found = 1`):
   - **Daun** → `removeKeyAt`.
   - **Internal** → cari **successor** (kunci terkecil di subtree kanan, terus `child[0]`
     sampai daun), salin ke posisi ini, lalu hapus successor dari subtree kanan.
     Kalau subtree itu underflow → `fixUnderflow`.
3. **Tidak di node ini**:
   - Daun → `return 0` (tak ketemu).
   - Internal → rekursi ke `child[i]`; kalau underflow → `fixUnderflow`.
4. `return node->nKeys == 0` (beri tahu parent apakah node ini kosong).

### `pop` — wrapper, tangani akar kosong
- Jalankan `deleteRec`; kalau `found == 0` → pesan "tidak ditemukan".
- Kalau **akar jadi kosong** (`nKeys == 0`): akar baru = `child[0]` (kalau internal)
  atau `NULL` (kalau daun). `free` akar lama. **Tinggi pohon turun 1.**

### `inorder`
- Untuk tiap kunci: cetak subtree kiri kunci, lalu kuncinya; terakhir subtree paling kanan.
- Pola: `child[0] key[0] child[1] key[1] child[2]`. Hasil **terurut menaik**.

---

## 4. Aturan 2-3 Tree (Invariant)

> **INVARIANT UTAMA:**
> 1. Tiap node punya **1 atau 2 kunci** (selain saat sementara overflow/underflow).
> 2. 2-node punya **2 anak**, 3-node punya **3 anak** (node internal).
> 3. **Semua daun di level yang sama** (tinggi seragam → seimbang sempurna).
> 4. Urutan kunci terjaga: anak ke-`i` berada di antara `keys[i-1]` dan `keys[i]`.

Aturan operasional:

| Situasi | Aturan |
| ------- | ------ |
| **Overflow** (daun/node jadi 3 kunci) | **SPLIT**: kunci tengah naik ke parent |
| **Underflow** (node jadi 0 kunci) | **PINJAM** dari saudara ber-2-kunci, kalau tidak bisa → **MERGE** |
| **Hapus kunci internal** | tukar dulu dengan **successor** di daun, baru hapus dari daun |
| **Akar pecah** | buat akar baru → **tinggi naik** |
| **Akar kosong** | anak tunggal jadi akar → **tinggi turun** |

> Catatan: penyeimbangan **selalu lewat akar** (split/merge merembet), bukan rotasi
> lokal seperti [AVL](../avl-tree/avl-tree.md). Itu sebabnya semua daun tetap sejajar.

---

## 5. Contoh Alur di `main`

```
Insert: 50 30 70 20 40 60 80 10 25 35
Pohon awal (inorder): 10 20 25 30 35 40 50 60 70 80

Hapus 10  → hapus langsung dari daun
Hapus 30  → kunci internal → tukar successor → hapus di daun
Hapus 60  → memicu MERGE (pinjam gagal → gabung)
Hapus 99  → tidak ditemukan
```

---

## 6. Ringkasan Kompleksitas

| Operasi | Waktu | Ruang | Catatan |
| ------- | ----- | ----- | ------- |
| Cari    | **O(log n)** | O(h) | dijamin — semua daun sejajar |
| Insert  | **O(log n)** | O(h) | split bisa merembet sampai akar |
| Delete  | **O(log n)** | O(h) | pinjam/merge bisa merembet sampai akar |
| Inorder | O(n) | O(h) | hasil terurut menaik |

> **Tak ada kasus terburuk O(n).** Keseimbangan sempurna (semua daun selevel) dijaga
> *by construction* lewat split/merge, bukan diperbaiki belakangan seperti rotasi [AVL](../avl-tree/avl-tree.md).

---

## 7. Kapan Dipakai?

✅ **Cocok ketika:**
- Ingin memahami **dasar B-Tree** sebelum ke ordo lebih besar (2-3-4 Tree, B+Tree).
- Butuh keseimbangan **selalu sempurna** dengan logika tanpa rotasi.

❌ **Di dunia nyata** jarang dipakai langsung; yang dipakai adalah **B-Tree/B+Tree**
(ordo besar, ratusan kunci per node):
- **Indeks database** (MySQL InnoDB, PostgreSQL) dan **filesystem** (NTFS, ext4, Btrfs).
- **Kenapa ordo besar?** Tiap node = satu blok disk. Node lebar → pohon **pendek** →
  jumlah baca disk (yang mahal) minimal. 2-3 Tree ini versi mini untuk belajar konsepnya.

> **2-3 Tree vs pohon biner ([AVL](../avl-tree/avl-tree.md)/[Red-Black Tree](../red-black-tree/red-black-tree.md)):** pohon biner optimal untuk
> data **di memori (RAM)**; B-Tree optimal untuk data **di disk/blok** karena
> meminimalkan operasi I/O.

| Aspek | [BST](../binary-search-tree/binary-search-tree.md) | [AVL](../avl-tree/avl-tree.md) | [Red-Black Tree](../red-black-tree/red-black-tree.md) | **2-3 Tree** |
| ----- | ------- | ------- | ----------------- | ------------ |
| Cara seimbang | — | rotasi (tinggi) | rotasi + warna | split / merge |
| Daun sejajar | ❌ | ❌ | ❌ | ✅ selalu |
| Kunci per node | 1 | 1 | 1 | 1–2 |
| Habitat ideal | dasar | RAM (read-heavy) | RAM (umum) | disk/blok (via B-Tree) |

Lihat juga: [BST](../binary-search-tree/binary-search-tree.md), [AVL](../avl-tree/avl-tree.md), [Red-Black Tree](../red-black-tree/red-black-tree.md) — pendekatan lain untuk pohon seimbang.
