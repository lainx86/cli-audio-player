# CLI Audio Player 🎧
## This is personal project

Pemutar audio minimalis via terminal, ditulis dengan C++.  
Gak ada UI, gak ada warna, cuma suara.

### Akan dikembangkan lebih jauh lagi, nanti... jika aku nggak males-malesan

## Fitur

- Memindai folder `music/` untuk file `.wav`
- Menampilkan daftar lagu
- Memutar lagu berdasarkan pilihan user
- Dibuat untuk belajar

## Requirements

- C++17 atau lebih baru
- [SFML 2.5](https://www.sfml-dev.org/download/sfml/2.5.1/) (hanya audio module)

### MacOS (via Homebrew)

```bash
brew install sfml
```

## Cara Build

1.  **Clone repositori ini:**
    ```bash
    git clone https://github.com/feby-s/cli-audio-player.git
    cd cli-audio-player
    ```

2.  **Buat direktori build:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Konfigurasi dengan CMake:**
    ```bash
    cmake ..
    ```
    Jika CMake tidak dapat menemukan SFML, Anda mungkin perlu menentukan lokasinya secara manual:
    ```bash
    cmake .. -DSFML_DIR=/path/to/sfml/lib/cmake/SFML
    ```

4.  **Compile proyek:**
    ```bash
    cmake --build .
    ```

5.  **Jalankan aplikasi:**
    Setelah build selesai, executable `cli-audio-player` akan berada di dalam direktori `build`.
    ```bash
    ./cli-audio-player
    ```
    Atau, Anda bisa menentukan path ke folder musik Anda:
    ```bash
    ./cli-audio-player /path/to/your/music
    ```
