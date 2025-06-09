#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// ====================== DEKLARASI KONSTANTA DAN STRUCT ======================

const int MAX_ANGGOTA = 100;
const int MAX_LOKASI = 10;
const int MAX_STACK = 200; // Maksimum riwayat peminjaman
const int MAX_QUEUE = 10;  // Maksimum antrean per buku

// Forward declaration untuk Queue
struct Queue;

// Struct untuk Buku
struct Buku {
    int id;
    string judul;
    string pengarang;
    bool tersedia;
    string lokasi;
    Queue* waitingList; // POINTER ke Queue untuk daftar tunggu
};

// Struct untuk Anggota
struct Anggota {
    int id;
    string nama;
};

// ====== IMPLEMENTASI TREE (BINARY SEARCH TREE) untuk Buku ======
struct NodeBuku {
    Buku data;
    NodeBuku *left;
    NodeBuku *right;
};

// ====== IMPLEMENTASI QUEUE untuk Daftar Tunggu ======
struct NodeAntrian {
    int idAnggota;
    NodeAntrian* next;
};

struct Queue {
    NodeAntrian *front, *rear;
};

// ====== IMPLEMENTASI STACK untuk Riwayat Peminjaman ====== // DIUBAH
struct Riwayat {
    string info;
};

struct Stack {
    int top;
    Riwayat data[MAX_STACK];
};

NodeBuku* rootBuku = nullptr;
Anggota daftarAnggota[MAX_ANGGOTA];
Stack riwayatPeminjaman;

int jumlahBuku = 0;
int jumlahAnggota = 0;
int idBukuOtomatis = 1;

int adjacencyMatrix[MAX_LOKASI][MAX_LOKASI];
string namaLokasi[MAX_LOKASI];
int jumlahLokasi = 0;


void cls() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void tampilkanHeader() {
    cout << "==================================================\n";
    cout << "                SISTEM PERPUSTAKAAN               \n";
    cout << "==================================================\n";
}


void initStack(Stack &s) {
    s.top = -1;
}

bool isStackEmpty(Stack &s) {
    return s.top == -1;
}

void push(Stack &s, Riwayat r) {
    if (s.top < MAX_STACK - 1) {
        s.data[++s.top] = r;
    }
}

void initQueue(Queue* &q) {
    q = new Queue();
    q->front = q->rear = nullptr;
}

bool isQueueEmpty(Queue* q) {
    return q->front == nullptr;
}

void enqueue(Queue* q, int idAnggota) {
    NodeAntrian* newNode = new NodeAntrian();
    newNode->idAnggota = idAnggota;
    newNode->next = nullptr;
    if (isQueueEmpty(q)) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

int dequeue(Queue* q) {
    if (isQueueEmpty(q)) return -1;
    NodeAntrian* temp = q->front;
    int idAnggota = temp->idAnggota;
    q->front = q->front->next;
    if (q->front == nullptr) {
        q->rear = nullptr;
    }
    delete temp;
    return idAnggota;
}

// ====================== FUNGSI-FUNGSI SISTEM PERPUSTAKAAN ======================

int cariIndexLokasi(string nama) {
    for (int i = 0; i < jumlahLokasi; i++) {
        if (namaLokasi[i] == nama) return i;
    }
    return -1;
}

void tambahLokasi(string lokasi) {
    int i = cariIndexLokasi(lokasi);
    if (i == -1 && jumlahLokasi < MAX_LOKASI) {
        i = jumlahLokasi;
        namaLokasi[jumlahLokasi++] = lokasi;
        adjacencyMatrix[i][i] = 1;
    }
}

NodeBuku* insertBuku(NodeBuku* node, Buku dataBuku) {
    if (node == nullptr) {
        node = new NodeBuku();
        node->data = dataBuku;
        node->left = node->right = nullptr;
        jumlahBuku++;
    } else if (dataBuku.id < node->data.id) {
        node->left = insertBuku(node->left, dataBuku);
    } else {
        node->right = insertBuku(node->right, dataBuku);
    }
    return node;
}

NodeBuku* searchBuku(NodeBuku* node, int idBuku) {
    if (node == nullptr || node->data.id == idBuku) {
        return node;
    }
    if (idBuku < node->data.id) {
        return searchBuku(node->left, idBuku);
    }
    return searchBuku(node->right, idBuku);
}

void tambahBuku() {
    Buku bukuBaru;
    bukuBaru.id = idBukuOtomatis++;
    cout << "Masukkan judul buku: ";
    cin.ignore(); getline(cin, bukuBaru.judul);
    cout << "Masukkan pengarang buku: ";
    getline(cin, bukuBaru.pengarang);
    cout << "Masukkan lokasi buku (misal: Rak A1): ";
    getline(cin, bukuBaru.lokasi);

    tambahLokasi(bukuBaru.lokasi);
    bukuBaru.tersedia = true;
    initQueue(bukuBaru.waitingList);

    rootBuku = insertBuku(rootBuku, bukuBaru);
    cout << "Buku berhasil ditambahkan dengan ID: " << bukuBaru.id << "!\n";
}

void inOrderTraversal(NodeBuku* node) {
    if (node == nullptr) return;
    inOrderTraversal(node->left);
    cout << "[" << node->data.id << "] "
         << node->data.judul << " oleh " << node->data.pengarang
         << " | Status: " << (node->data.tersedia ? "Tersedia" : "Dipinjam")
         << " | Lokasi: " << node->data.lokasi << "\n";
    inOrderTraversal(node->right);
}

void tampilkanBuku() {
    cout << "\nDaftar Buku (diurutkan berdasarkan ID):\n";
    if (rootBuku == nullptr) {
        cout << "Belum ada buku di perpustakaan.\n";
    } else {
        inOrderTraversal(rootBuku);
    }
}

void searchByTitleTraversal(NodeBuku* node, string keyword, bool &ditemukan) {
    if (node == nullptr) return;
    searchByTitleTraversal(node->left, keyword, ditemukan);
    if (node->data.judul.find(keyword) != string::npos) {
        cout << "[" << node->data.id << "] "
             << node->data.judul << " | Status: " << (node->data.tersedia ? "Tersedia" : "Dipinjam") << "\n";
        ditemukan = true;
    }
    searchByTitleTraversal(node->right, keyword, ditemukan);
}

void cariBukuByTitle() {
    string keyword;
    cout << "Masukkan kata kunci judul: ";
    cin.ignore(); getline(cin, keyword);
    bool ditemukan = false;
    searchByTitleTraversal(rootBuku, keyword, ditemukan);
    if (!ditemukan) {
        cout << "Buku dengan judul tersebut tidak ditemukan.\n";
    }
}

void tambahAnggota() {
    if (jumlahAnggota >= MAX_ANGGOTA) {
        cout << "Kapasitas anggota penuh.\n";
        return;
    }
    Anggota &anggota = daftarAnggota[jumlahAnggota];
    anggota.id = jumlahAnggota + 1;
    cout << "Masukkan nama anggota: ";
    cin.ignore(); getline(cin, anggota.nama);
    jumlahAnggota++;
    cout << "Anggota '" << anggota.nama << "' berhasil ditambahkan dengan ID: " << anggota.id << "!\n";
}

void tampilkanAnggota() {
    cout << "\nDaftar Anggota:\n";
    for (int i = 0; i < jumlahAnggota; i++) {
        cout << "[" << daftarAnggota[i].id << "] " << daftarAnggota[i].nama << "\n";
    }
}

void pinjamBuku() {
    int idAnggota, idBuku;
    cout << "Masukkan ID anggota: ";
    cin >> idAnggota;
    if (idAnggota <= 0 || idAnggota > jumlahAnggota) {
        cout << "ID anggota tidak valid.\n"; return;
    }
    cout << "Masukkan ID buku yang ingin dipinjam: ";
    cin >> idBuku;
    NodeBuku* nodeBuku = searchBuku(rootBuku, idBuku);
    if (nodeBuku == nullptr) {
        cout << "ID buku tidak valid.\n"; return;
    }

    if (nodeBuku->data.tersedia) {
        nodeBuku->data.tersedia = false;
        cout << "Buku '" << nodeBuku->data.judul << "' berhasil dipinjam oleh " << daftarAnggota[idAnggota - 1].nama << ".\n";

        Riwayat r;
        r.info = "Buku '" + nodeBuku->data.judul + "' (ID:" + to_string(idBuku) + ") dipinjam oleh " + daftarAnggota[idAnggota - 1].nama;
        push(riwayatPeminjaman, r);

    } else {
        cout << "Buku sedang dipinjam.\n";
        char pilihan;
        cout << "Apakah Anda ingin masuk daftar tunggu (waiting list)? (y/n): ";
        cin >> pilihan;
        if (pilihan == 'y' || pilihan == 'Y') {
            enqueue(nodeBuku->data.waitingList, idAnggota);
            cout << "Anda berhasil ditambahkan ke daftar tunggu.\n";
        }
    }
}

void kembalikanBuku() {
    int idBuku;
    cout << "Masukkan ID buku yang ingin dikembalikan: ";
    cin >> idBuku;
    NodeBuku* nodeBuku = searchBuku(rootBuku, idBuku);
    if (nodeBuku == nullptr) {
        cout << "ID buku tidak valid.\n"; return;
    }

    if (!nodeBuku->data.tersedia) {
        nodeBuku->data.tersedia = true;
        cout << "Buku berhasil dikembalikan.\n";

        Riwayat r;
        r.info = "Buku '" + nodeBuku->data.judul + "' (ID:" + to_string(idBuku) + ") telah dikembalikan.";
        push(riwayatPeminjaman, r);

        if (!isQueueEmpty(nodeBuku->data.waitingList)) {
            int idAnggotaBerikutnya = dequeue(nodeBuku->data.waitingList);
            cout << "Notifikasi: Buku ini sekarang tersedia untuk anggota "
                 << daftarAnggota[idAnggotaBerikutnya-1].nama << " (ID: " << idAnggotaBerikutnya << ") dari daftar tunggu.\n";
        }

    } else {
        cout << "Buku ini tidak sedang dalam status dipinjam.\n";
    }
}

void tampilkanRiwayatPeminjaman() {
    cout << "\n===== Riwayat Peminjaman (dari terbaru) =====\n";
    if (isStackEmpty(riwayatPeminjaman)) {
        cout << "Belum ada riwayat peminjaman.\n";
    } else {
        for (int i = riwayatPeminjaman.top; i >= 0; --i) {
            cout << "- " << riwayatPeminjaman.data[i].info << "\n";
        }
    }
}

void tampilkanLokasiBuku() {
    cout << "\nPemetaan Lokasi Rak Buku (Graf Matriks):\n\n    ";
    const int lebarKolom = 10;

    for (int i = 0; i < jumlahLokasi; i++) {
        cout << setw(lebarKolom) << namaLokasi[i];
    }
    cout << "\n";

    for (int i = 0; i < jumlahLokasi; i++) {
        cout << setw(4) << namaLokasi[i];
        for (int j = 0; j < jumlahLokasi; j++) {
            cout << setw(lebarKolom) << adjacencyMatrix[i][j];
        }
        cout << "\n";
    }
}

// ====================== MAIN MENU ======================

int main() {
    initStack(riwayatPeminjaman);
    int pilihan;
    do {
        cls();
        tampilkanHeader();
        cout << "1. Tambah Buku \n";
        cout << "2. Tampilkan Daftar Buku\n";
        cout << "3. Pinjam Buku \n";
        cout << "4. Kembalikan Buku\n";
        cout << "5. Cari Buku Berdasarkan Judul\n";
        cout << "6. Tambah Anggota\n";
        cout << "7. Tampilkan Daftar Anggota\n";
        cout << "8. Tampilkan Pemetaan Lokasi Rak \n";
        cout << "9. Tampilkan Riwayat Peminjaman \n";
        cout << "0. Keluar\n";
        cout << "Pilih menu: ";
        cin >> pilihan;

        cls();
        switch (pilihan) {
            case 1: tambahBuku(); break;
            case 2: tampilkanBuku(); break;
            case 3: pinjamBuku(); break;
            case 4: kembalikanBuku(); break;
            case 5: cariBukuByTitle(); break;
            case 6: tambahAnggota(); break;
            case 7: tampilkanAnggota(); break;
            case 8: tampilkanLokasiBuku(); break;
            case 9: tampilkanRiwayatPeminjaman(); break;
            case 0: cout << "Terima kasih telah menggunakan sistem.\n"; break;
            default: cout << "Pilihan tidak valid.\n"; break;
        }

        if (pilihan != 0) {
            cout << "\nTekan Enter untuk kembali ke menu...";
            cin.ignore(); cin.get();
        }

    } while (pilihan != 0);

    return 0;
}


