/*******************
 * Nama File : A03_241402074_01.cpp
 * Pembuat : Andrian James Siregar
 *           Darren Wangsa
 *           Sanny Lie
 * Deskripsi : Program ini adalah program queue yang dapat digunakan pada sistem antri dari restoran.
 ******************/

#include <iostream>
#include <queue>
#include <ctime>
#include <iomanip>
#include <fstream> // Buat file

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define BLUE "\033[94m"

const int MAX = 100;

using namespace std;

struct Orang {
    int id;
    string nama, notelp, strMasuk;
    time_t waktuMasuk; // Ganti chrono jadi time_t
};

class Queue {
private:
    Orang data[MAX];
    int front, rear;
    int nextId;
    double totalServiceTime;
    int totalServed, lastId;

public:
    Queue() {
        nextId = 1;
        totalServiceTime = 0;
        totalServed = 0;
        front = -1;
        rear = -1;
    }

    bool isEmpty() {
        return front == -1 || front > rear;
    }

    bool isFull() {
        return rear == MAX - 1;
    }

    void enqueue() {
        updateQueue();
        if (isFull()) {
            cout << RED << "Queue Penuh" << RESET << endl;
            return;
        }
        string nama, notelp;

        cout << GREEN << "Masukkan nama: ";
        getline(cin, nama);
        cout << GREEN << "Masukkan nomor telepon: ";
        getline(cin, notelp);
        if (nama.empty() || notelp.empty()) {
            cout << RED << "Nama dan nomor telepon tidak boleh kosong!" << RESET << endl;
            return;
        }

        Orang o;
        o.nama = nama;
        o.id = lastId;
        o.waktuMasuk = time(0); // Ambil waktu saat ini
        o.notelp = notelp;

        if (isEmpty()) {
            front = 0;
        }
        rear++;
        data[rear] = o;

        cout << BOLD BLUE << nama << RESET GREEN << " telah terdaftar.\n";

        logToFile(o); // Simpan data ke file
    }

    void logToFile(Orang o) {
        ofstream file("antrian_log.txt", ios::app);
        if (file.is_open()) {
            file << o.id << "|" << o.nama << "|" << o.notelp << "|" << o.waktuMasuk << endl;
            file.close();
        } else {
            cout << RED << "Gagal membuka file log!" << RESET << endl;
        }
    }


    void updateQueue() {
        string x;
        ifstream readFile("antrian_log.txt");

        int i = 0;
        lastId = 0;

        while(getline(readFile, x) && i < MAX){
            stringstream ss(x);
            string idTemp, namaTemp, notelpTemp, timeTemp;

            getline(ss, idTemp, '|');
            getline(ss, namaTemp, '|');
            getline(ss, notelpTemp, '|');
            getline(ss, timeTemp); // waktu dalam bentuk time_t string

            data[i].id = stoi(idTemp);
            data[i].nama = namaTemp;
            data[i].notelp = notelpTemp;
            data[i].waktuMasuk = stol(timeTemp); // konversi string ke time_t

            if (data[i].id > lastId)
                lastId = data[i].id;

            i++;
        }
        readFile.close();

        if (i > 0) {
            front = 0;
            rear = i - 1;
        } else {
            front = rear = -1;
        }
    }


    void dequeue() {
        updateQueue();
        ofstream file("history.txt", ios::app); // Buat file untuk menyimpan data keluar
        if (isEmpty()) {
            cout << RED << "Antrian Kosong!" << RESET << endl;
            return;
        }
        Orang o;
        Orang keluar = data[front];

        // Input jam dan menit keluar
        tm waktuKeluar = *localtime(&keluar.waktuMasuk);
        cout << "Masukkan jam keluar (0-23): ";
        cin >> waktuKeluar.tm_hour;
        cout << "Masukkan menit keluar (0-59): ";
        cin >> waktuKeluar.tm_min;
        waktuKeluar.tm_sec = 0;

        // Konversi ke time_t
        time_t waktuKeluarTime = mktime(&waktuKeluar);
        double detik = difftime(waktuKeluarTime, keluar.waktuMasuk);
        int menit = detik / 60;
        int jam = menit / 60;
        int sisaDetik = int(detik) % 60;

        cout << BOLD GREEN << "ID: " << keluar.id << "\n";
        cout << "Nama: " << keluar.nama << "\n";
        cout << "Waktu Masuk: " << ctime(&keluar.waktuMasuk);
        cout << "Waktu Keluar: " << ctime(&waktuKeluarTime);
        cout << "Service Time: " << jam << " jam " << menit << " menit " << sisaDetik << " detik\n" << RESET;
        cout << BOLD CYAN << setfill('=') << setw(100) << "=" << RESET << endl << setfill(' ');

        // Simpan ke file history
        if (file.is_open()) {
            file << "ID: " << keluar.id << ", Nama: " << keluar.nama
                 << ", Waktu Masuk: " << ctime(&keluar.waktuMasuk)
                 << ", Waktu Keluar: " << ctime(&waktuKeluarTime)
                 << ", Service Time: " << jam << " jam " << menit << " menit " << sisaDetik << " detik\n";
            file.close();
        } else {
            cout << RED << "Gagal membuka file history!" << RESET << endl;
        }
        totalServiceTime += detik;
        totalServed++;
        front++;
        rewriteFile();

        if (front > rear) {
            front = rear = -1;
        }
    }

    void rewriteFile() {
        ofstream file("antrian_log.txt", ios::trunc);
        if (!file.is_open()) {
            cout << RED << "Gagal membuka file untuk menulis ulang!" << RESET << endl;
            return;
        }

        for (int i = front; i <= rear; i++) {
            file << data[i].id << "|"
                << data[i].nama << "|"
                << data[i].notelp << "|"
                << data[i].waktuMasuk << endl;
        }

        file.close();
    }


    void tampilkanRataRata() {
        if (totalServed == 0) {
            cout << YELLOW << "Belum ada yang dilayani.\n" << RESET;
            return;
        }
        double rata = totalServiceTime / totalServed;
        cout << YELLOW << "Rata-rata service time: " << rata << " detik\n" << RESET;
    }

    void display() {
        updateQueue();
        if (isEmpty()) {
            cout << "Antrian Kosong\n";
            return;
        }

        cout << YELLOW << "Isi antrian:\n";
        for (int i = front; i <= rear; ++i) {
            tm *ltm = localtime(&data[i].waktuMasuk);
            cout << BOLD GREEN << " - ID: " << data[i].id
                << ", Nama: " << data[i].nama
                << ", Masuk: " << setfill('0')
                << setw(2) << ltm->tm_hour << ":"
                << setw(2) << ltm->tm_min << ":"
                << setw(2) << ltm->tm_sec << RESET << endl;
        }
    }

};

int main() {
    Queue q;
    int pilihan;
    string nama, loop;

    do {
        q.updateQueue();
        system("cls");
        cout << BOLD CYAN << setfill('=') << setw(100) << "=" << RESET << endl
             << setfill(' ') << setw(30) << " " << BOLD BLUE << "Tugas 4 Pemrograman Berorientasi Objek\n"
             << BOLD CYAN << setfill('=') << setw(100) << "=" << RESET << endl << setfill(' ');

        cout << YELLOW << "1. Enqueue\n2. Dequeue\n3. Tampilkan\n4. Rata-rata Service Time\n5. Keluar\n" << GREEN << "Pilih (1/2/3/4/5) : " << RESET;
        cin >> pilihan;
        cin.ignore();
        cout << BOLD CYAN << setfill('=') << setw(100) << "=" << RESET << endl << setfill(' ');

        switch (pilihan) {
            case 1:
                q.enqueue();
                system("pause");
                break;
            case 2:
                q.dequeue();
                system("pause");
                break;
            case 3:
                q.display();
                system("pause");
                break;
            case 4:
                q.tampilkanRataRata();
                system("pause");
                break;
            case 5:
                cout << BOLD CYAN << setfill('=') << setw(100) << "=" << RESET << endl
                     << setfill(' ') << setw(38) << " " << BOLD RED << "Program berhenti...\n"
                     << BOLD CYAN << setfill('=') << setw(100) << "=" << RESET << endl << setfill(' ');
                return 0;
        }

        cout << BOLD CYAN << setfill('=') << setw(100) << "=" << RESET << endl << setfill(' ');
        cout << GREEN << "Apakah anda melanjutkan program? (Yes/No) " << RESET;
        cin >> loop;

        for (char &c : loop) {
            c = tolower(c);
        }
    } while (loop != "no" && loop != "n");

    cout << BOLD CYAN << setfill('=') << setw(100) << "=" << RESET << endl
         << setfill(' ') << setw(38) << " " << BOLD RED << "Program berhenti...\n"
         << BOLD CYAN << setfill('=') << setw(100) << "=" << RESET << endl << setfill(' ');

    return 0;
}
