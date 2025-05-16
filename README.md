# NovisX
Simple game engine for developping visual novels.

## Installation

### Using Pre-built release

After downloading the executable file, place it in one directory along with assets/ directory. All scripts and assets should be placed within the assets/ directory.

### Compiling from source code

Running make inside the main source tree will result in an executable "Game" in the same root directory.

## Developing games using the engine

The engine can be used to develop simple games using XML-like scripting languages. All the scripts written should be placed inside the assets/ directory.

### Simple guide how to make a game using the engine

1. The start point of every program should be called start.xml. All branching, defining menus and assets should be first initialized inside.
2. Each XML file should have only one root element. It could be <xml></xml> or <script></script> or whatever you like
3. You can divide your sections using tags, according to your liking (in the example in assets/script.xml <scene> is used for visual novel scenes, and <play> for a character moving across generated terrain)
4. The engine's parser recognizes the following tags with special functions and attributes, useful for visual novel game development:
- <background> - with the attribute "file" can be used to set background during visual novel scene
- <music> - with the attribute "source" can be used to set music themes on loop
- <menu> - can be used to import menu files (custom files for appearing menu) - using the "file" attribute
- <panel> - everything within a panel element will be rendered at once (basically a panel in visual novels). The panel element can currently contain two elements - <character> and <dialogue>
- <character> - used to set a character sprite and name. Can be used inside panel component or separately
- <dialogue> - a wrapper element, that contains <say> elements. Can be used inside panel component or separately
- <say> - contains text to be displayed and the character saying it (if given). Can be used inside dialogue component or separately
- <choice> - used to define a branching in the story. Contains text to be displayed and a script to be loaded. Can be wrapped within <choices> tag
5. The engine's parser recognizes the following tags, useful for 2D top-down games:
- <map> - used to load a map from a 2D array file (each number in it can represent a separate tile). Wrapper tag for <tile>
- <tile> - with its "file" attribute can be used to define tiles, that will be used with the loaded map to generate 2D terrain
- <player_sprite> - used to define a 2D sprite, that can be controlled by the player to move accross the terrain. Wraps <direction> tags
- <direction> - defines separate sprites for an entity, depending on the direction it is facing. They go in the following order - up, down, right, left
6. You can always refer to the uploaded example scripts (will upload more in the future)

### Make sure you have the necessary dependencies

The engine currently requires SDL and libxml2. Will add a config script in the future.

## License

This project is licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0).

You are free to use this engine to develop and sell your own games, without any need to open source your game code.

However, any modifications to the engine itself must be published under the same LGPL-3.0 license.

See [LICENSE](LICENSE) for full license text.

