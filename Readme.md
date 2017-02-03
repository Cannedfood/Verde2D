# Verde
This is a game engine for a game I'm planning to make.
All geometry is based on rectangles.

The build system currently only works on linux

## Compile

- `premake5 gmake && make`

or short

- `./tools/gen`
- or `./tools/genclang` if you like clang more

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
- `1` - `3`: change texture of currently selected box
	- `1`: rocks (default)
	- `2`: dark rocks
	- `3`: dirt
- `X`: delete currently selected box
- `K`: toggle snapping (whether boxes should be aligned to a 0.5 distance grid)
- Drag and drop an image to make it the texture of the current box
- `ctrl-S`: Save current level
- `ctrl-O`: Load last save state
- `arrow-up`: Move higher
- `arrow-down`: Move lower
