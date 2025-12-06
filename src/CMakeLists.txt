set(SRCS
    defines.h
    utils.h
    regex.h
    regex.c
    parser.h
    parser.c
    state.h
    state.c
    logger.h
    logger.c
    memory.h
    memory.c
    range.h
    range.c
)

target_sources(regexer PRIVATE ${SRCS})
