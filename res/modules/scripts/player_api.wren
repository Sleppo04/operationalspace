class Game {
    foreign static GetPlayerCount()
    foreign static GetPlayerIDs()
    foreign static GetPlayerName(id)

    foreign static MyShipCount()
    foreign static MyShipIDs()
}

class World {
    foreign static GetSectorColumnCount()
    foreign static GetSectorRowCount()
    static GetSectorCount() {
        return World.GetSectorColumnCount() * World.GetSectorRowCount()
    }

    foreign static GetTileColumnCount()
    foreign static GetTileRowCount()
    static GetTileCount() {
        return World.GetSectorColumnCount() * World.GetSectorRowCount()
    }
}


// Now you might ask: Why has this enum foreign members?
// And I would answer, to ensure the enum members have the same values as the C enum members
class Direction {
    foreign static UP
    foreign static DOWN
    foreign static LEFT
    foreign static RIGHT
}

foreign class NoAction {

}

foreign class MovementAction {
    foreign direction
}

foreign class ModuleAction {
    foreign module_id
    foreign activation_state
}

foreign class ModuleDirectionAction {
    foreign module_id
    foreign x_direction
    foreign y_direction
}

// This class should be extended by all
class PlayerController {
    // Name the controller will be registered with, maximum of 64 bytes
    name { "Change this to your Controllers name" }
    ShipTickAction(ship_id) { NoAction.new() }
}