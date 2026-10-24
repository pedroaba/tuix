# Tuix

**Tuix** is an experimental, open-source C++ UI engine focused on building modern desktop interfaces with a component-driven architecture.

Inspired by the composability and design principles of projects such as shadcn/ui, Tuix aims to provide reusable components, a consistent design system, and a developer-friendly C++ API — without relying on web technologies to render the interface.

> [!IMPORTANT]
> **Early development / learning project.** Tuix is being built incrementally to explore UI engine architecture, graphics rendering, event handling, and modern C++. The features described in the roadmap are goals, **not existing functionality**.

## Project goals

- Build a component system in modern **C++20**.
- Draw interfaces with **Skia**, using a rendering layer owned by Tuix.
- Use **SDL3** for window creation and operating-system events.
- Support composable interfaces with reusable components.
- Develop independent layout, state, event, and theming systems.
- Keep rendering details separate from the public component API.
- Work toward cross-platform support on macOS, Linux, and Windows.

Tuix is not a C++ port of shadcn/ui. It takes inspiration from the *design philosophy* of reusable, customizable components while implementing its own native architecture.

## Current status

Development is at the foundation stage: setting up the C++ project, initializing SDL3, and establishing the application's event loop. **Skia integration and the component system are planned but not yet implemented.**

## Technology stack

| Technology | Role | Status |
| --- | --- | --- |
| C++20 | Core language | In use |
| CMake | Build system | In use |
| SDL3 | Native windows and events | Initial integration |
| Skia | 2D graphics and text rendering | Planned |

## Getting started

### Requirements

- A C++20-compatible compiler
- CMake **3.20** or newer
- SDL3, including its CMake package configuration

The instructions below target **macOS with Homebrew**. Other platforms will need their own SDL3 installation and build setup.

### Install dependencies (macOS)

```sh
brew install cmake sdl3
```

### Configure and build

From the project root:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix sdl3)"
cmake --build build
```

Run the executable:

```sh
./build/tuix
```

The executable target is currently named `tuix`. These instructions cover the **SDL3 foundation**; they do not yet build a Skia-backed component renderer.

### Editor integration (Clangd)

The CMake project should export a compilation database:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

If Clangd does not discover the database in `build/`, add a `.clangd` file at the project root:

```yaml
CompileFlags:
  CompilationDatabase: build
```

Then restart the Clangd language server in your editor.

## Planned architecture

```text
Application
    |
Component tree
    |
Layout / state / events / themes
    |
Tuix renderer abstraction
    |
Skia (2D drawing)
    |
SDL3 (window / input / presentation)
```

The intention is to keep components independent of the drawing backend. This architecture is a **design direction**, subject to change as the engine evolves.

## Roadmap

- [x] Set up C++20 and CMake
- [x] Add SDL3 as a project dependency
- [ ] Create and manage the first native window and event loop
- [ ] Integrate Skia and present rendered pixels through SDL3
- [ ] Implement Tuix's renderer abstraction
- [ ] Implement primitive components (Text, Box, Button)
- [ ] Add mouse interaction, hit testing, and keyboard focus
- [ ] Build a retained component tree
- [ ] Create layout primitives (Row, Column, Stack, Padding)
- [ ] Add themes, design tokens, and component variants
- [ ] Implement text inputs and more advanced components
- [ ] Add tests, examples, and developer documentation
- [ ] Validate cross-platform builds

## Design principles

**Learn by building.** Core engine behavior is implemented and studied step by step, rather than hidden behind an existing widget framework.

**Components first.** Application code should describe UI elements and their behavior, not low-level drawing operations.

**Clear separation of concerns.** Platform events, layout, component state, and rendering have distinct responsibilities.

**Customization by design.** Themes and component variants should be first-class features, not hard-coded styles.

## Contributing

Tuix is a personal learning project in an early stage. Suggestions, discussions, and bug reports are welcome. Please open an issue before proposing large architectural changes.

## License

Tuix is licensed under the **MIT License**. See [LICENSE](LICENSE) for details.

SDL3 and Skia are third-party projects with their **own** licenses; the Tuix MIT license does not replace their terms.
