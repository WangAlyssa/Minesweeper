cmake_minimum_required(VERSION 3.17)
project(Minesweeper)

# 1. 强制使用 C++17 (这是 SFML 3.0 的硬性要求)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. 告诉 CMake 在 Homebrew 路径下寻找库 (针对 M1/M2 Mac)
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

# 3. 寻找 SFML 3 库
# 如果这里报错，说明您的电脑还没安装 SFML 3 (需运行 brew install sfml)
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# 4. 添加所有源文件 (根据您的截图，这些文件名都是对的)
add_executable(Minesweeper
        main.cpp
        Board.cpp
        Board.h
        Tile.cpp
        Tile.h
        Leaderboard.cpp
        Leaderboard.h
        TextureManager.cpp
        TextureManager.h
)

# 5. 链接 SFML 库
target_link_libraries(Minesweeper PRIVATE SFML::Graphics SFML::Window SFML::System)