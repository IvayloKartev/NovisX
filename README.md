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
2. Each XML file should have only one root element. It could be \<xml\>\<\/xml\> or \<script\>\<\/script\> or whatever you like
3. You can divide your sections using tags, according to your liking (in the example in assets/script.xml \<scene\> is used for visual novel scenes, and \<play\> for a character moving across generated terrain)
4. The engine's parser recognizes the following tags with special functions and attributes, useful for visual novel game development:
- \<background\> - with the attribute "file" can be used to set background during visual novel scene
- \<music\> - with the attribute "source" can be used to set music themes on loop
- \<menu\> - can be used to import menu files (custom files for appearing menu) - using the "file" attribute
- \<panel\> - everything within a panel element will be rendered at once (basically a panel in visual novels). The panel element can currently contain two elements - \<character\> and \<dialogue\>. It can take two attributes - text-bg (for defining background for the displayed text in a dialogue box) and speaker-bg (for defining background for the text of the speaker's name). They can currently either accept paths to images, or "default", which renders semi-transparent background behind them. If left empty, nothing displays behind the text.
- \<character\> - used to set a character sprite and name. Can be used inside panel component or separately. Can accept scale attribute to scale the sprite (accepts percentages, for example scale="150")
- \<dialogue\> - a wrapper element, that contains \<say\> elements. Can be used inside panel component or separately
- \<say\> - contains text to be displayed and the character saying it (if given). Can be used inside dialogue component or separately
- \<choice\> - used to define a branching in the story. Contains text to be displayed and a script to be loaded. Can be wrapped within \<choices\> tag
- \<page\> - used to declare a static page in the start.xml (static pages are explained in 5.)

5. The engine supports development of static pages (for example, but not limited to, settings, stats, inventory, etc). They consist of UI elements and other tags:
- \<static\> - the root node of each static page. This tag is required to distinguish between dynamic content (like switching panels in visual novels) and static content
- \<font\> - a tag that can be a child node of every component that contains text. Allows to load a font from .ttf file and font size
- \<color\> - a tag that can be a child node of every component that contains text. Allows to set text color using RBGA
- \<label\> - used to define a label component (text suitable for headings and titles as well). Accepts \<font\> and \<color\> tags.
- \<dropdown\> - used to build a dropdown menu. Supports different types (currently only "resolution"). Accepts \<font\>, \<color\> and \<options\>
- \<options\> - wrapper tag for \<option\>  tags
- \<option\> - each option of a dropdown menu. Contains action, which will be executed on click
- \<button\> - defines a button. Supports different types (currently "exit_save" for exiting pages and "link" for redirecting to a particular script). Accepts maximum of one \<link\> tag
- \<link\> - contains the path to the script to be executed on clicking of the button.

6. The engine's parser recognizes the following tags, useful for 2D top-down games:
- \<map\> - used to load a map from a 2D array file (each number in it can represent a separate tile). Wrapper tag for \<tile\>
- \<tile\> - with its "file" attribute can be used to define tiles, that will be used with the loaded map to generate 2D terrain
- \<player_sprite\> - used to define a 2D sprite, that can be controlled by the player to move accross the terrain. Wraps \<direction\> tags
- \<direction\> - defines separate sprites for an entity, depending on the direction it is facing. They go in the following order - up, down, right, left
6. You can always refer to the uploaded example scripts (will upload more in the future)

### Make sure you have the necessary dependencies

The engine currently requires SDL and libxml2. Will add a config script in the future.

## License

This project is is dual-licensed under the following licenses:  
1. **GNU General Public License v3 (GPL-3.0)**  
2. **Commercial proprietary license**  

Anyone can use, redestribute, contribute and modify the software under the clauses, defined by the GPL v3 license, as long as any derivates are distributed under the same license.
Commercial use of the games created with the engine is permitted, as long as their source code is provided.
Only after obtaining a **Commercial proprietart license** derivatives can be released as closed-source software

See [NOTICE.md](NOTICE.md) for explanation of the licensing.
See [LICENSE](LICENSE) for full license text.

