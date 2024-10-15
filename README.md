# Client (the game)

## Build and run

(Raylib installation is not needed!)

### Mac OS

```sh
./build-macos.sh
```

### Windows (msvc)

Make sure to select the terminal profile: `x64NativeToolsCommandPrompt`

```bat
build-msvc.bat
```

## Repo layout

### .git

Duh. Change `Files: Exclude` setting if invisible.

### .vscode

Includes project workspace settings :sunglasses:

### bin

The target for compiled files. Untracked.

### docs

Documentation, resources.

### external

#### include

Raylib header files.

#### lib

Raylib library files.

### map-transformer

Script for generating the map. More info in `map-transformer/README.MD`.

### src

The game :D
