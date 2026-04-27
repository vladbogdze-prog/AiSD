### 2. Chaotyczne przekształcanie obrazu cyfrowego (C++)
* **Folder:** `M2_ChaoticScramble`
* **Opis:** Zaawansowany system szyfrowania obrazów wykorzystujący permutację pikseli oraz substytucję (Etapy 1 Naiwny scrumbling,
* 2 Permutacja sterowana kluczem, 3 Permutacja i Sucstytucja).
* **Technologie:** C++, OpenCV, CMake, platforma - Visual Studio Code.
* **Interfejs:** Graficzny interfejs użytkownika (GUI) z suwakami do analizy statystycznej korelacji i wrażliwości klucza
* (GUI jest narzędziem analitycznym, nie elementem dekoracyjnym), Przycisk ESC zamyka program, lecz dla pewności zaleca się
* kliknąć w Terminal kombinację przycisków Ctrl+c, to zamknie program wpełni, również jest przycisk S, który w folder build\Debug zasejfuje wyniki
* teraźniejszego szyfrowania obrazu, przycisk L daje możliwość dynamicznej zmiany obrazu, by działało w Terminal trzeba wpisać bezporednią
* ściężkę w komputerze do obrazu.
* **Przyszykowanie** Aby program działał, najpierw trzeba zainstalować skrypt, oraz bibliotekę OpenCV (we własnym zakresie),
* następnie, gdy program będzie otwarty na platformie, trzeba kliknąć build, by przekształcić kod w działający plik,
* następnie by wszystko działało trzeba dodać obrazki "obrazek.png" i "gradient.png" do folderu build\Debug. Możno dodać inne, lecz dla analizy pasują te.

