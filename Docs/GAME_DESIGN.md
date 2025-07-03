# HydroGrow Simulator - Game Design Document

## Table of Contents
1. [Game Overview](#game-overview)
2. [Core Mechanics](#core-mechanics)
3. [Progression System](#progression-system)
4. [Plant Database](#plant-database)
5. [Equipment & Systems](#equipment--systems)
6. [Economy & Resources](#economy--resources)
7. [Challenge System](#challenge-system)
8. [Skill Trees](#skill-trees)
9. [Educational Integration](#educational-integration)
10. [Endgame Content](#endgame-content)

## Game Overview

**Genre**: Educational Simulation/Management  
**Target Audience**: Casual gamers interested in gardening, students, hydroponics enthusiasts  
**Platform**: PC (Steam/Epic), Console (planned)  
**Engine**: Unreal Engine 5.5  
**Monetization**: Premium ($19.99), optional cosmetic DLC  

### Core Gameplay Loop
1. Plant seeds in hydroponic containers
2. Monitor and adjust growing conditions
3. Harvest crops for rewards
4. Reinvest in better equipment
5. Unlock new plants and techniques
6. Complete challenges for bonuses
7. Progress through skill trees

## Core Mechanics

### Time System
- **Real-time mode**: 1 game hour = 1 real minute
- **Accelerated mode**: 1 game day = 5 real minutes
- **Offline progression**: Limited growth while away
- **Time controls**: Pause, 1x, 2x, 4x speed

### Plant Growth Mechanics

#### Growth Stages
1. **Seed**: Initial planting (0-10% progress)
2. **Seedling**: First leaves appear (10-25%)
3. **Vegetative**: Rapid growth phase (25-60%)
4. **Flowering**: Reproductive stage (60-80%)
5. **Harvest**: Ready for collection (80-100%)

#### Growth Factors
- **pH Level**: Optimal 5.5-6.5, affects nutrient uptake
- **Nutrients**: N-P-K ratios impact different growth stages
- **Light**: Duration and spectrum affect photosynthesis
- **Temperature**: 65-75°F optimal for most plants
- **Humidity**: 50-70% for vegetative, 40-50% for flowering
- **Oxygen**: Root zone aeration critical for health

### pH Management
- **pH Range**: 4.0-8.0 scale
- **Drift Rate**: ±0.1-0.3 per day based on system
- **Adjustment**: pH Up/Down solutions
- **Automation**: Unlock auto-dosing at Tier 2
- **Testing**: Manual (Tier 1) → Digital meter (Tier 2) → Auto-monitoring (Tier 3)

### Nutrient System

#### Primary Nutrients (N-P-K)
- **Nitrogen (N)**: Vegetative growth, leaf development
- **Phosphorus (P)**: Root development, flowering
- **Potassium (K)**: Overall health, fruit quality

#### Secondary Nutrients
- Calcium, Magnesium, Sulfur

#### Micronutrients
- Iron, Manganese, Zinc, Copper, Boron, Molybdenum

#### EC (Electrical Conductivity)
- Measures nutrient concentration
- Target ranges vary by plant (0.8-2.5 mS/cm)
- Too high: Nutrient burn
- Too low: Deficiencies

## Progression System

### Level Structure (1-30)

#### Tier 1: Beginner's Garden (Levels 1-5)
**Containers**: 4  
**Systems**: Basic DWC buckets  
**Plants**: Lettuce, Basil, Spinach, Mint  
**Features**:
- Manual pH adjustment
- Basic nutrients (1-part solution)
- Simple timer lights
- Hand watering backup

**Learning Objectives**:
- Understanding pH importance
- Basic nutrient mixing
- Light cycle basics (16/8 for leafy greens)
- Recognizing deficiencies

#### Tier 2: Intermediate Setup (Levels 6-12)
**Containers**: 8  
**Systems**: Ebb & flow, improved DWC  
**Plants**: Cherry tomatoes, Peppers, Cucumbers, Strawberries  
**Features**:
- pH controller unlocked
- 2-part nutrient solutions
- Adjustable spectrum LED lights
- EC meter access
- Reservoir recycling

**Learning Objectives**:
- EC management
- Nutrient ratios for different growth stages
- Pruning and training techniques
- Integrated Pest Management basics

#### Tier 3: Advanced Grower (Levels 13-20)
**Containers**: 16  
**Systems**: NFT channels, Vertical towers  
**Plants**: Full-size tomatoes, Melons, Exotic herbs  
**Features**:
- Climate control room
- CO2 supplementation
- Automated dosing pumps
- Data logging and charts
- Beneficial microbe additions

**Learning Objectives**:
- System design and flow rates
- Environmental optimization
- Advanced nutrient strategies
- Companion planting benefits

#### Tier 4: Master Cultivator (Levels 21-30)
**Containers**: 32+  
**Systems**: Aeroponics, Custom hybrids  
**Plants**: Rare varieties, Medicinal herbs, Dwarf fruit trees  
**Features**:
- Full automation suite
- Custom nutrient formulation
- Breeding program
- Research lab
- Commercial operations

**Learning Objectives**:
- Genetic selection and breeding
- Commercial viability
- System redundancy and failsafes
- Teaching and mentorship

### Experience & Leveling
- **XP Sources**: Successful harvests, challenge completion, experimentation
- **Level Requirements**: Exponential curve (100 XP for level 2, 150 for level 3, etc.)
- **Milestone Rewards**: Major unlocks every 5 levels

## Plant Database

### Starter Plants (Tier 1)
| Plant | Growth Time | pH Range | EC Range | Yield | Value |
|-------|-------------|----------|----------|-------|-------|
| Lettuce | 6 days | 5.5-6.5 | 0.8-1.2 | 1 head | 10 coins |
| Basil | 8 days | 5.5-6.5 | 1.0-1.6 | 20 leaves | 15 coins |
| Spinach | 7 days | 6.0-7.0 | 1.8-2.2 | 15 leaves | 12 coins |
| Mint | 10 days | 6.0-7.0 | 1.8-2.4 | 30 leaves | 20 coins |

### Intermediate Plants (Tier 2)
| Plant | Growth Time | pH Range | EC Range | Yield | Value |
|-------|-------------|----------|----------|-------|-------|
| Cherry Tomato | 20 days | 5.5-6.5 | 2.0-3.5 | 50 fruits | 50 coins |
| Bell Pepper | 25 days | 6.0-6.5 | 2.0-3.0 | 8 peppers | 60 coins |
| Cucumber | 18 days | 5.5-6.5 | 1.7-2.5 | 10 cukes | 40 coins |
| Strawberry | 30 days | 5.5-6.5 | 1.0-1.4 | 20 berries | 80 coins |

### Advanced Plants (Tier 3)
- Beefsteak Tomatoes
- Watermelon (dwarf variety)
- Thai Basil
- Lavender
- Kale varieties

### Master Plants (Tier 4)
- Saffron
- Wasabi
- Vanilla orchids
- Ginseng
- Rare heritage varieties

## Equipment & Systems

### Growing Systems

#### Deep Water Culture (DWC)
- **Unlock**: Level 1
- **Cost**: 50 coins
- **Capacity**: 1 plant
- **Pros**: Simple, good oxygenation
- **Cons**: Temperature sensitivity

#### Ebb & Flow
- **Unlock**: Level 6
- **Cost**: 200 coins
- **Capacity**: 4 plants
- **Pros**: Versatile, reliable
- **Cons**: Pump dependency

#### Nutrient Film Technique (NFT)
- **Unlock**: Level 13
- **Cost**: 500 coins
- **Capacity**: 8 plants
- **Pros**: Water efficient, great for leafy greens
- **Cons**: Vulnerable to power outages

#### Aeroponics
- **Unlock**: Level 21
- **Cost**: 1000 coins
- **Capacity**: 12 plants
- **Pros**: Fastest growth, highest yields
- **Cons**: Complex, expensive

### Equipment Upgrades

#### Lighting
1. **Basic Timer Light**: 16/8 cycle only
2. **Adjustable LED**: Variable timing
3. **Full Spectrum LED**: Veg/bloom modes
4. **Advanced LED**: Custom spectrums

#### Monitoring
1. **pH Test Kit**: Manual testing
2. **Digital pH Meter**: Instant readings
3. **pH Controller**: Auto-adjustment
4. **Full Monitoring Suite**: pH, EC, temp, humidity

#### Automation
1. **Simple Timer**: Lights only
2. **Pump Timer**: Feeding schedules
3. **Environmental Controller**: Climate management
4. **AI Assistant**: Predictive adjustments

## Economy & Resources

### Currencies

#### Coins (Primary)
- **Earned by**: Selling produce, completing orders
- **Used for**: Equipment, seeds, nutrients
- **Daily bonus**: 50 coins

#### Research Points
- **Earned by**: Experimenting, perfect grows, discoveries
- **Used for**: Skill tree advancement
- **Bonus sources**: First-time achievements

#### Energy Credits
- **Consumed by**: Lights, pumps, climate control
- **Management**: Balance growth vs. efficiency
- **Renewable options**: Solar panels (Tier 3)

### Market System
- **Base prices**: Fluctuate ±20% daily
- **Seasonal bonuses**: Out-of-season produce worth 2x
- **Bulk orders**: 25% bonus for large quantities
- **Special requests**: Time-limited, high-value orders

## Challenge System

### Daily Challenges (Reset every 24 hours)
- "Maintain pH within 0.1 for 4 hours"
- "Harvest 5 plants at perfect ripeness"
- "Use less than 100 energy credits"

### Weekly Challenges
- "Grow 50 tomatoes"
- "Achieve 90% survival rate"
- "Complete 5 special orders"

### Seasonal Events
- **Spring**: Seed starting marathon
- **Summer**: Yield maximization
- **Fall**: Rare variety festival
- **Winter**: Energy efficiency challenge

### Challenge Rewards
- Coins (100-500)
- Research points (10-50)
- Rare seeds
- Equipment vouchers
- Cosmetic items

## Skill Trees

### Nutrition Specialist
1. **Efficient Mixing**: -10% nutrient usage
2. **Deficiency Detection**: Visual indicators earlier
3. **Custom Formulas**: Mix your own nutrients
4. **Micro Management**: +20% micronutrient effect
5. **Master Chemist**: Unlock exotic formulations

### Automation Engineer
1. **Power Saver**: -15% energy consumption
2. **Redundant Systems**: Backup on failures
3. **Smart Scheduling**: Optimal timing algorithms
4. **Remote Management**: Control from anywhere
5. **AI Integration**: Predictive automation

### Crop Diversification Expert
1. **Seed Saver**: Chance to get seeds from harvest
2. **Cross Breeding**: Create new varieties
3. **Companion Master**: +15% yield with companions
4. **Rare Collector**: Access to exclusive seeds
5. **Heritage Keeper**: Preserve endangered varieties

## Educational Integration

### Tutorial System
- Interactive guided first plant
- Tooltips explaining real concepts
- "Why did this happen?" failure analysis
- Video tutorials for advanced techniques

### Encyclopedia
- Plant profiles with real-world info
- Nutrient deficiency guide with photos
- System comparisons and use cases
- Troubleshooting decision trees

### Real-World Connection
- QR codes linking to actual products
- "Try this at home" simple experiments
- Success stories from real growers
- Partnership with educational institutions

## Endgame Content

### Mastery Challenges
- "Perfect 10": Grow 10 plants with 100% efficiency
- "Speed Demon": Lettuce from seed to harvest in 5 days
- "Diversity Master": 50 different species grown
- "Zero Waste": 30 days with 100% water recycling

### Competitive Features
- Weekly leaderboards
- Seasonal tournaments
- Co-op challenges
- Mentor system for new players

### Sandbox Mode
- Unlimited resources
- All equipment unlocked
- Experimental features
- Share custom scenarios

### Prestige System
- Reset progress for permanent bonuses
- Exclusive prestige plants
- Special cosmetic rewards
- "Master Grower" achievements

---

*This document represents the complete game design for HydroGrow Simulator. All mechanics are based on real hydroponics principles to ensure educational value while maintaining engaging gameplay.*