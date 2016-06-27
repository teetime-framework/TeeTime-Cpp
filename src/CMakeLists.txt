# Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

SET(INCDIR "../include/teetime")

SET(HEADERS
  ${INCDIR}/common.h
  ${INCDIR}/logging.h
  ${INCDIR}/Configuration.h
  ${INCDIR}/Execution.h  
  ${INCDIR}/Thread.h
  ${INCDIR}/Signal.h
  ${INCDIR}/Runnable.h
  ${INCDIR}/BlockingQueue.h  
  ${INCDIR}/stages/AbstractStage.h
  ${INCDIR}/stages/AbstractConsumerStage.h
  ${INCDIR}/stages/AbstractProducerStage.h
  ${INCDIR}/ports/AbstractOutputPort.h
  ${INCDIR}/ports/AbstractInputPort.h
  ${INCDIR}/ports/InputPort.h
  ${INCDIR}/ports/OutputPort.h
  ${INCDIR}/ports/Port.h
  ${INCDIR}/pipes/Pipe.h
  ${INCDIR}/pipes/AbstractPipe.h
  ${INCDIR}/pipes/UnsynchedPipe.h  
  ${INCDIR}/pipes/SynchedPipe.h  
)

SET(SOURCES
  logging.cpp  
  AbstractStage.cpp  
  AbstractOutputPort.cpp  
  Configuration.cpp
  Runnable.cpp
  Thread.cpp
)

add_library("teetime" ${HEADERS} ${SOURCES})