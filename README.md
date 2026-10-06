<img width="600" height="500" alt="image" src="https://github.com/user-attachments/assets/2b8db43e-16ec-422e-b630-aff9491b2829" />

 # Game Engine Architecture

A component-based game engine built with C++20, utilizing SDL3 for core systems, Box2D for physics, FMOD for audio, and RapidJSON for serialization.

## Features

* **C++20 Architecture:** Leverages modern language features for high performance.
* **Component-Based System:** Decouples logic into reusable, modular behaviors.
* **SDL3 Framework:** Handles window management, graphics context, and event polling.
* **Box2D Physics:** Drives accurate 2D rigid-body simulation and collision detection.
* **FMOD Audio Studio:** Powers spatialized 2D audio and dynamic soundscapes.
* **RapidJSON Serialization:** Manages fast JSON-based game state saving and loading.

## Engine Core Systems

### Audio System
* Powered by FMOD.
* Supports channel groups, volume fading, and 2D spatial audio.

### Input System
* Processes keyboard, mouse, and controller inputs.
* Uses an action-mapping system to bind physical keys to game events.

### Physics System
* Driven by Box2D.
* Updates physics steps, manages collision listeners, and syncs positions to transforms.

### Renderer System
* Uses SDL3 hardware-accelerated rendering.
* Handles texture loading, sprite batching, camera viewports, and UI layers.

## Demo Game Overview

The project includes a built-in demo game showcasing engine capabilities:
* **Actors & Components:** Example setups for player movement, AI, and health.
* **Tilemaps:** Grid-based level rendering with automatic static physics colliders.
* **User Interface:** Canvas system rendering main menus, HUDs, and buttons.

## Project Structure

```text
├── assets/               # Textures, audio banks, tilemaps, config files
├── engine/               # Core engine framework source code
│   ├── audio/            # FMOD wrappers and audio managers
│   ├── core/             # Base Actor, Component, and Scene classes
│   ├── input/            # Input mapping and event handling
│   ├── physics/          # Box2D world management and colliders
│   └── renderer/         # SDL3 rendering pipeline and UI systems
├── game/                 # Demo game logic, specific actors, and components
└── main.cpp              # Engine initialization and main game loop
```

## Getting Started

### Prerequisites

Ensure you have Visual Studio installed with the **Desktop development with C++** workload enabled. You will need the following dependencies linked in your solution setup:
* SDL3
* FMOD Engine SDK
* Box2D
* RapidJSON

### Building the Project

1. Clone the repository:
   ```bash
   git clone https://github.com
   cd your-engine-repo
   ```

2. Open the solution file:
   * Locate the `.sln` file in the root directory and open it with Visual Studio.

3. Configure build settings:
   * Set your build configuration to **Release** or **Debug**.
   * Set your target platform to **x64**.

4. Build and Run:
   * Press `Ctrl + Shift + B` to compile the solution.
   * Press `F5` to run the demo game.
