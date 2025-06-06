> [!WARNING]
> The project is still in its early stages.

# A data-driven game engine

This is my pet project, developed in my free time.
The goals are very simple:
Create an efficient and easy to use game engine/framework that can be "scripted" in its native language.

This is achieved landing the `main` function in the developers' hands and providing a very flexible easy-to-extend architecture.
(Check out the [examples](examples/src/demos).)

### Major features

- I am currently working on a custom archetype-based ECS framework.
This will later act as a building block for writing efficient multi-threaded systems.

- There already exists a minimal [Vulkan](https://www.vulkan.org/) renderer that needs rewriting.

I took [Bevy](https://bevyengine.org/) as a huge an inspiration, in combination with its [stageless RFC](https://github.com/bevyengine/rfcs/blob/main/rfcs/45-stageless.md).

## Building

Building is a little complicated as I don't support multiple setups for the ease of my development.

See the [docs](docs/BUILDING.adoc) if you would like to give it a try.

## Features

- glTF loading
- Basic Vulkan renderer
- Virtual texturing
- Entity Component System (in progress)

## Screenshots

Sponza glTF model with PBR \
![](docs/screenshots/Sponza-pbr.png)

Damaged Helmet glTF model with PBR \
![](docs/screenshots/DamagedHelmet-pbr.png)

## Development

Development related docs can be found in the [docs](docs) folder.
