#--------------------------------------------------------
# The CMakeLists.txt for:                       pGenPath
# Author(s):                              Simen Oevereng
#--------------------------------------------------------

SET(SRC
  Geometry.cpp
  GenPath.cpp
  GenPath_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pGenPath ${SRC})

TARGET_LINK_LIBRARIES(pGenPath
   ${MOOS_LIBRARIES}
   geometry
   apputil
   mbutil
   m
   pthread)

