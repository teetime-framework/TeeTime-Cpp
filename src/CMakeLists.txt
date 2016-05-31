
SET(INCDIR "../include/teetime")

SET(HEADERS
  ${INCDIR}/common.h
  ${INCDIR}/logging.h
  ${INCDIR}/AbstractStage.h
  ${INCDIR}/AbstractConsumerStage.h
  ${INCDIR}/AbstractProducerStage.h
  ${INCDIR}/Configuration.h
  ${INCDIR}/InputPort.h
  ${INCDIR}/OutputPort.h
  ${INCDIR}/Port.h
  ${INCDIR}/Pipe.h
  ${INCDIR}/AbstractPipe.h
  ${INCDIR}/UnsynchedPipe.h  
  ${INCDIR}/SynchedPipe.h  
  ${INCDIR}/Thread.h
)

SET(SOURCES
  logging.cpp
  AbstractStage.cpp  
  Thread.cpp
)

add_library("teetime" ${HEADERS} ${SOURCES})