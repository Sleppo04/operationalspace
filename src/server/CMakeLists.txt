target_sources(os-server PRIVATE gameobject.c
            main.c
            modulestore.c
            stats.c
            world.c
            worldgen.c)
add_subdirectory(ubc)
