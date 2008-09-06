#Change PROJECT_NAME to the name of your project
PROJECT(skeletonUtils)

# set the name of the input image used to run the test
SET(INPUT_IMAGE ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1.png)
SET(INPUT_IMAGE3D ${CMAKE_CURRENT_SOURCE_DIR}/images/bunnyPadded.nrrd)

SET(INPUT_SKEL ${CMAKE_CURRENT_SOURCE_DIR}/images/ctskel.tif)
SET(INPUT_LINE ${CMAKE_CURRENT_SOURCE_DIR}/images/diag.tif)

#include some macros from another file...
INCLUDE(IJMacros.txt)

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)
FIND_PROGRAM(ITK_TEST_DRIVER itkTestDriver)
SET(TEST_COMMAND ${ITK_TEST_DRIVER} --add-before-env PATH ${CMAKE_CURRENT_BINARY_DIR})

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
  ITK
)

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
  ITKCommon
  ITKBasicFilters
  ITKIO
)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# install devel files
OPTION(INSTALL_DEVEL_FILES "Install C++ headers" ON)
IF(INSTALL_DEVEL_FILES)
FILE(GLOB develFiles *.h *.txx) 
FOREACH(f ${develFiles})
  INSTALL_FILES(/include/InsightToolkit/BasicFilters FILES ${f})
ENDFOREACH(f)
ENDIF(INSTALL_DEVEL_FILES)



# option for wrapping
OPTION(BUILD_WRAPPERS "Wrap library" OFF)
IF(BUILD_WRAPPERS)
   SUBDIRS(Wrapping)
ENDIF(BUILD_WRAPPERS)
   
   

#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project

IF(BUILD_TESTING)

FOREACH(CurrentExe "skelTest" "skelTest2" "skelTest3d" "demoError" "basicPrune" "fastPrune" "prunePerformance")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})
ENDFOREACH(CurrentExe)

ENDIF(BUILD_TESTING)

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.
# ADD_TEST(Testname ExecutableToRun arg1 arg2 arg3)

ADD_TEST(skelTest ${TEST_COMMAND}
		  skelTest ${INPUT_IMAGE} skel1.png
)

ADD_TEST(skelTest2 ${TEST_COMMAND}
		  skelTest2 ${INPUT_IMAGE} skel2.png
)

ADD_TEST(skelTest3d ${TEST_COMMAND}
		  skelTest3d ${INPUT_IMAGE3D} skel3.nrrd
)

ADD_TEST(basicPrune ${TEST_COMMAND}
   basicPrune ${INPUT_SKEL} basicskel_1.png
   --compare basicskel_1.png ${CMAKE_CURRENT_SOURCE_DIR}/images/basicskel_1.png
)

ADD_TEST(fastPrune2 ${TEST_COMMAND}
   fastPrune 2 ${INPUT_SKEL} fastskel_2.png basicskel_2.png 
   --compare fastskel_2.png basicskel_2.png
)

ADD_TEST(fastPrune10 ${TEST_COMMAND}
   fastPrune 10 ${INPUT_SKEL} fastskel_10.png basicskel_10.png 
   --compare fastskel_10.png basicskel_10.png
)
ADD_TEST(fastPrune20 ${TEST_COMMAND}
   fastPrune 20 ${INPUT_SKEL} fastskel_20.png basicskel_20.png
   --compare fastskel_20.png basicskel_20.png
)

ADD_TEST(demoError ${TEST_COMMAND}
   demoError ${INPUT_LINE} failed.png
)
