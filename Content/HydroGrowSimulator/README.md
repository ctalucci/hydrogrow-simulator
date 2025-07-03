# HydroGrow Simulator Content Structure

This folder contains all the Blueprint assets and content for the HydroGrow Simulator project.

## Folder Structure

### `/Blueprints/`
Main Blueprint assets organized by category:

#### `/Blueprints/Characters/`
- **BP_HydroGrowCharacter**: Main player character Blueprint (based on AHydroGrowCharacter C++ class)
- Character-related components and sub-blueprints

#### `/Blueprints/Plants/`
- **BP_PlantActor**: Base plant Blueprint (based on APlantActor C++ class)
- Individual plant species Blueprints (tomato, lettuce, herbs, etc.)
- Plant growth stage variants

#### `/Blueprints/Systems/`
- **BP_HydroponicsContainer**: Hydroponic container Blueprint (based on AHydroponicsContainer C++ class)
- **BP_GameMode**: Game mode Blueprint (based on AHydroGrowNetworkGameMode C++ class)
- **BP_GameInstance**: Game instance Blueprint (based on UHydroGrowGameInstance C++ class)
- **BP_PlayerController**: Player controller Blueprint (based on AHydroGrowPlayerController C++ class)
- Interaction and networking system Blueprints

#### `/Blueprints/UI/`
- Main menu and HUD Blueprints
- Plant information panels
- Inventory and shop UI
- Multiplayer lobby and player management UI

### `/Input/`
Enhanced Input system assets:
- **IMC_Default**: Input Mapping Context for default controls
- **IA_Move**: Input Action for movement (WASD)
- **IA_Look**: Input Action for camera look (mouse)
- **IA_Jump**: Input Action for jumping (spacebar)
- **IA_Interact**: Input Action for interaction (E key)
- **IA_Sprint**: Input Action for sprinting (shift)
- **IA_Crouch**: Input Action for crouching (ctrl)
- **IA_Inventory**: Input Action for inventory (tab)

### `/Maps/`
- **MainMenu**: Main menu level
- **TestLevel**: Primary gameplay level for testing
- **MultiplayerLobby**: Multiplayer lobby level

### `/Materials/`
- Plant materials and textures
- Container and UI materials
- Environmental materials

### `/Meshes/`
- Custom meshes for containers and equipment
- Plant mesh variants (if not using procedural generation)

## Blueprint Class Hierarchy

```
AHydroGrowCharacter (C++)
├── BP_HydroGrowCharacter (Blueprint)

APlantActor (C++)
├── BP_PlantActor (Base Blueprint)
    ├── BP_TomatoPlant
    ├── BP_LettucePlant
    ├── BP_BasilPlant
    └── [Other plant species]

AHydroponicsContainer (C++)
├── BP_HydroponicsContainer (Blueprint)

AHydroGrowNetworkGameMode (C++)
├── BP_GameMode (Blueprint)
```

## Getting Started

1. **Character Setup**: Create BP_HydroGrowCharacter based on the AHydroGrowCharacter C++ class
2. **Input Setup**: Create Enhanced Input assets in the `/Input/` folder
3. **Plant Setup**: Create BP_PlantActor and specific plant Blueprints
4. **Container Setup**: Create BP_HydroponicsContainer for the hydroponic systems
5. **UI Setup**: Create user interface Blueprints for the game experience

## Notes

- All C++ classes are already implemented and compiled
- Blueprints should extend the corresponding C++ base classes
- Enhanced Input system requires UE5.0+ features
- Networking functionality is built into the base C++ classes