<h1 align="center">— WaSIFM —<br/>Wiradisastra and Setiaji's Inefficient File Manager</h1>

<p align="center">
  <img alt="GitHub License" src="https://img.shields.io/github/license/SastraWiradisastra/WaSIFM_TUBES-SDA-PRAKTEK-D4">
  <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/SastraWiradisastra/WaSIFM_TUBES-SDA-PRAKTEK-D4">
  <br/> (Preview gambar disini)
</p>


`WaSIFM` adalah sebuah file manager berbasis terminal sederhana yang dikembangkan menggunakan bahasa C dengan library `ncurses` untuk display grafis terminal. Projek ini dibuat untuk memenuhi tugas besar semester 2, mata kuliah Struktur Data dan Algoritma, D4 Teknik Informatika, Politeknik Negeri Bandung yang dibimbing oleh Bapak Wendi Wirasta, S.T., M.T.

*Note: Projek ini dikembangkan secara spesifik untuk ekosistem GNU/Linux.* 


## **Core Features**
- Display TUI multi-kolom menggunakan `ncurses`
- Kolom tree-view style display untuk menunjukkan hirarki direktori
- Display informasi file (i.e. nama file, ukuran file, tipe atau format file, last modified date) serta lokasi/alamat direktori yang ditampilkan
- Manipulasi file dasar (i.e. open file, create file, delete/cut file, rename file, copy file, move file)
- Sistem searching untuk mencari file/direktori (Dengan potensi menggunakan `fzf`)
- Sistem pembuangan ke Trash (terletak di `$HOME/.local/share/Trash/files` menggunakan `trash-cli`)
- File display filtering dan sorting
- (Potensi) Pop-out preview thumbnail menggunakan `Überzug++`


## **Dependencies**
- `ncurses`
- `trash-cli`
- (Potensi) `Überzug++`
- (Tambah lagi kalo masih ada)


## **Installation**
(Aplikasinya belum jadi wak)


## **Usage**
(Aplikasinya belum jadi wak)


## **License**
Repository ini, berserta segala kontennya, dilisensikan di bawah `GPL-3.0` terkecuali disebutkan secara eksplisit. Projek dan library milik pihak ketiga yang digunakan dalam repository ini mungkin tunduk kepada lisensinya sendiri. Lihat isi `LICENSE` untuk informasi lebih lanjut.


## **Authors**
Projek ini dikembangkan oleh mahasiswa Politeknik Negeri Bandung dari kelas 1A-D4 Teknik Informatika: 
- **Naufal Zahran Razzaq (241524020) (https://github.com/SastraWiradisastra)** `Gentoo Linux (Profile amd64/23.0/desktop)` 
- **Zaidan Zulkaisi Setiaji (241524031) (https://github.com/Voynobu)** `Linux Mint 22.1`
