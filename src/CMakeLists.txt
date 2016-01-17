
SET(HEADERS
  ../include/johl/foo.h
)

SET(SOURCES
  foo.cpp
)

add_library("foo" ${HEADERS} ${SOURCES})