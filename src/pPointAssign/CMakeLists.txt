#--------------------------------------------------------
# The CMakeLists.txt for:                       pPointAssign
# Author(s):                              Simen Oevereng
#--------------------------------------------------------

SET(SRC
  Geometry.cpp
  PointAssign.cpp
  PointAssign_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pPointAssign ${SRC})

TARGET_LINK_LIBRARIES(pPointAssign
   ${MOOS_LIBRARIES}
   geometry
   mbutil
   m
   pthread)

