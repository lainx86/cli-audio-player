#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <string>
#include <mutex>
#include <limits>
#include <memory>

namespace fs = std::filesystem;

std::mutex displayMutex;
std::string currentInput;

class MarqueeText {
private:
    std::string text;
    size_t position;
    size_t displayWidth;
    std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
    std::chrono::milliseconds delay;
    bool needsMarquee;

public:
    MarqueeText(const std::string& t, size_t width, std::chrono::milliseconds delay = std::chrono::milliseconds(200))
        : text(t), position(0), displayWidth(width), delay(delay) {
        needsMarquee = text.length() > displayWidth;
        lastUpdate = std::chrono::steady_clock::now();
    }

    std::string getDisplayText() {
        if (!needsMarquee) {
            return text;
        }

        auto now = std::chrono::steady_clock::now();
        if (now - lastUpdate > delay) {
            position = (position + 1) % (text.length() + 3);
            lastUpdate = now;
        }

        std::string display;
        for (size_t i = 0; i < displayWidth; ++i) {
            size_t textPos = (position + i) % (text.length() + 3);
            if (textPos < text.length()) {
                display += text[textPos];
            } else {
                display += ' ';
            }
        }

        return display;
    }
};

void clearScreen() {
    // Use system-specific clear command
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

std::string createProgressBar(sf::Time currentTime, sf::Time duration, int width = 30) {
    float progress = 0.0f;
    if (duration != sf::Time::Zero) {
        progress = currentTime.asSeconds() / duration.asSeconds();
    }

    int pos = static_cast<int>(width * progress);

    std::string bar;
    bar += "[";
    for (int i = 0; i < width; ++i) {
        if (i < pos) bar += "=";
        else if (i == pos) bar += ">";
        else bar += " ";
    }
    bar += "]";

    return bar;
}

void displayPlayer(const std::string& songName, const std::string& status, MarqueeText& marquee,
                  const std::string& playbackMode, sf::Time currentTime, sf::Time duration,
                  const std::string& nextSong = "") {
    std::lock_guard<std::mutex> lock(displayMutex);

    clearScreen();  // Clear the entire screen before redrawing

    auto toTimeString = [](sf::Time time) {
        int seconds = static_cast<int>(time.asSeconds());
        return std::to_string(seconds / 60) + ":" +
              (seconds % 60 < 10 ? "0" : "") + std::to_string(seconds % 60);
    };

    std::string progressBar = createProgressBar(currentTime, duration);

    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║          🎵 Terminal Player            ║\n";
    std::cout << "║            Dibuat oleh Feby            ║\n";
    std::cout << "║                 /\\_/\\                  ║\n";
    std::cout << "║                ( o.o )                 ║\n";
    std::cout << "║                 > ^ <                  ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║ Now Playing: " << std::left << std::setw(26) << marquee.getDisplayText() << "║\n";
    std::cout << "║ Status     : " << std::left << std::setw(26) << status << "║\n";
    std::cout << "║ Mode       : " << std::left << std::setw(26) << playbackMode << "║\n";
    std::cout << "║ Time       : " << std::left << std::setw(26)
              << toTimeString(currentTime) + " / " + toTimeString(duration) << "║\n";
    if (!nextSong.empty()) {
        std::cout << "║ Next Song  : " << std::left << std::setw(26) << nextSong << "║\n";
    }
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║ Controls:                              ║\n";
    std::cout << "║ [P] Play/Pause      [N] Next Track     ║\n";
    std::cout << "║ [S] Stop            [B] Previous Track ║\n";
    std::cout << "║ [F] Forward (+10s)  [Q] Quit           ║\n";
    std::cout << "║ [R] Backward (-10s)                    ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║                ʕ •ᴥ•ʔ                  ║\n";
    std::cout << "║    " << progressBar << "    ║\n";  // Progress bar added here
    std::cout << "╚════════════════════════════════════════╝\n";
}

void displaySongList(const std::vector<fs::path>& songs) {
    clearScreen();

    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║             🎼 Daftar Lagu             ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";

    for (size_t i = 0; i < songs.size(); ++i) {
        std::string songEntry = std::to_string(i + 1) + ". " + songs[i].filename().string();
        if (songEntry.length() > 36) {
            songEntry = songEntry.substr(0, 33) + "...";
        }
        std::cout << "║ " << std::left << std::setw(38) << songEntry << " ║\n";
    }

    std::cout << "╚════════════════════════════════════════╝\n";
}

int main(int argc, char* argv[]) {
    std::string folderPath = "music";
    if (argc > 1) {
        folderPath = argv[1];
    } else {
        std::cout << "Usage: " << argv[0] << " <path_to_music_folder>" << std::endl;
        std::cout << "Falling back to default 'music' directory." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    std::vector<fs::path> laguList;

    // Scan folder
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension().string();
            if (ext == ".wav" || ext == ".WAV" || ext == ".mp3" || ext == ".MP3" || ext == ".ogg" || ext == ".OGG") {
                laguList.push_back(entry.path());
            }
        }
    }

    if (laguList.empty()) {
        clearScreen();
        std::cout << "╔════════════════════════════╗\n";
        std::cout << "║   Gak ada lagu yang bisa    ║\n";
        std::cout << "║        diputar.            ║\n";
        std::cout << "╚════════════════════════════╝\n";
        return 1;
    }

    // Tampilkan daftar lagu dengan ASCII art
    displaySongList(laguList);

    // Pilih lagu
    std::cout << "\n╔════════════════════════════╗\n";
    std::cout << "║ Masukkan nomor lagu yang   ║\n";
    std::cout << "║ mau diputar:               ║\n";
    std::cout << "╚════════════════════════════╝\n";
    std::cout << "➤ ";
    int currentTrack;
    std::cin >> currentTrack;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    if (currentTrack < 1 || currentTrack > laguList.size()) {
        clearScreen();
        std::cout << "╔════════════════════════════╗\n";
        std::cout << "║     Nomor gak valid.       ║\n";
        std::cout << "╚════════════════════════════╝\n";
        return 1;
    }

    currentTrack--; // Convert to 0-based index

    sf::Music music;
    std::atomic<bool> stop(false);
    std::atomic<bool> paused(false);
    std::string playbackMode = "Normal";
    sf::Time jumpDuration = sf::seconds(10);
    std::unique_ptr<MarqueeText> marquee;

    auto loadAndPlay = [&](int trackIndex) -> bool {
        // Handle wrap-around for playlist
        if (trackIndex < 0) {
            trackIndex = laguList.size() - 1;
        } else if (trackIndex >= laguList.size()) {
            trackIndex = 0;
        }

        if (music.openFromFile(laguList[trackIndex].string())) {
            currentTrack = trackIndex;
            marquee = std::make_unique<MarqueeText>(laguList[currentTrack].filename().string(), 26);
            music.play();
            return true;
        }
        return false;
    };

    if (!loadAndPlay(currentTrack)) {
        clearScreen();
        std::cout << "╔════════════════════════════╗\n";
        std::cout << "║     Gagal buka file.       ║\n";
        std::cout << "╚════════════════════════════╝\n";
        return 1;
    }

    // Clear everything before showing player
    clearScreen();

    // Thread buat input kontrol
    std::thread inputThread([&]() {
        char cmd;
        while (!stop) {
            std::cin >> cmd;
            cmd = tolower(cmd);

            if (cmd == 'p') {
                if (!paused) {
                    music.pause();
                    paused = true;
                } else {
                    music.play();
                    paused = false;
                }
            }
            else if (cmd == 's') {
                music.stop();
                paused = false;
            }
            else if (cmd == 'f') { // Forward 10 seconds
                sf::Time newTime = music.getPlayingOffset() + jumpDuration;
                if (newTime < music.getDuration()) {
                    music.setPlayingOffset(newTime);
                } else {
                    music.setPlayingOffset(music.getDuration());
                }
            }
            else if (cmd == 'r') { // Rewind 10 seconds
                sf::Time newTime = music.getPlayingOffset() - jumpDuration;
                if (newTime > sf::Time::Zero) {
                    music.setPlayingOffset(newTime);
                } else {
                    music.setPlayingOffset(sf::Time::Zero);
                }
            }
            else if (cmd == 'n') { // Next track (with wrap-around)
                music.stop();
                loadAndPlay(currentTrack + 1);
            }
            else if (cmd == 'b') { // Previous track (with wrap-around)
                music.stop();
                loadAndPlay(currentTrack - 1);
            }
            else if (cmd == 'q') {
                stop = true;
                music.stop();
                break;
            }
        }
    });

    // Main thread untuk update display dan musik
    while (!stop) {
        // Determine next song (with wrap-around)
        int nextTrackIndex = (currentTrack + 1) % laguList.size();
        std::string nextSong = laguList[nextTrackIndex].filename().string();
        if (nextSong.length() > 26) {
            nextSong = nextSong.substr(0, 23) + "...";
        }

        displayPlayer(laguList[currentTrack].filename().string(),
                     paused ? "Paused" : (music.getStatus() == sf::SoundSource::Status::Playing ? "Playing" : "Stopped"),
                     *marquee, playbackMode, music.getPlayingOffset(), music.getDuration(), nextSong);

        auto status = music.getStatus();
        if (status == sf::SoundSource::Status::Stopped && !paused) {
            // Auto-play next track when current finishes (with wrap-around)
            music.stop();
            loadAndPlay(currentTrack + 1);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    stop = true;
    inputThread.join();

    return 0;
}
