# OperationalSpace - Custom file formats
## .osm - OperationSpace Modules File
This file format contains a set of module definitions for the engine to load on startup. Multiple OSMs can be loaded at once.
The file starts with the magic 4-byte ident 'OSMF' (0x4F534D46), followed by the header.
### The OSM Header
| Field Name | Field Type | Description |
| ---------- | ---------- | ----------- |
| gameVersion | int32     | This is the version of the game that the modules were created for. The game will refuse to load files made for different versions. | 
| namespace  | char[8]    | The namespace the modules are registered under. Used to prevent name conflicts with different addons. |
| count      | int32      | Number of modules provided by this file.
| 