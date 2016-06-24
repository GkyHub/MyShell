PROJECT(yaush)
INCLUDE_DIRECTORIES(include)
AUX_SOURCE_DIRECTORY(src DIR_SRCS)

SET(MAIN ${DIR_SRCS})

ADD_EXECUTABLE(./yaush ${MAIN})

TARGET_LINK_LIBRARIES(./yaush libm.so)
TARGET_LINK_LIBRARIES(./yaush libreadline.so)
TARGET_LINK_LIBRARIES(./yaush libhistory.so)

add_definitions(-DDEBUG -g3 -Wall -lncurses)
