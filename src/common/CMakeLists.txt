target_sources(operationalspace PRIVATE util.c
            arraylist/arraylist.c
            buffer/buffer.c
            coordinate/coordinate.c
            dynamicbuffer/dynamicbuffer.c
            lexer/lexer.c
            memorypool/memoryarena.c
            memorypool/memorypool.c
            rand/opensimplex.c
            rand/xoshiro256.c
            threading/threading.c)

target_sources(os-server PRIVATE util.c
            arraylist/arraylist.c
            buffer/buffer.c
            coordinate/coordinate.c
            dynamicbuffer/dynamicbuffer.c
            lexer/lexer.c
            memorypool/memoryarena.c
            memorypool/memorypool.c
            rand/opensimplex.c
            rand/xoshiro256.c
            threading/threading.c)
