# Sorex Engine Project Instructions

Welcome to the Sorex Engine project. This document provides an overview of the codebase, development workflows, and conventions to help you navigate and contribute effectively.

## Project Overview

**Sorex Engine** (Simple OpenGL Rendering Engine eXtended) is a 2D rendering engine built with C++20 and OpenGL. It is designed to be modular and extensible, utilizing a component-based architecture for its core management.

### Key Technologies
- **Language:** C++20
- **Graphics API:** OpenGL
- **Build System:** CMake (3.12+)
- **Windowing/Input:** GLFW
- **Mathematics:** GLM
- **Logging:** spdlog (via `SxJournalManager`)
- **Resource Loading:** stb (images), tinyxml2 (XML/Fonts)

### Architecture
- **Director:** The central hub of an application. You typically inherit from `Sorex::Director` and override `Initialize()` and `OnDraw(Canvas& canvas)`.
- **Components:** The `Director` manages various components such as `DirectorFileSystem`, `AssetManager`, and `RenderDevice`.
- **Canvas:** Provides a high-level API for 2D drawing (lines, rects, circles, textures, text).
- **Asset Management:** Assets (Textures, Fonts) are loaded via the `AssetManager`, which supports both synchronous and asynchronous loading.
- **Platform Abstraction:** Platform-specific logic (Windows, Linux, Desktop) is encapsulated in the `Engine/Platform` directory.

## Building and Running

### Build Prerequisites
- C++20 compliant compiler (GCC 10+, Clang 10+, MSVC 19.29+)
- CMake 3.12 or higher

### Build Commands (using CMake Presets)

The project uses CMake presets for consistent configurations across environments.

#### Linux (Debug)
```bash
cmake --preset linux_debug
cmake --build build
```

#### Windows (Debug - MSVC)
```bash
cmake --preset win_x64_debug
cmake --build build
```

#### Running the Sandbox
After building, the `Sandbox` executable will be available in the `build/Sandbox` (or `build/bin`) directory.
```bash
./build/Sandbox/Sandbox # Path may vary depending on generator
```

### Unit Tests
Tests are located in `Engine/Tests`. To build and run them:
```bash
cmake --preset unit_tests
cmake --build build/unit_tests
ctest --preset unit_tests_run
```

## Development Conventions

### Coding Style
- **Namespaces:** Core logic resides in the `Sorex` namespace. Sub-namespaces include `Graphics`, `Resource`, `Platform`, `Math`.
- **Type Aliases:** Use the custom type aliases defined in `SxTypes.h`:
  - Basic types: `int32`, `uint32`, `scalar_t`, `String`, `StringView`.
  - Template aliases: `TVector<T>`, `TMap<K, V>`, `TUniquePointer<T>`, `TSharedPointer<T>`.
- **Naming:**
  - Classes/Structs: PascalCase (e.g., `AssetManager`). Note the `Sx` prefix for files names.
  - Methods: PascalCase (e.g., `Initialize()`, `OnDraw()`).
  - Variables: camelCase (e.g., `mAssetManager` for members).
- **Error Handling:** Use the `Status` class for returning success or error information. Avoid exceptions for recoverable errors.
- **Assertions:** Use `SRX_ASSERT(condition)` for internal sanity checks.

### File Organization
- `Engine/Include/Sorex`: Public headers.
- `Engine/Source`: Engine implementation.
- `Engine/Platform`: Platform-specific code.
- `Engine/ThirdParty`: Bundled external libraries.
- `Sandbox`: A sample application demonstrating engine usage.
- `Precompiled`: Precompiled headers for faster build times.

### Formatting and Linting
- **Clang-Format:** A `.clang-format` file is provided. Ensure your code is formatted before committing.
- **Cppcheck:** Static analysis is used. Configuration is in `.cppcheck-supressions` and `Extras/Cppcheck`.

## Contribution Workflow
1. Ensure your code follows the established coding style and formatting.
2. Add unit tests for new features or bug fixes in `Engine/Tests`.
3. Verify that all tests pass using the `unit_tests` preset.
4. Check for static analysis warnings using Cppcheck.
