# 📚 DSA Notes

> Catatan pribadi **Data Structures & Algorithms (DSA)** dalam bahasa **C** — lengkap
> dengan diagram, penjelasan kode per fungsi, aturan/invariant, analisis kompleksitas,
> dan kapan sebaiknya dipakai. Ditulis dalam Bahasa Indonesia supaya enak dibaca ulang
> di mana saja.

Setiap topik punya dua berkas berdampingan:

- **`*.c`** — implementasi yang bisa langsung dikompilasi & dijalankan.
- **`*.md`** — catatan teori + pembedahan kodenya.

---

## 🗺️ Daftar Isi

### Algorithms

#### Sorting
| Topik | Catatan | Kode | Waktu (rata-rata) | Stabil | In-place |
| ----- | ------- | ---- | ----------------- | ------ | -------- |
| **Bubble Sort** | [📄 catatan](algorithms/sorting/bubble-sort/bubble-sort.md) | [💻 kode](algorithms/sorting/bubble-sort/bubble-sort.c) | O(n²) | ✅ | ✅ |
| **Merge Sort** | [📄 catatan](algorithms/sorting/merge-sort/merge-sort.md) | [💻 kode](algorithms/sorting/merge-sort/merge-sort.c) | O(n log n) | ✅ | ❌ |

### Data Structures

#### Linear
| Topik | Catatan | Kode | Akses | Insert/Hapus ujung |
| ----- | ------- | ---- | ----- | ------------------ |
| **Linked List** (singly) | [📄 catatan](data-structures/linear/linked-list/linked-list.md) | [💻 kode](data-structures/linear/linked-list/linked-list.c) | O(n) | O(1) |

#### Non-Linear — Trees
| Topik | Catatan | Kode | Cari/Insert/Hapus | Cara seimbang |
| ----- | ------- | ---- | ----------------- | ------------- |
| **Binary Search Tree** | [📄 catatan](data-structures/non-linear/trees/binary-search-tree/binary-search-tree.md) | [💻 kode](data-structures/non-linear/trees/binary-search-tree/binary-search-tree.c) | O(log n) ~ O(n) | — (tak dijamin) |
| **AVL Tree** | [📄 catatan](data-structures/non-linear/trees/avl-tree/avl-tree.md) | [💻 kode](data-structures/non-linear/trees/avl-tree/avl-tree.c) | O(log n) | rotasi (tinggi) |
| **Red-Black Tree** | [📄 catatan](data-structures/non-linear/trees/red-black-tree/red-black-tree.md) | [💻 kode](data-structures/non-linear/trees/red-black-tree/red-black-tree.c) | O(log n) | rotasi + warna |
| **2-3 Tree** (B-Tree orde 3) | [📄 catatan](data-structures/non-linear/trees/b-tree-2-3/b-tree-2-3.md) | [💻 kode](data-structures/non-linear/trees/b-tree-2-3/b-tree-2-3.c) | O(log n) | split / merge |

---

## 📂 Struktur Folder

```
DSA-Notes/
├── algorithms/
│   └── sorting/
│       ├── bubble-sort/      bubble-sort.{c,md}
│       └── merge-sort/       merge-sort.{c,md}
└── data-structures/
    ├── linear/
    │   └── linked-list/      linked-list.{c,md}
    └── non-linear/
        └── trees/
            ├── binary-search-tree/  binary-search-tree.{c,md}
            ├── avl-tree/            avl-tree.{c,md}
            ├── red-black-tree/      red-black-tree.{c,md}
            └── b-tree-2-3/          b-tree-2-3.{c,md}
```

---

## 🛠️ Cara Menjalankan Kode

Butuh compiler C (mis. **GCC**). Dari folder topik mana pun:

```bash
# Contoh: Bubble Sort
gcc bubble-sort.c -o bubble-sort
./bubble-sort
```

> 💡 Beberapa berkas memakai fitur C99 (mis. VLA pada Merge Sort). Tambahkan
> `-std=c11` bila perlu: `gcc -std=c11 merge-sort.c -o merge-sort`.

Di **Windows (Command Prompt)** hasilnya `.exe`:

```powershell
gcc bubble-sort.c -o bubble-sort.exe
bubble-sort.exe
```

---

## 🧭 Saran Urutan Belajar

1. **[Bubble Sort](algorithms/sorting/bubble-sort/bubble-sort.md)** — konsep dasar
   perbandingan & swap.
2. **[Merge Sort](algorithms/sorting/merge-sort/merge-sort.md)** — kenalan dengan
   *Divide & Conquer* dan O(n log n).
3. **[Linked List](data-structures/linear/linked-list/linked-list.md)** — pointer,
   `malloc`/`free`, dan fondasi struktur dinamis.
4. **[Binary Search Tree](data-structures/non-linear/trees/binary-search-tree/binary-search-tree.md)**
   — pohon terurut; pahami ini dulu sebelum yang seimbang.
5. **[AVL Tree](data-structures/non-linear/trees/avl-tree/avl-tree.md)** &
   **[Red-Black Tree](data-structures/non-linear/trees/red-black-tree/red-black-tree.md)**
   — BST + penyeimbang lewat rotasi.
6. **[2-3 Tree](data-structures/non-linear/trees/b-tree-2-3/b-tree-2-3.md)** — dasar
   B-Tree, penyeimbang lewat split/merge (fondasi indeks database).

---

## 📌 Catatan

- Tautan antar-catatan memakai **link relatif** sehingga bisa langsung diklik saat
  dibaca di GitHub.
- Catatan ini untuk tujuan **belajar**; implementasi mengutamakan kejelasan, bukan
  optimasi produksi.

🤖 Indeks ini dibuat & dirapikan dengan bantuan [Claude Code](https://claude.com/claude-code).
