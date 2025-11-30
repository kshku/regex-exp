set(SRCS
    regex.h
    regex.c
    parser.h
    parser.c
    state.h
    state.c
)

target_sources(regexer PRIVATE ${SRCS})
