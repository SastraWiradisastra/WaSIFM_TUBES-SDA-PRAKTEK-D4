<h1 align="center">— WaSIFM —<br/>Wiradisastra and Setiaji's Inefficient File Manager</h1>

<p align="center">
  <img alt="GitHub License" src="https://img.shields.io/github/license/SastraWiradisastra/WaSIFM_TUBES-SDA-PRAKTEK-D4">
  <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/SastraWiradisastra/WaSIFM_TUBES-SDA-PRAKTEK-D4">
  ![screenshot](https://github.com/SastraWiradisastra/WaSIFM_TUBES-SDA-PRAKTEK-D4/blob/main/wasifm.png)
</p>


`WaSIFM` adalah sebuah file manager berbasis terminal sederhana yang dikembangkan menggunakan bahasa C dengan library `ncurses` untuk display grafis terminal. Projek ini dibuat untuk memenuhi tugas besar semester 2, mata kuliah Struktur Data dan Algoritma, D4 Teknik Informatika, Politeknik Negeri Bandung yang dibimbing oleh Bapak Wendi Wirasta, S.T., M.T.

*Note: Projek ini dikembangkan secara spesifik untuk ekosistem GNU/Linux.* 


## **Core Features**
- Display TUI multi-kolom menggunakan `ncurses`
- Kolom tree-view style display untuk menunjukkan hirarki direktori
- Display informasi file (i.e. nama file, ukuran file, tipe atau format file, last modified date) serta lokasi/alamat direktori yang ditampilkan
- Manipulasi file dasar (i.e. open file, create file, delete/cut file, rename file, copy file, move file)
- Sistem searching untuk mencari file/direktori menggunakan fuzzyfinder (`fzf`)
- File display sorting


## **Dependencies**
- `ncurses` 
- `fzf` 
- `GNU coreutils (i.e. cp, mv, rm)` 
- `GNU Make` 


## **Installation**
Clone repository ini dengan menjalankan

    git clone https://github.com/SastraWiradisastra/WaSIFM_TUBES-SDA-PRAKTEK-D4.git

Pindah ke repository yang sudah di-clone

    cd WaSIFM_TUBES_SDA-PRAKTEK-D4

Untuk melakukan kompilasi, jalankan

    make

Setelah kompilasi, install dengan menjalankan

    make install

Jika ingin menghapus program, maka jalankan

    make uninstall



## **Usage**
| Keybind | Kegunaan |
|:---:| --- |
| <kbd>h l</kbd> | Navigasi window |
| <kbd>j k</kbd> | Navigasi directory |
| <kbd>[ENTER]</kbd> | Membuka/Pindah ke directory item |
| <kbd>[BACKSPACE]</kbd> | Kembali ke parent directory |
| <kbd>n</kbd> | Membuat file |
| <kbd>d</kbd> | Menghapus file |
| <kbd>a</kbd> | Menamakan ulang file |
| <kbd>y</kbd> | Copy file ke cache directory |
| <kbd>p</kbd> | Paste file dari cache directory |
| <kbd>c</kbd> | Clearing directory cache |
| <kbd>m</kbd> | Memindahkan file ke directory lain |
| <kbd>o</kbd> | Sort display directory |
| <kbd>f</kbd> | Search menggunakan fzf |
| <kbd>u</kbd> | Undo action sebelumnya |
| <kbd>r</kbd> | Redo action sebelumnya |
| <kbd>q</kbd> | Quit |

## **License**
Repository ini, berserta segala kontennya, dilisensikan di bawah `GPL-3.0` terkecuali disebutkan secara eksplisit. Projek dan library milik pihak ketiga yang digunakan dalam repository ini mungkin tunduk kepada lisensinya sendiri. Lihat isi `LICENSE` untuk informasi lebih lanjut.


## **Authors**
Projek ini dikembangkan oleh mahasiswa Politeknik Negeri Bandung dari kelas 1A-D4 Teknik Informatika: 
- **Naufal Zahran Razzaq (241524020) (https://github.com/SastraWiradisastra)** `Gentoo Linux (Profile amd64/23.0/desktop)` 
- **Zaidan Zulkaisi Setiaji (241524031) (https://github.com/Voynobu)** `Linux Mint 22.1`
