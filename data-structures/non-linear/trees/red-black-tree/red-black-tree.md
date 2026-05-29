# Red-Black Tree — [`red-black-tree.c`](red-black-tree.c)

> **Red-Black Tree** = [BST](../binary-search-tree/binary-search-tree.md) seimbang yang menjaga keseimbangan lewat **WARNA**
> (merah/hitam) + rotasi, bukan lewat tinggi seperti [AVL](../avl-tree/avl-tree.md). Selama 5 aturan warna
> terjaga, tinggi pohon dijamin **O(log n)**. Implementasi ini mengikuti **CLRS**
> (buku *Introduction to Algorithms*) lengkap dengan **sentinel NIL**.

---

## 1. Lima Aturan Warna (INTI — wajib hafal)

> 1. **Setiap node berwarna RED atau BLACK.**
> 2. **Akar (root) selalu BLACK.**
> 3. **Setiap daun NULL dianggap BLACK** (di sini diwakili sentinel `NIL`).
> 4. **Node RED tidak boleh punya anak RED** → *tidak ada dua merah beruntun.*
> 5. **Setiap jalur dari sebuah node ke daun-daun di bawahnya punya jumlah node BLACK yang SAMA** (disebut *black-height*).

| Aturan dilanggar | Kapan terjadi | Cara perbaikan |
| ---------------- | ------------- | -------------- |
| **Aturan 4** (merah beruntun) | setelah **insert** (node baru RED) | `insertFixup` (lihat warna *uncle*) |
| **Aturan 5** (black-height beda) | setelah **delete** node BLACK | `deleteFixup` (lihat warna *sibling*) |
| **Aturan 2** (akar harus hitam) | bisa rusak saat fixup | dipaksa hitam di akhir fixup |

> **Kenapa O(log n)?** Karena tiap jalur punya jumlah hitam sama (aturan 5) dan
> merah tak boleh beruntun (aturan 4), jalur terpanjang **maksimal 2×** jalur
> terpendek. Tidak ada cabang yang bisa jauh lebih panjang dari yang lain.

---

## 2. Sentinel NIL — trik penting

```c
Node NIL_NODE = {0, BLACK, NULL, NULL, NULL};
Node *NIL = &NIL_NODE;
```

- **Satu** node hitam dipakai sebagai pengganti **semua** `NULL` (daun & parent akar).
- **Kenapa?** Saat fixup kita sering akses `x->color`, `w->left->color`, dst.
  Kalau pakai `NULL` asli, harus cek `if (x != NULL)` di mana-mana. Dengan `NIL`
  yang punya `color = BLACK`, akses selalu aman dan kode jauh lebih ringkas.

---

## 3. Struktur Data

```c
typedef enum { RED, BLACK } Color;

typedef struct NodeTree {
  int data;
  Color color;
  struct NodeTree *left;
  struct NodeTree *right;
  struct NodeTree *parent;  // PERLU parent agar fixup bisa "naik" ke atas
} Node;
```

> Beda dengan [BST](../binary-search-tree/binary-search-tree.md)/[AVL](../avl-tree/avl-tree.md): ada **`parent`**. Fixup bergerak dari node ke atas
> (ke leluhur), jadi butuh tahu siapa parent & kakek tiap node.

---

## 4. Rotasi (sama prinsip dengan AVL, tapi jaga `parent`)

### `rotateLeft(x)` — anak kanan `y` naik, `x` turun jadi anak kiri `y`
```
     x                 y
    / \               / \
   a   y     ───▶    x   c
      / \           / \
     b   c         a   b
```
Tiga sambungan parent yang harus diperbarui: `y->left` (b) pindah ke `x->right`,
`y` menggantikan posisi `x` di parent-nya, lalu `x` jadi `y->left`.

### `rotateRight(x)` — cermin dari `rotateLeft`.

---

## 5. INSERT & `insertFixup`

### `insert`
1. Sisip seperti **BST biasa** (turun kiri/kanan, duplikat diabaikan).
2. Node baru `z` lahir **RED** dengan anak `NIL`.
3. Panggil `insertFixup(root, z)`.

> **Kenapa node baru selalu RED?** Menambah node RED **tidak** mengubah black-height
> (aturan 5 aman). Satu-satunya risiko adalah aturan 4 (kalau parent juga RED) —
> itu yang diperbaiki fixup. Kalau lahir BLACK, aturan 5 langsung rusak di banyak jalur.

### `insertFixup` — selama `parent z` RED (langgar aturan 4)
Lihat warna **uncle** (paman = saudara dari parent):

| Kasus | Kondisi | Aksi |
| ----- | ------- | ---- |
| **1** | uncle **RED** | parent & uncle → BLACK, kakek → RED, lalu naikkan `z = kakek` (ulangi) |
| **2** | uncle BLACK, `z` di sisi **dalam** (zig-zag) | rotasi pada parent untuk **meluruskan** → jadi kasus 3 |
| **3** | uncle BLACK, `z` di sisi **luar** (lurus) | parent → BLACK, kakek → RED, rotasi pada kakek |

Terakhir: `(*root)->color = BLACK` (jaga aturan 2). Kode menangani dua arah cermin
(parent di kiri kakek vs di kanan kakek).

---

## 6. DELETE & `deleteFixup`

### Fungsi pendukung
- **`findMin`**: node terkiri (successor saat node punya 2 anak).
- **`transplant(u, v)`**: ganti subtree `u` dengan `v` — cukup pindahkan pointer
  parent (`v` menggantikan posisi `u` di parent `u`).

### `deleteNode` (gaya CLRS)
1. Cari node target `z`. Kalau tak ada → "tidak ditemukan".
2. `y` = node yang **benar-benar lepas** dari posisinya; `yOriginal` = warna asli `y`.
3. Tiga kasus:
   - **Tanpa anak kiri** → `x = z->right`, `transplant(z, z->right)`.
   - **Tanpa anak kanan** → `x = z->left`, `transplant(z, z->left)`.
   - **Dua anak** → `y = findMin(z->right)` (successor); `x = y->right`; pasang `y`
     menggantikan `z` dan `y` mewarisi **warna** `z`.
4. `free(z)`.
5. **Kunci:** kalau `yOriginal == BLACK`, satu node hitam hilang → aturan 5 bisa
   rusak → panggil `deleteFixup(root, x)`. (Kalau yang lepas RED, black-height tak berubah → aman.)

### `deleteFixup` — `x` membawa "ekstra hitam"
Konsep: node yang naik (`x`) dianggap membawa **double-black** yang harus
"diserap" sambil naik. Lihat warna **sibling** (saudara) `w`:

| Kasus | Kondisi sibling `w` | Aksi |
| ----- | ------------------- | ---- |
| **1** | `w` **RED** | tukar warna `w`↔parent, rotasi ke arah `x` → ubah jadi kasus sibling hitam |
| **2** | `w` BLACK, **kedua anak `w` BLACK** | `w` → RED, naikkan `x = parent` (teruskan ekstra hitam) |
| **3** | `w` BLACK, anak "jauh" BLACK, anak "dekat" RED | rotasi pada `w` → jadi kasus 4 |
| **4** | `w` BLACK, anak "jauh" RED | tukar warna, rotasi pada parent, **selesai** (`x = root`) |

Terakhir `x->color = BLACK` menyerap sisa ekstra hitam. Lagi-lagi ada dua arah cermin.

### `pop`
- Wrapper sederhana memanggil `deleteNode`.

### `inorder`
- Cetak `data` + warna: `(M)` merah / `(H)` hitam. Berhenti di `NIL`/`NULL`.
- Hasil tetap **terurut menaik** (sifat BST).

#### 🎞️ Diagram Delete (alur keputusan)

```
deleteNode(value)
   │
   ├─ cari node z  ── tak ada ─▶ "tidak ditemukan"
   │
   ├─ z punya < 2 anak ─▶ transplant langsung (x = anak yang ada / NIL)
   │
   └─ z punya 2 anak ─▶ y = successor (findMin kanan)
                         y gantikan z, y WARISI warna z
                         x = y->right
   │
   ▼
  yOriginal == BLACK ?
     ├─ TIDAK (merah lepas) ─▶ selesai (black-height tak berubah)
     └─ YA   (hitam lepas)  ─▶ deleteFixup(x)   ← aturan 5 mungkin rusak
```

**Contoh hapus daun `25` (RED):** node merah lepas → black-height tiap jalur **tidak**
berubah → tak perlu fixup. Cukup lepas & `free`.

**`deleteFixup` — menyelesaikan "double-black" lewat warna sibling `w`:**
```
   parent(?)               x bawa EKSTRA hitam, lihat saudara w
   /      \
  x        w   ── w RED ─────────────▶ Kasus 1: rotasi → w jadi hitam
 (xtra   /  \
 black) wL  wR  ── wL,wR hitam ──────▶ Kasus 2: w jadi RED, x naik ke parent
                ── wR hitam, wL merah ▶ Kasus 3: rotasi w → jadi Kasus 4
                ── wR merah ──────────▶ Kasus 4: tukar warna + rotasi, SELESAI
```
Loop berhenti saat `x` jadi merah atau jadi akar; lalu `x->color = BLACK`
menyerap sisa ekstra hitam (memulihkan aturan 5).

---

## 7. Aturan & Hal Penting (ringkasan untuk debugging)

1. **5 aturan warna** di atas adalah invariant — semua operasi harus menjaganya.
2. **Node baru = RED** agar hanya aturan 4 yang mungkin rusak.
3. **Fixup hanya perlu kalau node yang lepas BLACK** (delete) atau parent RED (insert).
4. **Akar dipaksa BLACK** di akhir tiap fixup.
5. **Sentinel `NIL` selalu BLACK** dan jangan pernah di-`free`.
6. Setiap rotasi **wajib** memperbarui ketiga pointer `parent` yang terlibat.

> **Verifikasi cepat:** hitung black-height dari akar ke tiap daun — harus sama
> semua. Cek tidak ada merah beruntun. Inorder harus menaik.

---

## 8. Ringkasan Kompleksitas

| Operasi | Waktu | Ruang | Catatan |
| ------- | ----- | ----- | ------- |
| Cari    | **O(log n)** | O(h) | tinggi dijamin ≤ 2·log₂(n+1) |
| Insert  | **O(log n)** | O(1)* | maks **2 rotasi**, sisanya ganti warna |
| Delete  | **O(log n)** | O(1)* | maks **3 rotasi**, sisanya ganti warna |
| Inorder | O(n) | O(h) | hasil terurut menaik |

> *Rotasi pada RB-Tree **dibatasi konstan** (insert ≤ 2, delete ≤ 3) — sisanya hanya
> pewarnaan ulang yang murah. Inilah kenapa RB-Tree **lebih hemat untuk update**
> dibanding [AVL](../avl-tree/avl-tree.md) yang bisa berkali-kali rotasi saat delete.

---

## 9. Kapan Dipakai?

✅ **Cocok ketika:**
- **Insert & hapus sering** (campuran read/write) → rotasi yang dibatasi bikin update murah.
- Butuh struktur terurut general-purpose dengan jaminan O(log n).
- Inilah pilihan **library standar**: `std::map`/`std::set` (C++), `TreeMap` (Java),
  *scheduler* & *virtual memory* kernel Linux.

❌ **Kurang ideal ketika:**
- Beban **hampir murni baca** → [AVL](../avl-tree/avl-tree.md) yang lebih rapat sedikit lebih cepat dicari.
- Data di disk per-blok → [2-3 Tree](../b-tree-2-3/b-tree-2-3.md)/B-Tree.

---

## 10. Perbandingan Singkat

| Aspek            | [BST](../binary-search-tree/binary-search-tree.md) | [AVL](../avl-tree/avl-tree.md) | Red-Black |
| ---------------- | ------- | ------- | --------- |
| Jaminan seimbang | ❌      | ✅ (ketat) | ✅ (longgar) |
| Tinggi           | O(n) terburuk | ~1.44 log n | ~2 log n |
| Cari             | bisa lambat | tercepat | cepat |
| Biaya insert/delete | murah | banyak rotasi | sedikit rotasi (lebih murah) |
| Dipakai di       | dasar   | DB index | `std::map`, kernel Linux, dll |

Lihat juga: [BST](../binary-search-tree/binary-search-tree.md), [AVL](../avl-tree/avl-tree.md), [2-3 Tree](../b-tree-2-3/b-tree-2-3.md).
