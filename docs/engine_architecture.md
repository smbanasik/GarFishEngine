# Engine Architecture {#enginearchitecture}

With graphics, model loading, and basic text rendering done, we're at an important point in our engine. It's time to take our rendering demo to an actual engine. Our end goal is a dependency graph of everything that will happen in a given frame, as well as "hooks" that a game developer can use to insert their code at certain intervals. This, of course, will pave the way for multithreading the engine.

We do not want to be too perscriptive of what gets executed when. We care about "game logic" executing before rendering, but we do not care about what is executed within that.

## The Frame Pipeline

At the time of writing this document, our engine does the following, in order:
- Checks to see if we should end the engine
- Starts a system clock at the beginning of a frame
- Polls events
- Determines qualities of our window (should it be rendered, should it be resized)
- Paints the ImGui ui
- Runs our entire graphics Pipeline
  - Updates our scene
	- Updates the camera
	- Clears all surfaces
	- Draws loaded items
	- Calculates viewproj matrix
	- Sets scene data
  - Awaits next frame
  - Sets up frame data
  - Draws to the frame
  - Presents the frame
- Determines if we should kill the game or not
- Finishes counting the frame time

You'll notice there is no room for game logic, and our auxillary systems are somewhat lackluster. First, we're going to want a dedicated camera module instead of our ad hoc system. We'll also want a dedicated function to provide some engine defaults to our camera input.

I hadn't originally planned on this, but I think implementing an asset system around now is a correct play. With the asset system complete, we'll have a great opportunity to implement the pipeline for our frames

Then, we'll work on some surrounding systems to help as the engine becomes more complex: logging, CVars, and a profiler. Later on I'd like to revamp the WInput module to produce events that are consumed by game logic down the line instead of directly calling functions.

Next, we're going to want to add a system for game logic. We're going to want to make use of an Entity Component System, and we'll need to do research on how to implement this (compiled below). The end goal is to allow a user to arbitrarily create components and systems, and arbitrarily link those components via entities. This can all be done inside of a module called a World, Coordinator, or Registry. Not sure on which name I prefer yet.

Finally, we'll improve the graphics pipeline again: separating the draw and cull logic, scene management, improving how we send data to the graphics card, and an asset system.

All of this will give us enough structure to create a multithreading module and integrate it with the rest of our engine.

## Logging
Create a file `./common/logger.hpp` and `./common/logger.cpp`. Have an enum called `LogLevel` which has the following: DEBUG, INFO, WARN, ERROR. There should be some configurable state for the logger such as whether it should write to a file or not, and what log levels should count. There should additionally be configuration for writing to std out or std err, and whether to timestamp each log.
Once everything is configured we create a `Logger::Log()` function that takes a `LogLevel`, and an error message.
There's plenty of room for improvement, but for now I think this is acceptable.

## Asset Management

Currently we handle assets directly in `vulkan.cpp` and `vulkan.hpp` and there's several things to adjust to load in assets. We do abstract gltf from asset loading, which is nice, but I'd like for asset management to be in it's own module: the `AssetManager`. The asset manager should take files to load as input, handle them with a respective input adapter (such as `gltf.hpp`), and output an asset ready for ownership by a node... Since the asset manager isn't owning, we need a different name, `AssetLoader`. This should be somewhat similar to the structure we have for the `ImgBuffAllocator`, but without a need to tie the resources to the allocator.

### Current method:
Init default data
Get a structure path, load_gltf
place into loaded scenes
loaded scenes and loaded nodes, call draw functions to put them into main draw context
So do we really need an asset loader?

That's our end goal though: creating a simplified way of loading assets.

## Function Hooks: A precursor to ECS

Need phases for hooks. Pre and post for updates and render? Physics phase as well for when we add it.
Hook should have a phase, be sorted based off of priority, and a system which contains an std function.

Need pending adds and removes.

A phase should contain vectors of our systems with pending adds and removes.
Then we have a vector that contains every phase.

Since these are systems, they should be owned by our ECS "world"

## The ECS Architecture

### The problem

Suppose you're making an RTS game. You have thirty-two thousand units, all of which are moving, shooting, taking damage, being created, and being destroyed. How can we handle this all efficiently? There are two things we want to take advantage of here: caches and multithreading. To make the best use of these we need data locality and data isolation.

Data locality is simple: we need to place our data in such a manner that is compact and predictable. The best way for us to implement this is to split our data based on how it's used instead of how it is logically organized. Rather than an array of objects, we split the class's member variables into arrays and operate on them in bulk. We can get a complete picture of an object by linking these components together with an entity ID.

Data isolation is more difficult since we need to figure out our dependencies. Ideally, we want as many parallelizable actions as possible. In that case, we should develop our systems in a way that they do not rely on one another.

### Entities

At their core, entities are just integers. That's it. `using uint32_t entity` (or 64 if we really need it).

**Generation**:
For now, increment up by one.

**Interface**:
We need a system for adding and removing entities, as well as connecting entities to their respective components.

### Components

Think of components as the data of an OOP class separated into arrays. Rather than a "name" class, we have a "player names" array which contains the name of every player.

**Interface**:
We need a way for players to easily create components. Should be template based.

### Systems
It's the behavior of the a class, like a method.

**Interface**:
Need a way for players to hook systems to our engine.