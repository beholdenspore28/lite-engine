# ⚠️ Lite-Engine is under development and should not be used for production projects yet. ⚠️

![cube_preview](./doc/img/cube_preview.png)

# Building on Linux
Ensure you have installed GLFW3 using your package manager of choice before you continue.

To build the demo, open a terminal and navigate to the lite-engine folder.
Run this command to build the engine:
```
$ make -Bj linux
```

# Building on FreeBSD
Ensure you have installed GLFW3 using pkg install before you continue.

To build the demo, open a terminal and navigate to the lite-engine folder.
Run this command to build the engine:
```
$ make -Bj free_bsd
```

# Building on WSL2-Ubuntu on Windows 11 (2025)
```
git clone --recursive https://github.com/beholdenspore28/lite-engine.git
cd lite-engine/
sudo apt install make clang
sudo apt-get install libglfw3
sudo apt-get install libglfw3-dev
make
```
