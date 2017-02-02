# Verde
This is a game engine for a game I'm planning to make.
All geometry is based on rectangles.

The build system currently only works on linux

## Compile

`premake5 gmake && make`

Required libraries:
- OpenGL
- GLFW
- GLM

## Controls

General:
- `WASD`: control the character
- `Space` or `W`: jump

Editing:
- `tab`: toggle edit mode (not necessary currently, but useful, because it displays box edges)
- `middle click`: create new box
- `right click`: select a box
- `left click`: edit the boxes size
- `1` - `4`: change texture of currently selected box
- `X`: delete currently selected box
