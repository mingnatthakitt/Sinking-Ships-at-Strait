# 🚢 Sinking Ships @ Strait ⚓️

<p align="center">
  <img src="COMP2113/ProjectTEMP/sinkingships.png" width="65%" />
</p>
## 👥 Team Members
- Natthakitt Prapunwattana (3036644606)
- Fahtai Tanawongsuwan (3036538194)
- Nutthapon Chaitavatputtiporn (3036525599)

## 🎮 Game Description
**Sinking Ship @ Strait** is a tactical 1v1 strategy game designed for the terminal. Inspired by BattleTabs, players engage in a high-stakes naval battle across a **25x25 grid**. The game combines positioning, special abilities, and resource management. Players select a custom fleet and navigate a procedural map filled with islands to eliminate the enemy Nexus.

## 🌟 Key Features
- **Polymorphic Fleet**: Choose from 5 unique ship types, each with distinct weapon patterns and abilities.
- **Piercing Torpedo Penetration**: Heavy weapons penetrate through multiple targets, clearing entire lines until they hit land.
- **Stealth Mechanics**: Submarines remain invisible until they fire multiple shots or are directly hit.
- **Dynamic Movement**: All ships can move during their turn to reposition or dodge incoming fire.
- **Visual Highlighting**: The currently selected ship is highlighted in **Yellow** for clear tactical awareness.
- **Save & Resume**: Full persistence support allowing you to save your progress mid-battle.
- **Procedural Maps**: Three difficulty levels with randomized island generation and fleet deployment.

## 🚢 Ship Classes & Weapons

| Unit | Icon | Size | Ability | Weapon Type |
| :--- | :---: | :---: | :--- | :--- |
| **Submarine** | `S/I` | 3x1 | Stealth (Hidden until 2nd shot) | Arch (1x1) |
| **Big Boat** | `B` | 3x2 | Juggernaut (High Health) | Piercing Torpedo (Line) |
| **Missile Vessel** | `M` | 1x1 | Kamikaze (Sinks after firing) | Arch (3x3 Blast) |
| **Cruiser** | `C` | 2x1 | Standard Fleet Unit | Arch (1x1) |
| **Long Boat** | `L` | 3x1 | Long-range Sniper | Piercing Torpedo (Line) |

## 📂 Directory Structure
```text
Project/
├── 📄 Makefile             # Build instructions
├── 📄 README.md             # Project documentation
├── 📂 include/             # Header files (.h)
│   ├── Board.h             # Grid & placement logic
│   ├── Cell.h              # Individual tile properties
│   ├── GameManager.h       # Main game loop & state
│   ├── Ship.h              # Polymorphic unit classes
│   └── ...                 # Common constants & inputs
├── 📂 src/                 # Source files (.cpp)
│   ├── GameManager.cpp     # Logic implementation
│   ├── Board.cpp           # Rendering & movement
│   └── ...                 # Component implementations
├── 📂 data/                # Persistence & logs
│   ├── history.txt         # Match outcome history
│   └── *.sav               # User save files
└── 📂 obj/                 # Compiled object files
```

## ⌨️ Controls

| Key | Action |
| :--- | :---: |
| **Arrows** | Move the cursor or move the selected ship |
| **W** | Cycle through your active fleet |
| **R** | Toggle between **SHOOT** and **MOVE** modes |
| **Space/Enter** | Perform an action (Shoot or confirm Move) |
| **S** | Save the current game state to a file |
| **Q** | Immediate quit to terminal |

## 🛠️ Compilation and Execution
To compile the game, use the provided Makefile:
```bash
make
```

To run the game:
```bash
./SinkingShipAtStrait
```

To clean build files:
```bash
make clean
```

## 📜 Coding Requirements Implementation

| Requirement | Implementation Detail |
| :--- | :--- |
| **🎲 Random Events** | Procedural island generation and fleet deployment using `rand()`. |
| **📊 Data Structures** | Use of `vector`, `pair`, and `set` for state management. |
| **Memory Management** | 2D dynamic arrays with explicit `new`/`delete` in the `Board` class. |
| **💾 File I/O** | Custom save/load system for `.sav` files and match history logging. |
| **Modularity** | Clean separation of concerns into 14 header and source files. |

