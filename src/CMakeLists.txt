SET(prj_name old_perf_ocl)
PROJECT(${prj_name})
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# fix Visual Studio defaults
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  if(NOT OpenCV_SHARED)
    foreach(flag_var
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif()
      if(${flag_var} MATCHES "/MDd")
        string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
      endif()
    endforeach(flag_var)

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:atlthunk.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcrtd.lib")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libcmt.lib")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmtd.lib")
  endif()
endif()

file(GLOB src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
ADD_EXECUTABLE(${prj_name}  ${src})

FIND_PACKAGE(OpenCV REQUIRED)
TARGET_LINK_LIBRARIES(${prj_name} ${OpenCV_LIBS})

if(OpenCV_VERSION VERSION_EQUAL 2.4.6)
  message("[OpenCV-2.4.6 fix]: adding opencv_ocl.lib and OpenCL.lib to the project")
  TARGET_LINK_LIBRARIES(${prj_name} debug "${OpenCV_LIB_PATH}/opencv_ocl246d.lib" optimized "${OpenCV_LIB_PATH}/opencv_ocl246.lib")
  include(OpenCVDetectOpenCL.cmake)
  if(OPENCL_FOUND)
    TARGET_LINK_LIBRARIES(${prj_name} ${OPENCL_LIBRARY})
  else()
    message("Cannot find OpenCL.lib, please configure your project manually")
  endif()

endif()
