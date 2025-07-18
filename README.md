> [!WARNING]
> The project is still in its early stages.

# A data-driven game engine

This is my pet project, developed in my free time.
The goals are very simple:
Create an efficient and easy to use game engine/framework that can be "scripted" in its native language.

This is achieved by landing the `main` function in the developers' hands and providing a very flexible easy-to-extend architecture.

I took [Bevy](https://bevyengine.org/) as a huge an inspiration, in combination with its [stageless RFC](https://github.com/bevyengine/rfcs/blob/main/rfcs/45-stageless.md).

## Features

- App builder
- Resource management
- Entity Component System
- Task scheduling primitives
- Event system
- State management
- Asset management
- Timers

### Outdated

- glTF loading
- Basic Vulkan renderer
- Virtual texturing

## Documentation

- [Building](docs/BUILDING.adoc)
- [Developers' guide](docs/Guidelines.md)
- [Tutorials](docs/tutorials)
- [Examples](examples)

## Screenshots

These screenshots were taken from the outdated examples.
The engine's renderer is in need of a rework.

Sponza glTF model with PBR \
![](docs/screenshots/Sponza-pbr.png)

Damaged Helmet glTF model with PBR \
![](docs/screenshots/DamagedHelmet-pbr.png)
