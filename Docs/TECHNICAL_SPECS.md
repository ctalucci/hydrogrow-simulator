# Unreal Engine 5 Technical Specifications
## HydroGrow Simulator

### Engine Configuration

#### Project Settings
```ini
[/Script/EngineSettings.GameMapsSettings]
EditorStartupMap=/Game/Maps/MainMenu
GameDefaultMap=/Game/Maps/MainMenu
GlobalDefaultGameMode=/Game/Blueprints/Core/BP_GameMode.BP_GameMode_C

[/Script/Engine.RendererSettings]
r.DefaultFeature.AutoExposure=False
r.DefaultFeature.MotionBlur=False
r.AntiAliasingMethod=2 ; TAA
r.Lumen.Reflections.Enable=1
r.Lumen.TranslucencyReflections.FrontLayer.Enable=0

[/Script/WindowsTargetPlatform.WindowsTargetSettings]
DefaultGraphicsRHI=DefaultGraphicsRHI_DX12
```

#### Performance Targets
- **Target FPS**: 60 (PC), 30 (Steam Deck)
- **Resolution**: 1080p-4K support
- **Scalability**: Low to Epic settings
- **Loading Times**: <3 seconds between rooms

### Core Architecture

#### C++ Class Hierarchy

```cpp
// Core Game Framework
class AHydroGrowGameMode : public AGameModeBase
{
    // Manages game state, progression, save/load
};

class AHydroGrowPlayerController : public APlayerController
{
    // Handles input, UI management, camera control
};

class UHydroGrowGameInstance : public UGameInstance
{
    // Persistent data, save system, settings
};

// Plant System
class APlantActor : public AActor
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FPlantData PlantData;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProceduralMeshComponent* PlantMesh;
    
    void UpdateGrowth(float DeltaTime);
    void ApplyEnvironmentalFactors();
};

// Hydroponics Systems
class AHydroponicsContainer : public AActor
{
    UPROPERTY(EditAnywhere)
    EContainerType ContainerType; // DWC, NFT, Ebb&Flow, Aero
    
    UPROPERTY()
    TArray<APlantActor*> Plants;
    
    void UpdateNutrientFlow();
    void CheckPHLevels();
};

// Data Management
class UPlantDataAsset : public UPrimaryDataAsset
{
    UPROPERTY(EditAnywhere)
    FPlantSpeciesData SpeciesData;
};
```

#### Blueprint Architecture

**Core Blueprints**
- `BP_GameMode`: Extends AHydroGrowGameMode
- `BP_PlayerController`: Camera management, input handling
- `BP_SaveGame`: Serialized save data structure

**Gameplay Blueprints**
- `BP_PlantBase`: Base class for all plants
- `BP_Container_[Type]`: Specific container implementations
- `BP_EquipmentBase`: Base for pumps, lights, etc.

**UI Blueprints (UMG)**
- `WBP_MainMenu`: Title screen and options
- `WBP_HUD`: Main gameplay interface
- `WBP_PlantInfo`: Detailed plant statistics
- `WBP_Shop`: Equipment and seed purchasing

### Data Systems

#### Data Tables

**DT_PlantDatabase**
| Column | Type | Description |
|--------|------|-------------|
| PlantID | FName | Unique identifier |
| DisplayName | FText | Localized name |
| GrowthTime | float | Days to maturity |
| OptimalPH | FVector2D | Min/Max pH range |
| OptimalEC | FVector2D | Min/Max EC range |
| BaseYield | int32 | Expected harvest amount |
| MarketValue | int32 | Base coin value |
| ModelPath | FSoftObjectPath | Plant mesh reference |

**DT_EquipmentStats**
| Column | Type | Description |
|--------|------|-------------|
| EquipmentID | FName | Unique identifier |
| Category | EEquipmentType | Light/Pump/Sensor/etc |
| PowerUsage | float | Energy credits per hour |
| Effectiveness | float | Performance multiplier |
| Cost | int32 | Purchase price |
| UnlockLevel | int32 | Required player level |

#### Save System Structure

```cpp
USTRUCT(BlueprintType)
struct FSaveGameData
{
    GENERATED_BODY()
    
    // Player Progress
    int32 PlayerLevel;
    int32 ExperiencePoints;
    int32 Coins;
    int32 ResearchPoints;
    
    // Facility State
    TArray<FContainerSaveData> Containers;
    TArray<FPlantSaveData> ActivePlants;
    TMap<FName, int32> Inventory;
    
    // Statistics
    FGameplayStats LifetimeStats;
    TArray<FAchievementProgress> Achievements;
};
```

### Visual Systems

#### Material Setup

**Plant Growth Shader**
- Dynamic material instances for growth visualization
- Vertex animation for wind/growth movement
- Subsurface scattering for realistic leaves
- Parameters: GrowthPercent, HealthColor, NutrientStress

**Water Simulation**
- Translucent material with refraction
- Flow maps for NFT/Ebb&Flow systems
- Bubble particle systems for DWC
- Caustics for visual appeal

#### Lighting Configuration

**Growth Lights**
- Rect lights for LED panels
- IES profiles for realistic light distribution
- Dynamic color temperature (2700K-6500K)
- Performance: Max 4 shadow-casting lights per room

**Environment Lighting**
- Lumen GI for realistic bounce lighting
- Skylight for ambient fill
- Exponential height fog for atmosphere

### Gameplay Systems Implementation

#### Time Management System

```cpp
class UTimeManager : public UGameInstanceSubsystem
{
public:
    // Time multipliers
    float GetCurrentTimeScale();
    void SetTimeScale(float NewScale);
    
    // Day/Night cycle
    float GetTimeOfDay(); // 0-24
    int32 GetCurrentDay();
    
    // Offline progression
    FTimespan CalculateOfflineTime();
};
```

#### Plant Growth Algorithm

```cpp
void APlantActor::UpdateGrowth(float DeltaTime)
{
    float GrowthRate = BaseGrowthRate;
    
    // Environmental factors (0.0 - 1.0)
    float PHFactor = CalculatePHEffect(CurrentPH, OptimalPHRange);
    float NutrientFactor = CalculateNutrientEffect(CurrentEC, OptimalECRange);
    float LightFactor = CalculateLightEffect(LightIntensity, LightSpectrum);
    float TempFactor = CalculateTemperatureEffect(CurrentTemp, OptimalTempRange);
    
    // Apply all factors
    GrowthRate *= PHFactor * NutrientFactor * LightFactor * TempFactor;
    
    // Skill tree bonuses
    GrowthRate *= GetSkillTreeMultiplier();
    
    // Update growth progress
    GrowthProgress += GrowthRate * DeltaTime;
    
    // Visual update
    UpdatePlantVisuals();
}
```

### UI/UX Implementation

#### HUD Layout (UMG)
- **Top Bar**: Resources (Coins, Research, Energy)
- **Left Panel**: Active challenges and objectives
- **Right Panel**: Quick equipment controls
- **Bottom Bar**: Plant selection and tools
- **Center**: 3D viewport with interaction prompts

#### Input Mapping (Enhanced Input)

```cpp
// Input Actions
IA_CameraRotate - Mouse drag
IA_CameraZoom - Mouse wheel
IA_SelectObject - Left click
IA_OpenContextMenu - Right click
IA_QuickHarvest - H key
IA_ToggleSpeed - Space
IA_OpenInventory - I key
```

### Performance Optimization

#### LOD Strategy
- **Plants**: 3 LODs based on growth stage
  - LOD0: Full detail (< 5m)
  - LOD1: Reduced polygons (5-15m)
  - LOD2: Billboard (> 15m)
- **Containers**: 2 LODs
  - LOD0: Full detail with water
  - LOD1: Simplified geometry

#### Instancing
- Use Instanced Static Mesh for repeated elements
- Hierarchical Instanced Static Mesh for plants
- GPU instancing for particle effects

#### Texture Streaming
- Virtual Textures for environment
- Texture streaming pool: 1024 MB
- Compressed textures (BC7 for base color)

### Audio Architecture

#### Audio Classes
- **Master**: Overall volume control
- **SFX**: UI sounds, interactions
- **Ambient**: Room tone, equipment hum
- **Music**: Background tracks
- **Alerts**: pH warnings, harvest ready

#### Implementation
- Audio components on equipment for 3D sound
- Attenuation settings for room acoustics
- Dynamic audio based on room size/equipment

### Networking Considerations (Future)

#### Multiplayer Architecture
- Listen server for co-op (2-4 players)
- Shared garden spaces
- Turn-based management actions
- Real-time visitors to gardens

### Platform-Specific Features

#### PC (Windows)
- Full graphics settings
- Mod support via UGC
- Steam achievements
- Cloud saves

#### Steam Deck
- Optimized UI for 720p
- Controller-first design
- Reduced shadow quality
- 30 FPS target

#### Console (Future)
- Adaptive triggers for watering
- Activity cards for daily tasks
- Platform achievements

### Development Tools

#### Custom Editor Tools
- Plant data importer
- Growth curve visualizer
- Challenge validator
- Localization helper

#### Debug Console Commands
```
HydroGrow.SetPlayerLevel [num]
HydroGrow.AddCoins [amount]
HydroGrow.CompleteGrowth [plantID]
HydroGrow.SetTimeScale [multiplier]
HydroGrow.UnlockAllEquipment
```

### Build Configuration

#### Packaging Settings
- Build Configuration: Shipping
- Include Debug Files: No
- Pak files: Yes
- Compress pak files: Yes
- Create compressed cooked packages: Yes

#### Platform Requirements

**Minimum Specs**
- GPU: GTX 1060 / RX 580
- CPU: Intel i5-8400 / Ryzen 5 2600
- RAM: 8 GB
- Storage: 15 GB

**Recommended Specs**
- GPU: RTX 3060 / RX 6600
- CPU: Intel i7-10700 / Ryzen 7 3700X
- RAM: 16 GB
- Storage: 20 GB SSD

### Version Control

#### Git LFS Tracking
```.gitattributes
*.uasset filter=lfs diff=lfs merge=lfs -text
*.umap filter=lfs diff=lfs merge=lfs -text
*.png filter=lfs diff=lfs merge=lfs -text
*.jpg filter=lfs diff=lfs merge=lfs -text
*.wav filter=lfs diff=lfs merge=lfs -text
```

#### Binary File Management
- Use Git LFS for all binary assets
- Regular commits for C++ source
- Branch protection for main
- Feature branches for new systems

---

*This technical specification provides the foundation for implementing HydroGrow Simulator in Unreal Engine 5, leveraging modern features while maintaining performance across platforms.*