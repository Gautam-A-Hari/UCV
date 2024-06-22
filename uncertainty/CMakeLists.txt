##============================================================================
##  Copyright (c) Kitware, Inc.
##  All rights reserved.
##  See LICENSE.txt for details.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##============================================================================

set(uncertainty_headers
  Fiber.h
  #FiberMultiVar.h
  Fiber4Var.h
  MeanField2Var.h
  Fiber2Var.h
  Fiber3Var.h
  )
set(uncertainty_sources_device
  Fiber.cxx
  #FiberMultiVar.cxx
  Fiber4Var.cxx
  MeanField2Var.cxx
  Fiber2Var.cxx
  Fiber3Var.cxx
  )

add_library(filter_uncertainty
  ${uncertainty_sources_device}
  )

target_include_directories(filter_uncertainty
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  )
target_link_libraries(filter_uncertainty
  PUBLIC vtkm::filter_core
  )

vtkm_add_target_information(filter_uncertainty
   DEVICE_SOURCES ${uncertainty_sources_device}
   DROP_UNUSED_SYMBOLS MODIFY_CUDA_FLAGS
  )

add_subdirectory(testing)

if(BUILD_PARAVIEW_PLUGIN)
  # ParaView plugins need this included to be built.
  # (Why don't they just include themselves?)
  include(GNUInstallDirs)

  # ParaView plugins need to be packaged in a shared library
  set(BUILD_SHARED_LIBS ON)

  paraview_plugin_scan(
    PLUGIN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pv-plugin/paraview.plugin
    PROVIDES_PLUGINS plugins
    ENABLE_BY_DEFAULT ON
    HIDE_PLUGINS_FROM_CACHE ON
    )

  # Note: this will call subdirectory for pv-plugin so we don't have to.
  paraview_plugin_build(PLUGINS ${plugins})
endif()
