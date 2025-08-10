# DirectX 11 Hooking Project

This project provides a basic framework for hooking DirectX 11 applications to capture rendering calls. It was created to demonstrate how to intercept `IDXGISwapChain::Present` and `ID3D11DeviceContext::DrawIndexed` for purposes like capturing game subtitles or other text.

The project is composed of two main parts:
1.  `dx11hook.dll`: A DLL that, when injected into a process, hooks DirectX 11 functions.
2.  `injector.exe`: A command-line tool to inject the DLL into a target process.

## File Structure

- `CMakeLists.txt`: The main build script for the entire project.
- `dllmain.cpp`: The entry point for the `dx11hook.dll`.
- `hooks.h` / `hooks.cpp`: Contains the core logic for setting up and managing the DirectX hooks.
- `injector/`: Subdirectory containing the source code and build script for the injector application.
- `minhook/`: Subdirectory containing the source code for the MinHook library, which is used to create the function hooks.
- `README.md`: This file.

## How to Compile

This project uses CMake to generate build files for your specific compiler and platform (e.g., Visual Studio on Windows).

### Prerequisites
- A C++ compiler (e.g., Visual Studio with C++ workload).
- CMake (https://cmake.org/download/).

### Build Steps

1.  **Create a build directory:**
    Open a terminal or command prompt in the project's root directory and run:
    ```bash
    mkdir build
    cd build
    ```

2.  **Run CMake to generate project files:**
    If you are using Visual Studio 2019 or later, you can run:
    ```bash
    cmake .. -G "Visual Studio 16 2019" -A x64
    ```
    (Adjust the generator `-G` and architecture `-A` for your environment). If you have `make` installed, you can just run `cmake ..`.

3.  **Build the project:**
    If you generated Visual Studio files, you can open the `.sln` file in the `build` directory and compile from there. Alternatively, you can build from the command line:
    ```bash
    cmake --build . --config Release
    ```

    After building, you should find `dx11hook.dll` and `injector.exe` inside the `build/bin/Release` directory.

## How to Use

1.  **Identify the target application:**
    Find a DirectX 11 game or application that you want to hook. Make sure it is running.

2.  **Run the injector:**
    Open a command prompt or PowerShell window, navigate to the directory containing `injector.exe` and `dx11hook.dll` (e.g., `build/bin/Release`), and run the injector with the name of the target process as an argument.

    For example, if you want to inject into `ac_client.exe`:
    ```bash
    ./injector.exe ac_client.exe
    ```

3.  **Verify the injection:**
    If the injection is successful, a file named `hook.log` should be created in the target application's directory (e.g., where `ac_client.exe` is located).

    The log file should contain messages like:
    ```
    DLL Injected. Starting hook thread.
    Hooks created and enabled successfully.
    Present hook called for the first time.
    DrawIndexed called. IndexCount: ...
    ```

4.  **Unhook the DLL:**
    To unhook the DLL, press the `END` key on your keyboard while the target application is in focus. This will clean up the hooks and unload the DLL. The `hook.log` file will have a final "Unhooking..." message.
