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
  ${INCDIR}/platform.h
  ${INCDIR}/logging.h
  ${INCDIR}/Configuration.h
  ${INCDIR}/Execution.h  
  ${INCDIR}/Thread.h
  ${INCDIR}/Signal.h
  ${INCDIR}/Runnable.h
  ${INCDIR}/BlockingQueue.h  
  ${INCDIR}/File.h    
  ${INCDIR}/BufferedFile.h  
  ${INCDIR}/Image.h   
  ${INCDIR}/Md5Hash.h   
  ${INCDIR}/stages/AbstractStage.h
  ${INCDIR}/stages/AbstractConsumerStage.h
  ${INCDIR}/stages/AbstractFilterStage.h
  ${INCDIR}/stages/AbstractProducerStage.h
  ${INCDIR}/stages/InitialElementProducer.h
  ${INCDIR}/stages/CollectorSink.h
  ${INCDIR}/stages/DistributorStage.h
  ${INCDIR}/stages/MergerStage.h
  ${INCDIR}/stages/DelayStage.h
  ${INCDIR}/stages/Directory2Files.h
  ${INCDIR}/stages/File2FileBuffer.h
  ${INCDIR}/stages/ReadImage.h
  ${INCDIR}/stages/ResizeImage.h
  ${INCDIR}/stages/SaveImage.h
  ${INCDIR}/stages/WriteFileBuffer.h
  ${INCDIR}/stages/Md5Hashing.h
  ${INCDIR}/stages/FileExtensionSwitch.h  
  ${INCDIR}/stages/FunctionStage.h  
  ${INCDIR}/stages/RandomIntProducer.h  
  ${INCDIR}/ports/AbstractOutputPort.h
  ${INCDIR}/ports/AbstractInputPort.h
  ${INCDIR}/ports/InputPort.h
  ${INCDIR}/ports/OutputPort.h
  ${INCDIR}/ports/Port.h
  ${INCDIR}/pipes/Pipe.h
  ${INCDIR}/pipes/AbstractPipe.h
  ${INCDIR}/pipes/UnsynchedPipe.h  
  ${INCDIR}/pipes/SynchedPipe.h  
  ${INCDIR}/pipes/ProducerConsumerQueue.h  
  ${INCDIR}/pipes/SpscQueue.h  
)

SET(SOURCES
  logging.cpp    
  Configuration.cpp
  Runnable.cpp
  Thread.cpp
  Image.cpp  
  Md5Hash.cpp
  BufferedFile.cpp
  platform_posix.cpp
  platform_win32.cpp
  ports/AbstractOutputPort.cpp  
  stages/RandomIntProducer.cpp
  stages/Directory2Files.cpp
  stages/File2FileBuffer.cpp
  stages/FileExtensionSwitch.cpp
  stages/Md5Hashing.cpp
  stages/ReadImage.cpp
  stages/ResizeImage.cpp
  stages/SaveImage.cpp
  stages/WriteFileBuffer.cpp
  stages/AbstractStage.cpp  
)

IF(MSVC)
  source_group(private .*)
  source_group(private\\stages stages/.*)
  source_group(private\\ports ports/.*)
  source_group(public .*/include/.*)
  source_group(public\\stages .*/include/.*/stages/.*)
  source_group(public\\ports .*/include/.*/ports/.*)
  source_group(public\\pipes .*/include/.*/pipes/.*)
ENDIF()  

add_library("teetime" ${HEADERS} ${SOURCES})