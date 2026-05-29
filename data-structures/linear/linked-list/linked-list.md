# Linked List (Singly Linked List) — [`linked-list.c`](linked-list.c)

> **Singly Linked List** = rangkaian node yang tiap node menunjuk **satu arah** ke
> node berikutnya. Diibaratkan gerbong kereta: tiap gerbong (`Node`) menyimpan
> data (`id`) dan tali sambung ke gerbong di belakangnya (`next`).

---

## 1. Konsep & Mekanisme

```
head                                   tail
 │                                      │
 ▼                                      ▼
┌─────┬───┐   ┌─────┬───┐   ┌─────┬──────┐
│ 10  │ ●─┼──▶│ 20  │ ●─┼──▶│ 30  │ NULL │
└─────┴───┘   └─────┴───┘   └─────┴──────┘
```

- **`head`** = penunjuk ke node **pertama**. Kalau `head == NULL`, list **kosong**.
- **`tail`** = penunjuk ke node **terakhir**. Mempercepat penambahan di belakang
  (tanpa `tail` kita harus traversal sampai ujung tiap kali).
- **`next`** node terakhir selalu `NULL` → penanda akhir list.

Karena hanya ada pointer `next` (satu arah), kita **tidak bisa mundur**. Untuk
menghapus sebuah node kita perlu menyimpan node *sebelumnya* (`prev`) secara manual.

### Kompleksitas waktu

| Operasi             | Waktu  | Catatan                                  |
| ------------------- | ------ | ---------------------------------------- |
| `pushHead`          | O(1)   | langsung di depan                        |
| `pushTail`          | O(1)   | karena ada pointer `tail`                |
| `pop` (cari + hapus)| O(n)   | harus telusuri sampai ketemu nilainya    |
| `viewNode`          | O(n)   | telusuri semua node                      |

---

## 2. Penjelasan Struktur Data

```c
typedef struct NodeTrain {
  int id;                 // data yang disimpan node
  struct NodeTrain *next; // pointer ke node berikutnya
} Node;
```

> **Kenapa `**head` (pointer to pointer)?** Fungsi yang bisa **mengubah** `head`/`tail`
> milik `main` harus menerima **alamat** dari pointer tersebut. Kalau hanya `*head`
> (salinan pointer), perubahan tidak akan terlihat di `main`.

---

## 3. Penjelasan Code per Fungsi

### `createNode` — pabrik node
- Alokasi memori `malloc` seukuran satu `Node`.
- **Cek gagal alloc**: jika `malloc` mengembalikan `NULL`, program berhenti (`exit(1)`).
- Isi `id = val`, set `next = NULL`, lalu kembalikan node siap pakai.

### `pushTail` — tambah di **belakang**
- Buat node baru.
- **Jika list kosong** (`!*head`): `head` dan `tail` keduanya menunjuk node baru.
- **Jika sudah ada isi**: `tail` lama disambung ke node baru, lalu `tail` digeser ke node baru.

### `pushHead` — tambah di **depan**
- Buat node baru.
- **Jika list kosong**: `head = tail =` node baru.
- **Jika sudah ada isi**: `next` node baru = `head` lama, lalu `head` digeser ke node baru.
- > Catatan: `pushHead` **tidak** mengubah `tail` saat list tidak kosong (memang benar — ekor tetap).

### `viewNode` — cetak isi list
- Jika kosong → cetak `"Kosong"`.
- Pakai `temp` mulai dari `head`, maju lewat `next` sampai `NULL`, cetak tiap `id`.

### `pop` — hapus node pertama bernilai `val`
Langkah:
1. Telusuri dengan dua pointer: `temp` (node saat ini) & `prev` (node sebelumnya).
2. Berhenti saat `temp->id == val`.
3. Kalau `temp == NULL` → nilai tak ada → cetak `"Not Found"`.
4. **Penyambungan ulang** tergantung posisi:
   - **Di head**: `head` maju ke `temp->next`. Jika jadi kosong, `tail = NULL`.
   - **Di tengah/akhir**: `prev->next = temp->next` (loncati node). Jika yang dihapus adalah `tail`, `tail` mundur ke `prev`.
5. `free(temp)` membebaskan memori.

#### 🎞️ Diagram `pop` (hapus node tengah, mis. hapus `20`)

```
       prev    temp
        │       │
        ▼       ▼
┌────┐   ┌────┐   ┌────┐
│ 10 │──▶│ 20 │──▶│ 30 │──▶ NULL
└────┘   └────┘   └────┘

Langkah 1: sambungkan prev langsung ke temp->next (loncati 20)
┌────┐        ┌────┐
│ 10 │───────▶│ 30 │──▶ NULL
└────┘   ┌────┐└────┘
         │ 20 │  ← terputus
         └────┘

Langkah 2: free(temp)  → node 20 dihapus dari memori
┌────┐   ┌────┐
│ 10 │──▶│ 30 │──▶ NULL
└────┘   └────┘
```

**Hapus di head** (`prev == NULL`): cukup geser `head = temp->next`, lalu `free`.
**Hapus di tail**: setelah `prev->next = NULL`, mundurkan `tail = prev`.

### `freeAll` — bersihkan semua node
- Telusuri sambil simpan `next` **sebelum** `free` (kalau di-free dulu, `temp->next` jadi dangling).
- Akhirnya `head = tail = NULL`. **Wajib** dipanggil agar tidak ada *memory leak*.

---

## 4. Aturan & Hal Penting (sering jadi bug)

| Aturan | Penjelasan |
| ------ | ---------- |
| **Selalu cek `malloc`** | `malloc` bisa gagal → harus ditangani agar tidak crash. |
| **Simpan `next` sebelum `free`** | Mengakses memori yang sudah dibebaskan = *use-after-free* (undefined behavior). |
| **Jaga konsistensi `head` & `tail`** | Saat list menjadi kosong, **keduanya** harus `NULL`. Saat menghapus tail, `tail` harus mundur. |
| **Set `next = NULL` pada node baru** | Kalau lupa, node terakhir menunjuk sampah → traversal tak pernah berhenti. |
| **`**head` untuk fungsi pengubah** | Fungsi yang menambah/menghapus harus terima `Node **` agar perubahan terlihat di `main`. |
| **Selalu `freeAll` di akhir** | Mencegah memory leak. |

> ⚠️ **Catatan tentang `main`**: di file ini loop memanggil `pushHead`, `pushTail`,
> **dan** `pop` untuk setiap input dalam satu iterasi — ini hanya **contoh demo**
> agar semua fungsi terpakai. Pada program nyata, panggil sesuai kebutuhan saja.

---

## 5. Ringkasan Alur

```
INPUT N angka
   └─ tiap angka: pushHead → pushTail → pop
viewNode  (cetak sisa list)
freeAll   (bebaskan memori)
```

---

## 6. Ringkasan Kompleksitas

| Operasi              | Waktu | Ruang | Catatan |
| -------------------- | ----- | ----- | ------- |
| Akses elemen ke-`i`  | O(n)  | O(1)  | harus telusuri dari `head` (tak ada indeks acak) |
| Cari nilai           | O(n)  | O(1)  | telusuri sampai ketemu |
| `pushHead`           | O(1)  | O(1)  | langsung di depan |
| `pushTail`           | O(1)  | O(1)  | cepat karena ada pointer `tail` |
| `pop` (hapus nilai)  | O(n)  | O(1)  | O(n) untuk **mencari**, hapusnya sendiri O(1) |
| Ruang total          | —     | O(n)  | tiap node butuh ekstra 1 pointer `next` |

> **Linked List vs Array**
> - **Linked List unggul**: insert/hapus di depan/tengah O(1) (kalau node sudah dipegang), ukuran tumbuh dinamis tanpa realokasi.
> - **Array unggul**: akses indeks acak O(1), hemat memori (tanpa pointer), *cache-friendly* (data berdekatan).

---

## 7. Kapan Dipakai?

✅ **Cocok ketika:**
- Sering **menambah/menghapus di ujung** atau saat node sudah dipegang langsung.
- Jumlah data **tidak diketahui** di awal dan berubah-ubah (tumbuh dinamis).
- Jadi fondasi struktur lain: **Stack**, **Queue**, **hash table chaining**, *adjacency list* graf.

❌ **Hindari ketika:**
- Butuh **akses acak cepat** (elemen ke-`i`) → pakai array/`dynamic array`.
- Sangat sensitif memori/kecepatan cache → overhead pointer & lompatan memori mahal.

---

## 8. Variasi Linked List

| Jenis | Ciri | Kelebihan |
| ----- | ---- | --------- |
| **Singly** (file ini) | tiap node punya `next` saja | paling hemat memori |
| **Doubly** | punya `next` **dan** `prev` | bisa mundur; hapus node O(1) tanpa cari `prev` |
| **Circular** | node terakhir `next` → kembali ke `head` | cocok untuk antrian melingkar / round-robin |

Lihat juga: [BST](../../non-linear/trees/binary-search-tree/binary-search-tree.md) (struktur node bercabang dua arah).
