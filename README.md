# BomberMan

*Projet de Romain, Vassili, Loïs
Implémentation du BomberMan en C.

## How to build

Make sure CMake and SDL are installed.

Configure the project (once):
```sh
# Clone the project
$ git clone git@gitlab.ensimag.fr:bernatv/bomberman
$ cd bomberman

# Update submodules
$ git submodule update --init

# Configure CMake
$ cmake -B build
```

Then build with:
```sh
$ make -C build
```

Launch the game with:
```sh
$ ./build/bomberman
```
