# AVL Tree — [`avl-tree.c`](avl-tree.c)

> **AVL Tree** = [BST](../binary-search-tree/binary-search-tree.md) yang **selalu seimbang**. Tiap node menyimpan **tinggi**,
> dan selisih tinggi anak kiri & kanan (**Balance Factor**) wajib tetap di rentang
> `-1, 0, +1`. Begitu keluar dari rentang itu, pohon diperbaiki dengan **ROTASI**.
> Hasilnya: tinggi pohon dijamin **O(log n)**, jadi cari/insert/hapus selalu cepat.

Dinamakan dari penemunya: **A**delson-**V**elsky & **L**andis (1962).

---

## 1. Konsep Kunci: Height & Balance Factor

```c
int height(Node *root)        // tinggi node (NULL = 0, daun = 1)
int balanceFactor(Node *root) // = height(kiri) - height(kanan)
```

### 📏 Balance Factor (BF)

```
BF = tinggi(subtree kiri) − tinggi(subtree kanan)
```

| Nilai BF       | Arti                          | Status        |
| -------------- | ----------------------------- | ------------- |
| `-1`, `0`, `+1`| seimbang                      | ✅ VALID      |
| `+2` atau lebih| **berat di KIRI**             | ❌ VIOLATION → rotasi kanan |
| `-2` atau kurang| **berat di KANAN**           | ❌ VIOLATION → rotasi kiri  |

> **ATURAN INTI (yang ditanyakan):**
> Kalau **BF ≥ +2** (mis. `+2`) atau **BF ≤ −2** (mis. `−2`) → **VIOLATION**.
> Pohon **harus** di-rebalance lewat rotasi sampai BF tiap node kembali ke `{-1,0,+1}`.
> - `BF == +2` → terlalu berat kiri.
> - `BF == -2` → terlalu berat kanan.

---

## 2. Empat Kasus Ketidakseimbangan & Solusinya

Ada 4 pola, ditentukan oleh **BF node** dan **BF anaknya**:

| Kasus | Kondisi | Bentuk | Solusi |
| ----- | ------- | ------ | ------ |
| **Left-Left (LL)**   | `BF > 1` dan `BF(kiri) ≥ 0`  | `/` lurus kiri  | **1× rotasi kanan** pada node |
| **Left-Right (LR)**  | `BF > 1` dan `BF(kiri) < 0`  | zig-zag `<`     | rotasi **kiri** pada anak kiri → lalu rotasi **kanan** pada node |
| **Right-Right (RR)** | `BF < -1` dan `BF(kanan) ≤ 0`| `\` lurus kanan | **1× rotasi kiri** pada node |
| **Right-Left (RL)**  | `BF < -1` dan `BF(kanan) > 0`| zig-zag `>`     | rotasi **kanan** pada anak kanan → lalu rotasi **kiri** pada node |

> Cara ingat: kasus **lurus** (LL/RR) → 1 rotasi. Kasus **bengkok** (LR/RL) →
> luruskan dulu jadi LL/RR, baru 1 rotasi lagi (total 2 rotasi).

### Ilustrasi Rotasi Kanan (untuk LL)

```
       y                 x
      / \               / \
     x   T3   ───▶     T1   y
    / \                    / \
  T1   T2               T2   T3
```

### Ilustrasi Rotasi Kiri (untuk RR)

```
     x                   y
    / \                 / \
  T1   y      ───▶     x   T3
      / \             / \
    T2   T3         T1   T2
```

---

## 3. Penjelasan Code per Fungsi

### `createNode`
- Daun baru selalu `height = 1` (bukan 0), `left = right = NULL`.

### `height`, `max`, `updateHeight`
- `height(NULL)` aman → `0` (tanpa ini akan crash saat akses node kosong).
- `updateHeight`: `height = 1 + max(tinggi kiri, tinggi kanan)`. **Wajib** dipanggil
  setiap kali struktur berubah.

### `rotateRight(y)` — untuk kasus berat kiri
1. `x = y->left` (anak kiri jadi akar baru).
2. `T2 = x->right` dipindah jadi `y->left`.
3. `x->right = y` (y turun ke kanan).
4. `updateHeight(y)` **dulu** (node di bawah), baru `updateHeight(x)`.
5. Kembalikan `x` sebagai akar subtree baru.

### `rotateLeft(x)` — cermin dari rotateRight, untuk kasus berat kanan.

### `rebalance` — otak penyeimbang
```c
updateHeight(root);
int bf = balanceFactor(root);
if (bf > 1)  { jika BF(kiri)  < 0 → rotateLeft(kiri) dulu (LR); return rotateRight(root); }
if (bf < -1) { jika BF(kanan) > 0 → rotateRight(kanan) dulu (RL); return rotateLeft(root); }
return root; // sudah seimbang
```
- Cek `bf > 1` (berat kiri) dan `bf < -1` (berat kanan).
- Untuk kasus zig-zag, lakukan rotasi pada anak dulu (mengubah LR→LL, RL→RR).

### `insert`
- Sama seperti [BST](../binary-search-tree/binary-search-tree.md) (`< kiri`, `> kanan`, `==` diabaikan).
- **Bedanya:** setiap balik dari rekursi memanggil `return rebalance(root)`.
- Karena rebalance dipanggil di **jalur balik**, setiap leluhur node baru ikut dicek dan diperbaiki.

### `findMin`, `deleteNode`
- Logika hapus **identik dengan BST** (4 kasus: daun / 1 anak / 2 anak + successor).
- **Bedanya:** `deleteNode` mengembalikan `rebalance(root)`, bukan `root` — karena
  menghapus bisa membuat node leluhur tak seimbang.

### `pop`, `inorder`
- `pop`: wrapper dengan flag `found` + pesan "tidak ditemukan".
- `inorder`: hasil terurut menaik (cara cek validitas).

#### 🎞️ Diagram Delete + Rebalance

Menghapus node bisa membuat sisi lain jadi **terlalu berat** → rebalance di jalur balik.

**Contoh: hapus `10` membuat akar `30` berat kanan (BF = −2 → RR).**
```
       30  (BF=-2)              40            ← rotasi KIRI pada 30
      /  \                     /  \
    10    40       ───▶      30    50
            \               /
             50            (seimbang lagi, BF tiap node ∈ {-1,0,1})
   ↑ hapus 10
```

**Langkah internal `deleteNode`:**
```
1. Hapus node seperti BST biasa (daun / 1 anak / 2 anak + successor).
2. Saat rekursi BALIK ke tiap leluhur → updateHeight + cek balanceFactor.
3. Begitu ketemu BF = +2 atau -2 → rotasi yang sesuai (LL/LR/RR/RL).
4. Lanjut naik; bisa terjadi LEBIH DARI SATU rotasi sepanjang jalur ke akar.
```

> **Beda penting dengan insert:** insert paling banyak butuh **1 perbaikan rotasi**.
> Delete bisa memicu rotasi **berkali-kali** sampai ke akar (tiap leluhur dicek ulang).

---

## 4. Aturan AVL (Invariant)

> **INVARIANT:** untuk **setiap** node, `balanceFactor ∈ {-1, 0, +1}`
> **DAN** aturan BST tetap berlaku (`kiri < node < kanan`).

Yang harus dijaga tiap operasi:

1. **`updateHeight` sebelum `balanceFactor`** — BF dihitung dari tinggi yang sudah benar.
2. **Rebalance di jalur balik rekursi** — setelah insert/delete, cek semua leluhur dari bawah ke atas.
3. **BF == ±2 = sinyal perbaikan** — segera rotasi; jangan biarkan menumpuk.
4. **Urutan update tinggi saat rotasi**: node yang turun (di bawah) di-update **dulu**.
5. **Rotasi tidak merusak BST** — `inorder` sebelum & sesudah rotasi harus sama.

---

## 5. Kenapa AVL > BST biasa?

```c
int values[] = {10, 20, 30, 40, 50, 25}; // sengaja hampir menaik
```
Di [BST](../binary-search-tree/binary-search-tree.md) biasa, data menaik bikin pohon **miring** (O(n)). Di AVL, rotasi otomatis
menjaga tinggi tetap ~log n. Inorder tetap `10 20 25 30 40 50` tapi bentuknya seimbang.

> **AVL vs [Red-Black Tree](../red-black-tree/red-black-tree.md):** AVL lebih **ketat** seimbang (pencarian sedikit lebih
> cepat), tapi butuh lebih banyak rotasi saat insert/delete. Red-Black lebih longgar
> namun lebih murah untuk update — itulah yang dipakai banyak library standar.

---

## 6. Ringkasan Kompleksitas

| Operasi | Waktu | Ruang | Catatan |
| ------- | ----- | ----- | ------- |
| Cari    | **O(log n)** | O(h) | dijamin, karena tinggi selalu ~log n |
| Insert  | **O(log n)** | O(h) | maks **1** perbaikan rotasi (1 atau 2 putaran) |
| Delete  | **O(log n)** | O(h) | bisa **beberapa** rotasi sepanjang jalur ke akar |
| Rotasi  | O(1) | O(1) | hanya menukar sejumlah pointer |
| Inorder | O(n) | O(h) | hasil terurut menaik |

> Tinggi AVL dijamin `≤ 1.44 · log₂(n)` → **tak ada kasus terburuk O(n)** seperti [BST](../binary-search-tree/binary-search-tree.md).

---

## 7. Kapan Dipakai?

✅ **Cocok ketika:**
- **Pencarian (read) jauh lebih sering** daripada insert/hapus → keseimbangan ketat AVL bikin cari paling cepat.
- Butuh **jaminan** O(log n) bahkan saat data masuk terurut.
- Contoh: indeks database in-memory, kamus yang sering di-query.

❌ **Kurang ideal ketika:**
- **Insert/hapus sangat sering** → rotasi AVL yang agresif jadi mahal; [Red-Black Tree](../red-black-tree/red-black-tree.md) lebih hemat.
- Data tersimpan di disk per-blok → [2-3 Tree](../b-tree-2-3/b-tree-2-3.md)/B-Tree lebih cocok.

| Aspek | [BST](../binary-search-tree/binary-search-tree.md) | **AVL** | [Red-Black Tree](../red-black-tree/red-black-tree.md) |
| ----- | ------- | ------- | ----------------- |
| Jaminan tinggi | ❌ O(n) | ✅ ~1.44 log n (paling rapat) | ✅ ~2 log n |
| Kecepatan cari | bervariasi | **tercepat** | cepat |
| Biaya insert/delete | termurah | termahal (rotasi banyak) | sedang |

Lihat juga: [BST](../binary-search-tree/binary-search-tree.md), [Red-Black Tree](../red-black-tree/red-black-tree.md), [2-3 Tree](../b-tree-2-3/b-tree-2-3.md).
