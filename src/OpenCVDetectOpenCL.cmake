if(APPLE)
  set(OPENCL_FOUND YES)
  set(OPENCL_LIBRARY "-framework OpenCL" CACHE STRING "OpenCL library")
  set(OPENCL_INCLUDE_DIR "" CACHE STRING "OpenCL include directory")
  mark_as_advanced(OPENCL_INCLUDE_DIR OPENCL_LIBRARY)
else(APPLE)
  #find_package(OpenCL QUIET)

  if (NOT OPENCL_FOUND)
    find_path(OPENCL_ROOT_DIR
              NAMES OpenCL/cl.h CL/cl.h include/CL/cl.h include/nvidia-current/CL/cl.h
              PATHS ENV OCLROOT ENV AMDAPPSDKROOT ENV CUDA_PATH ENV INTELOCLSDKROOT
              DOC "OpenCL root directory"
              NO_DEFAULT_PATH)
    message("   OPENCL_ROOT_DIR = ${OPENCL_ROOT_DIR}")

    find_path(OPENCL_INCLUDE_DIR
              NAMES OpenCL/cl.h CL/cl.h
              HINTS ${OPENCL_ROOT_DIR}
              PATH_SUFFIXES include include/nvidia-current
              DOC "OpenCL include directory"
              NO_DEFAULT_PATH)
    message("   OPENCL_INCLUDE_DIR = ${OPENCL_INCLUDE_DIR}")

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(OPENCL_POSSIBLE_LIB_SUFFIXES lib/Win64 lib/x86_64 lib/x64)
    else()
      set(OPENCL_POSSIBLE_LIB_SUFFIXES lib/Win32 lib/x86)
    endif()

    find_library(OPENCL_LIBRARY
              NAMES OpenCL
              HINTS ${OPENCL_ROOT_DIR}
              PATH_SUFFIXES ${OPENCL_POSSIBLE_LIB_SUFFIXES}
              DOC "OpenCL library"
              NO_DEFAULT_PATH)
    message("   OPENCL_LIBRARY = ${OPENCL_LIBRARY}")

    mark_as_advanced(OPENCL_INCLUDE_DIR OPENCL_LIBRARY)
    include(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENCL DEFAULT_MSG OPENCL_LIBRARY OPENCL_INCLUDE_DIR )
  endif()
endif(APPLE)
