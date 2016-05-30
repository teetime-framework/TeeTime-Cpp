
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
  ${INCDIR}/UnsynchedPipe.h
)

SET(SOURCES
  logging.cpp
  AbstractStage.cpp  
)

add_library("teetime" ${HEADERS} ${SOURCES})