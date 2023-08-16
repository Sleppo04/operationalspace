# OperationalSpace - Game Stats
## Base Stats
The game OperationalSpace mimics the way traditional pen-and-paper RPGs work, and uses a **STAT** system, using 6 different **BASE STATs**
(also just called **STATs** for simplicity).
Those **STATs** are mainly used for three things:
- To tell the user what effects a certain module will have
- To tell the user how 'fit' a specific ship is for a certain situation and 
- To simulate battles and cargo transport.

Attack damage is not calculated using those **STATs** however, for this the weapons **WEAPON STATs** are used.

Those **STATs** are integer values reaching from $-2^{31}$ to $2^{31}$. On ships, they are influenced by the used modules; more specifically their **STAT MODIFIERs**.
The 6 **STATs** used are:

| Displayed Name | Full Name | Description |
| -------------- | --------- | ----------- |
| Sp             | Speed     | The maximum amount of tiles the object can move per round |
| Ma             | Mass      | The mass of a ship, how heavy it is |
| Hu             | Hull      | The maximum amount of hull integrity (health) the object can have |
| Sh             | Shield    | The maximum amount of shield integrity (shield health) the object can have |
| St             | Storage   | The maximum amount of ressources the object can carry at once |
| Ba             | Battery   | The maximum amount of energy the object can hold |

## Module Stats
Modules share a common set of **STATs**. These **STATs** take their effect on the ship the module is installed on.

| Displayed Name | Full Name        | Description |
| -------------- | ---------------- | ----------- |
| StM            | Storage Modifier | The amount of resources the ship can store with this module |
| HuM            | Hull Modifier    | How much hull does this module provide |
| ShM            | Shield Modifier  | Shields this module generates |
| SpM            | Speed Modifier   | Does this module accelerate the ships movement? |
| MaM            | Mass Modifier    | How much the module adds to the ships Mass
| BaM            | Battery Modifier | Energy storage of the module |
 
## Weapon Stats
Weapon modules have an extra set of **STATs** that detemines the weapons damage values, damage types and applied effects to the target: the **WEAPON STATs**.

| Displayed Name | Full Name          | Description |
| -------------- | ------------------ | ----------- |
| Ad             | Armor damage       | Damage dealt to the enemy armor, if shields are down |
| Sd             | Shield damage      | Damage dealt to the enemy shields |
| Aad            | Area Armor damage  | Armor damage for ships in the armor area |
| Ada            | Armor Damage Area  | What is the tile range for ships to be affected from the area armor damage |
| Asd            | Area Shield damage | Shield damage for ships in the shield area |
| Sda            | Shield Damage Area | The tile range for ships to be affected from the area shield damage |
| Cd             | Cooldown           | Time it takes to recharge the weapon after firing (in game ticks) |
| Ae             | Activation Energy  | How much energy does the weapon consume when being fired |
| Re             | Recharge Energy    | The amount of energy the weapon consumes when it is being recharged |

## Ship Prototype Stats
Each ship has a ship prototype and a set of modules.
The ship prototypes define the basic capabilities of a ship and limit the ships abilities.
There are generic ship prototypes for custom ship architectures and specialized prototypes for standard designs.

| Displayed Name | Full Name            | Description |
| -------------- | -------------------- | ---------- |
| Hu             | Hull                 | Hull the prototype provides |
| Hr             | Hull Regeneration    | Autonomous Hull-Repair per round |
| Sh             | Shield               | Shield the prototype provides |
| Sr             | Shield Regeneration  | Autonomous Shield Regeneration |
| Ba             | Battery              | Built-in Energy Capacity of the prototype |
| Br             | Battery Regeneration | How much Energy the ship recuperates per round |
| Ma             | Mass                 | How heavy is the prototype |
| Sp             | Speed                | The speed of the ship, will influence the actions per round |
| MC             | Metal Cost           | How many Metal Resources you have to pay to construct this prototype |
| CC             | Crystal Cost         | How many crystals you have to pay to construct this prototype |
| GC             | Gas Cost             | How many Gas Resources you have to pay to construct this prototype |
