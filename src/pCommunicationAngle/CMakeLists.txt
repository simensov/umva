#--------------------------------------------------------
# The CMakeLists.txt for:                       pCommunicationAngle
# Author(s):                              Simen Sem Oevereng
#--------------------------------------------------------

SET(SRC
  CommunicationAngle.cpp
  CommunicationAngle_Info.cpp
  # RayEntry.cpp
  Geometry.cpp
  main.cpp
)

ADD_EXECUTABLE(pCommunicationAngle ${SRC})

TARGET_LINK_LIBRARIES(pCommunicationAngle
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

