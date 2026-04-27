#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <random>    
#include <algorithm> 

// ============================================================================
// SEKCJA LOGIKI ALGORYTMÓW
// ============================================================================

// --- ETAP 1: Scrambling Naiwny (Przesuwanie wierszy) ---
class NaiveScrambler {
public:
    static cv::Mat scramble(const cv::Mat& input, int key) {
        cv::Mat result = input.clone();
        for (int i = 0; i < result.rows; ++i) {
            // Każdy wiersz przesuwamy o inną wartość zależną od klucza i numeru wiersza
            int shift = (key + i) % result.cols;
            // std::rotate przesuwa elementy w pamięci bez ich niszczenia (cyklicznie)
            std::rotate(result.ptr<cv::Vec3b>(i), 
                        result.ptr<cv::Vec3b>(i) + (result.cols - shift), 
                        result.ptr<cv::Vec3b>(i) + result.cols);
        }
        return result;
    }
    static cv::Mat unscramble(const cv::Mat& input, int key) {
        cv::Mat result = input.clone();
        for (int i = 0; i < result.rows; ++i) {
            int shift = (key + i) % result.cols;
            // Odwracamy przesunięcie w drugą stronę
            std::rotate(result.ptr<cv::Vec3b>(i), 
                        result.ptr<cv::Vec3b>(i) + shift, 
                        result.ptr<cv::Vec3b>(i) + result.cols);
        }
        return result;
    }
};

// --- ETAP 2: Czysta Permutacja (Mieszanie pikseli) ---
class PermutationScrambler {
public:
    static cv::Mat scramble(const cv::Mat& input, int key) {
        cv::Mat result = input.clone();
        int total = result.rows * result.cols;
        
        // Tworzymy mapę indeksów (0, 1, 2, ..., N)
        std::vector<int> idx(total); 
        std::iota(idx.begin(), idx.end(), 0);

        // Generator liczb losowych sterowany kluczem (seed)
        std::mt19937 g(key); 
        std::shuffle(idx.begin(), idx.end(), g); // Miesza indeksy w sposób powtarzalny dla danego klucza

        cv::Mat out = cv::Mat::zeros(result.size(), result.type());
        for (int i = 0; i < total; ++i) {
            // i - stara pozycja, idx[i] - nowa pozycja
            out.at<cv::Vec3b>(idx[i] / result.cols, idx[i] % result.cols) = 
                result.at<cv::Vec3b>(i / result.cols, i % result.cols);
        }
        return out;
    }
    static cv::Mat unscramble(const cv::Mat& input, int key) {
        int total = input.rows * input.cols;
        std::vector<int> idx(total); std::iota(idx.begin(), idx.end(), 0);
        std::mt19937 g(key); std::shuffle(idx.begin(), idx.end(), g);

        cv::Mat out = cv::Mat::zeros(input.size(), input.type());
        for (int i = 0; i < total; ++i) {
            // Operacja odwrotna: bierzemy piksel z pozycji idx[i] i kładziemy na i
            out.at<cv::Vec3b>(i / input.cols, i % input.cols) = 
                input.at<cv::Vec3b>(idx[i] / input.cols, idx[i] % input.cols);
        }
        return out;
    }
};

// --- ETAP 3: Hybryda (Permutacja + Szyfrowanie kolorów XOR) ---
class HybridScrambler {
public:
    static cv::Mat scramble(const cv::Mat& input, int key) {
        // 1. Najpierw mieszamy miejsca (Etap 2)
        cv::Mat res = PermutationScrambler::scramble(input, key);
        std::mt19937 g(key + 100); // Inny seed dla kolorów dla bezpieczeństwa
        
        for (int i = 0; i < res.total(); ++i) {
            cv::Vec3b& p = res.at<cv::Vec3b>(i);
            // Operacja XOR zmienia kolor piksela, ale jest odwracalna
            p[0] ^= (g() % 256); p[1] ^= (g() % 256); p[2] ^= (g() % 256);
        }
        return res;
    }
    static cv::Mat unscramble(const cv::Mat& input, int key) {
        cv::Mat res = input.clone();
        std::mt19937 g(key + 100);
        
        // 1. Najpierw odwracamy XOR (ponowne wykonanie XOR z tą samą liczbą cofa zmianę)
        for (int i = 0; i < res.total(); ++i) {
            cv::Vec3b& p = res.at<cv::Vec3b>(i);
            p[0] ^= (g() % 256); p[1] ^= (g() % 256); p[2] ^= (g() % 256);
        }
        // 2. Na końcu odwracamy mieszanie miejsc
        return PermutationScrambler::unscramble(res, key);
    }
};

// ============================================================================
// SEKCJA INTERFEJSU UŻYTKOWNIKA (GUI)
// ============================================================================

// Zmienne globalne przechowujące aktualny stan programu
cv::Mat g_orig, g_scram, g_rest;
int g_key = 12345;
int g_stage = 1; 
int g_wrong = 0;

// Główna funkcja odświeżająca obraz po zmianie suwaków
void refresh() {
    // Ustalenie, czy używamy poprawnego czy błędnego klucza do odkodowania
    int k = g_wrong ? (g_key + 1) : g_key;

    // Wybór algorytmu na podstawie pozycji suwaka
    if (g_stage == 1) { 
        g_scram = NaiveScrambler::scramble(g_orig, g_key); 
        g_rest = NaiveScrambler::unscramble(g_scram, k); 
    }
    else if (g_stage == 2) { 
        g_scram = PermutationScrambler::scramble(g_orig, g_key); 
        g_rest = PermutationScrambler::unscramble(g_scram, k); 
    }
    else { 
        g_scram = HybridScrambler::scramble(g_orig, g_key); 
        g_rest = HybridScrambler::unscramble(g_scram, k); 
    }

    // Łączenie trzech widoków w jeden obraz do wyświetlenia obok siebie
    cv::Mat combined;
    std::vector<cv::Mat> views = { g_orig, g_scram, g_rest };
    cv::hconcat(views, combined);
    cv::imshow("ANALIZA: Oryginal | Scrambled | Odtworzony", combined);
}

// Funkcja wywoływana przez OpenCV przy każdym ruchu suwaka
void on_change(int, void*) { refresh(); }

int main() {
    // Próba wczytania obrazu
    g_orig = cv::imread("obrazek.png");
    if (g_orig.empty()) { 
        std::cout << "Blad! Nie znaleziono obrazek.png. Podaj sciezke: "; 
        std::string p; std::cin >> p; 
        g_orig = cv::imread(p); 
    }
    if (g_orig.empty()) return -1;

    // Konfiguracja okna Panelu Sterowania
    cv::namedWindow("Panel Sterowania", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Etap (1-3)", "Panel Sterowania", &g_stage, 3, on_change);
    cv::setTrackbarMin("Etap (1-3)", "Panel Sterowania", 1);
    cv::createTrackbar("Klucz", "Panel Sterowania", &g_key, 65535, on_change);
    cv::createTrackbar("Zly klucz?", "Panel Sterowania", &g_wrong, 1, on_change);

    // Pierwsze wyrenderowanie obrazu
    refresh();

    std::cout << "\nKlawisze sterowania:\n";
    std::cout << "[S] - Zapisz aktualne wyniki do plikow .png\n";
    std::cout << "[L] - Wczytaj nowy obraz (wpisz sciezke w konsoli)\n";
    std::cout << "[ESC] - Wyjscie z programu\n";

    // Pętla obsługująca klawiaturę
    while (true) {
        char c = (char)cv::waitKey(10);
        if (c == 27) break; // Wyjście po wciśnięciu ESC
        
        if (c == 's' || c == 'S') {
            // Zapisywanie wyników analizy
            cv::imwrite("wynik_scrambled.png", g_scram);
            cv::imwrite("wynik_restored.png", g_rest);
            std::cout << "Zapisano wyniki w folderze z programem.\n";
        }
        
        if (c == 'l' || c == 'L') {
            // Wczytywanie nowego pliku w locie
            std::cout << "Podaj nowa sciezke pliku: "; 
            std::string p; std::cin >> p;
            cv::Mat t = cv::imread(p); 
            if(!t.empty()) { 
                g_orig = t; 
                refresh(); 
            } else {
                std::cout << "Nie udalo sie wczytac pliku!\n";
            }
        }
    }
    return 0;
}