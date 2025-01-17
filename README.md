![banner](./doc/img/lite-engine-banner.png)

# ⚠️ Lite-Engine is under development and should not be used for production projects yet. ⚠️

![cube_preview](./doc/img/cube_preview.png)

# The mission.
I created Lite-Engine with the purpose of providing a light-weight, extendable,
free and open source game engine for anyone and everyone. At the heart of the project
is the notion that anyone should be able to make a game for free with no strings
attached.

The engine is licensed under the MIT license. I chose it because it is extremely
permissive. You can do pretty much whatever you want with it. Please read the
provided copy of the MIT license carefuly. It is located in the LICENSE file

# Features
Lite engine is under development and should not be used for production projects yet.
That being said, here is a (mostly) comprehensive list of its current features.

- Basic 3D renderer using OpenGL 4.6. (It's easy to change the OpenGL version)
- Minimal dependencies.
- Simple build system using make
- Verbose and strict naming conventions.
    All functions are named using full english words. No abreviations or shorthand
- An extreme emphasis on modularity. Lite-Engine is intentionally light-weight to 
    enable easy modification.

# My current goals:
- Wayland, and Win32 platform layers.
- support for skeletal animation using the lmod file format.

# Building on Linux (X11)
To build the demo, open a terminal and navigate to the lite-engine folder.
Run this command to build the engine:
```
make -Bj X11
```
At the moment, it is not possible to build lite-engine on a platform that does not use
the X Window System. This will be fixed soon. VERY soon.
