# Documentation
## Build
[vcpkg](https://github.com/microsoft/vcpkg):
```
vcpkg install detours rapidjson --triplet=x64-windows-static
```
CMake (or open the directory with Visual Studio and config it manually):
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:\...\vcpkg\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build . --config Release
```

For the test you also need:
```
vcpkg install boost-test fmt
```
And add `-DBUILD_TESTING=ON` when calling `cmake ..` .