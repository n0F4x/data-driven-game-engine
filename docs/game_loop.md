# Game Loop

The game loop consists of multiple systems working together.

## The big 3

Synchronization is key, especially around the main building blocks.

### Input processing

- States:
    - player state ↓
    - game state

### Game logic

- States:
    - player state
    - bounding boxes
    - positions ↓

### Rendering

- States:
    - positions
    - visuals

## Sub-systems

- User interface
- Graphics (data modelling)
- Rendering
- Collision and physics
- Animation
- AI
- Audio
- Networking
- Core and script
- Profiling
