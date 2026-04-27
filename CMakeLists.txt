cmake_minimum_required(VERSION 3.10)
project(ImageChaosProject)

set(CMAKE_CXX_STANDARD 17)

# 1. SZTYWNE ŚCIEŻKI (Dostosuj, jeśli Twoje OpenCV jest w innym folderze!)
set(MY_OPENCV_INCLUDE "C:/opencv/build/include")
set(MY_OPENCV_LIB_DIR "C:/opencv/build/x64/vc16/lib")

# 2. Informujemy CMake, gdzie szukać biblioteki
set(OpenCV_DIR "C:/opencv/build")
find_package(OpenCV REQUIRED)

# 3. Tworzymy program
add_executable(M2_Chaotic M2_Chaotic.cpp)

# 4. WYMUSZAMY ścieżki inkludów (Globalnie i lokalnie)
include_directories(${MY_OPENCV_INCLUDE})
target_include_directories(M2_Chaotic PRIVATE ${MY_OPENCV_INCLUDE})

# 5. Łączymy z bibliotekami
target_link_libraries(M2_Chaotic ${OpenCV_LIBRARIES})