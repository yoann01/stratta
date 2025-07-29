# Qt6 Vcpkg Template

This is a minimal C++ Qt6 GUI project using CMake and vcpkg for dependency management.

## Setup

1. Clone vcpkg as a submodule:
   ```sh
git submodule add https://github.com/microsoft/vcpkg.git vcpkg
cd vcpkg
./bootstrap-vcpkg.bat # Windows
./bootstrap-vcpkg.sh  # Linux/macOS
   ```
2. Install Qt6:
   ```sh
./vcpkg/vcpkg install qt6-base
   ```
3. Configure and build:
   ```sh
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
   ```
4. Run the app:
   ```sh
./build/Qt6VcpkgTemplate
   ```

## Notes
- vcpkg is included as a git submodule for easy dependency management.
- CMake automatically uses vcpkg if `VCPKG_ROOT` is set.
