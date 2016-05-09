
SET(HEADERS
  ../include/teetime/foo.h
)

SET(SOURCES
  foo.cpp
)

add_library("teetime" ${HEADERS} ${SOURCES})