
SET(INCDIR "../include/teetime")

SET(HEADERS
  ${INCDIR}/common.h
  ${INCDIR}/logging.h
  ${INCDIR}/AbstractStage.h
  ${INCDIR}/AbstractConsumerStage.h
  ${INCDIR}/AbstractProducerStage.h
  ${INCDIR}/AbstractOutputPort.h
  ${INCDIR}/AbstractInputPort.h
  ${INCDIR}/Configuration.h
  ${INCDIR}/Execution.h
  ${INCDIR}/InputPort.h
  ${INCDIR}/OutputPort.h
  ${INCDIR}/Port.h
  ${INCDIR}/Pipe.h
  ${INCDIR}/AbstractPipe.h
  ${INCDIR}/UnsynchedPipe.h  
  ${INCDIR}/SynchedPipe.h  
  ${INCDIR}/Thread.h
  ${INCDIR}/Signal.h
  ${INCDIR}/Runnable.h
  ${INCDIR}/BlockingQueue.h
)

SET(SOURCES
  logging.cpp
  AbstractStage.cpp  
  AbstractOutputPort.cpp  
  Runnable.cpp
  Thread.cpp
)

add_library("teetime" ${HEADERS} ${SOURCES})