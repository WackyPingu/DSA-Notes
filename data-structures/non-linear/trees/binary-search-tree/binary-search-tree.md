# Binary Search Tree (BST) — [`binary-search-tree.c`](binary-search-tree.c)

> **BST** = pohon biner di mana **setiap node** menaati aturan:
> **semua nilai di subtree KIRI < node < semua nilai di subtree KANAN.**
> Aturan ini membuat pencarian, penambahan, dan penghapusan bisa "membuang
> separuh pohon" tiap langkah, mirip *binary search* pada array terurut.

---

## 1. Konsep & Mekanisme

```
            50
          /    \
        30      70
       /  \    /  \
     20   40  60   80
```

- Mau cari/insert nilai `X`: bandingkan dengan node.
  - `X < node`  → belok **kiri**.
  - `X > node`  → belok **kanan**.
  - `X == node` → ketemu (duplikat tidak disimpan di file ini).
- **Inorder traversal** (kiri → node → kanan) selalu menghasilkan urutan **menaik**.

### ⚠️ Kelemahan BST biasa
BST **tidak menjamin seimbang**. Kalau data masuk berurutan (`10,20,30,...`),
pohon merosot jadi seperti linked list:

```
10
  \
   20
     \
      30   ← tinggi = n, pencarian jadi O(n)
```

| Kondisi   | Tinggi  | Cari/Insert/Hapus |
| --------- | ------- | ----------------- |
| Seimbang  | O(log n)| O(log n)          |
| Terburuk (miring) | O(n) | O(n)        |

> Inilah alasan adanya [AVL](../avl-tree/avl-tree.md) dan [Red-Black Tree](../red-black-tree/red-black-tree.md) — mereka **memaksa** pohon tetap seimbang.

---

## 2. Struktur Data

```c
typedef struct NodeTree {
  int data;
  struct NodeTree *left;   // subtree nilai lebih kecil
  struct NodeTree *right;  // subtree nilai lebih besar
} Node;
```

---

## 3. Penjelasan Code per Fungsi

### `createNode`
- Alokasi node, cek `malloc` (kalau gagal `perror` + `exit`).
- Isi `data`, `left = right = NULL`.

### `insert` (rekursif)
- **Basis**: kalau `root == NULL`, di sinilah tempat node baru → `return createNode(value)`.
- `value < data` → masuk ke kiri: `root->left = insert(root->left, value)`.
- `value > data` → masuk ke kanan.
- `value == data` → tidak melakukan apa-apa (duplikat diabaikan).
- **Pola "kembalikan subtree"**: tiap pemanggil menyambungkan kembali hasil rekursi
  ke `left`/`right`-nya. Inilah cara node baru tersambung tanpa pointer `parent`.

### `findMin`
- Nilai terkecil = node paling kiri. Terus belok kiri sampai `left == NULL`.
- Dipakai saat menghapus node berdua anak (cari **successor**).

### `deleteNode` (rekursif, inti dari BST)
Cari dulu nodenya (sama seperti insert). Saat **ketemu** (`value == data`), set `*found = 1` lalu tangani **4 kasus**:

| Kasus | Kondisi | Aksi |
| ----- | ------- | ---- |
| **1. Daun** | tak punya anak | `free(root)`, kembalikan `NULL` |
| **2. Hanya anak kanan** | `left == NULL` | simpan `right`, `free(root)`, kembalikan anak kanan |
| **3. Hanya anak kiri** | `right == NULL` | simpan `left`, `free(root)`, kembalikan anak kiri |
| **4. Dua anak** | punya kiri & kanan | cari **successor** = `findMin(root->right)`; salin `data`-nya ke node ini; lalu hapus successor itu dari subtree kanan |

> **Kenapa successor (inorder successor)?** Nilai terkecil di subtree kanan adalah
> satu-satunya nilai yang bisa menggantikan node terhapus **tanpa melanggar** aturan
> BST: ia lebih besar dari semua di kiri dan lebih kecil dari sisa di kanan.
> (Alternatif yang juga sah: **predecessor** = nilai terbesar di subtree kiri.)

#### 🎞️ Diagram Delete per Kasus

**Kasus 1 — Daun** (hapus `20`): cukup lepas & `free`.
```
      30                 30
     /  \      ───▶         \
   20    40                  40
   ↑ hapus
```

**Kasus 2/3 — Satu anak** (hapus `30`, hanya punya anak kanan `40`):
anak menggantikan posisi node.
```
      50                 50
     /        ───▶      /
   30                  40
     \
      40
```

**Kasus 4 — Dua anak** (hapus akar `50`):
cari **successor** = `findMin(subtree kanan)` = `60`, salin nilainya ke node,
lalu hapus `60` dari subtree kanan (yang ternyata cuma daun → Kasus 1).
```
        50                    60              ← nilai successor naik
      /    \                /    \
    30      70    ───▶    30      70
   /  \    /  \          /  \       \
  20  40  60   80       20  40       80
          ↑ successor              (60 lama dihapus)
```

### `pop` — wrapper `deleteNode`
- Pakai flag `found`. Kalau setelah delete `found == 0`, cetak `"Nilai X tidak ditemukan!"`.
- Memisahkan logika rekursi (`deleteNode`) dari pesan ke user (good practice).

### `inorder`
- `inorder(left)` → cetak `data` → `inorder(right)`.
- Output **terurut menaik** — cara cepat memverifikasi BST tetap valid.

---

## 4. Aturan BST (Invariant)

> **INVARIANT UTAMA:** untuk setiap node N →
> `max(subtree kiri) < N->data < min(subtree kanan)`.

Aturan turunan yang harus dijaga tiap operasi:

1. **Insert** selalu berakhir sebagai **daun baru** di posisi yang benar.
2. **Duplikat** tidak disimpan (kebijakan file ini; bisa juga dibolehkan dengan aturan tambahan).
3. **Delete dua-anak** wajib pakai successor/predecessor — **jangan** asal cabut, agar invariant tetap terjaga.
4. **Inorder** wajib menghasilkan urutan menaik; kalau tidak, ada aturan yang dilanggar.

---

## 5. Contoh Alur di `main`

```
Insert: 50 30 70 20 40 60 80
Pohon awal (inorder): 20 30 40 50 60 70 80

Hapus 20  → Kasus 1 (daun)
Hapus 30  → Kasus 2/3 (satu anak: 40 naik)
Hapus 50  → Kasus 4 (dua anak: successor 60 menggantikan akar)
Hapus 99  → tidak ditemukan
```

---

## 6. Ringkasan Kompleksitas

| Operasi  | Rata-rata (seimbang) | Terburuk (miring) | Ruang |
| -------- | -------------------- | ----------------- | ----- |
| Cari     | O(log n)             | O(n)              | O(1) iteratif / O(h) rekursif |
| Insert   | O(log n)             | O(n)              | O(h) |
| Delete   | O(log n)             | O(n)              | O(h) |
| Inorder  | O(n)                 | O(n)              | O(h) |

> `h` = tinggi pohon. Pada pohon seimbang `h ≈ log n`; pada pohon miring `h ≈ n`.
> **Kelemahan utama BST polos: tidak ada jaminan tetap seimbang.**

---

## 7. Kapan Dipakai?

✅ **Cocok ketika:**
- Butuh struktur **terurut** yang mendukung cari + insert + hapus sekaligus.
- Sering perlu operasi berbasis urutan: nilai min/max, successor/predecessor, *range query*.
- Data masuk **acak** (cenderung membuat pohon ~seimbang secara alami).

❌ **Hindari / pakai varian lain ketika:**
- Data bisa masuk **terurut** → pohon merosot jadi O(n). Pakai [AVL](../avl-tree/avl-tree.md) atau [Red-Black Tree](../red-black-tree/red-black-tree.md).
- Butuh **jaminan** O(log n) di kondisi terburuk → pakai pohon seimbang.
- Data di disk/blok besar → pakai [2-3 Tree](../b-tree-2-3/b-tree-2-3.md) / B-Tree.

> **BST adalah fondasi.** Pahami ini dulu, karena [AVL](../avl-tree/avl-tree.md), [Red-Black Tree](../red-black-tree/red-black-tree.md), dan
> [2-3 Tree](../b-tree-2-3/b-tree-2-3.md) semuanya = BST + mekanisme penyeimbang.

Lihat juga: [AVL](../avl-tree/avl-tree.md), [Red-Black Tree](../red-black-tree/red-black-tree.md), [2-3 Tree](../b-tree-2-3/b-tree-2-3.md) — penyempurnaan BST agar selalu seimbang.
