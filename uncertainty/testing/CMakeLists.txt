##============================================================================
##  Copyright (c) Kitware, Inc.
##  All rights reserved.
##  See LICENSE.txt for details.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##============================================================================

OPTION (USE_HIP OFF)

if(USE_HIP)

set_source_files_properties(TestFiber.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestFiber TestFiber.cxx ../Fiber.cxx)
target_link_libraries(TestFiber ${VTKm_LIBRARIES} MPI::MPI_CXX)

set_source_files_properties(TestSuperNova.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestSuperNova TestSuperNova.cxx ../Fiber.cxx)
target_link_libraries(TestSuperNova ${VTKm_LIBRARIES} MPI::MPI_CXX)

set_source_files_properties(TestSuperNovaComparison.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestSuperNovaComparison TestSuperNovaComparison.cxx ../Fiber.cxx)
target_link_libraries(TestSuperNovaComparison ${VTKm_LIBRARIES} MPI::MPI_CXX)

set_source_files_properties(TestRedSea.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestRedSea TestRedSea.cxx ../Fiber.cxx)
target_link_libraries(TestRedSea ${VTKm_LIBRARIES} MPI::MPI_CXX)

set_source_files_properties(TestRedSeaComparison.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestRedSeaComparison TestRedSeaComparison.cxx ../Fiber.cxx)
target_link_libraries(TestRedSeaComparison ${VTKm_LIBRARIES} MPI::MPI_CXX)


#set_source_files_properties(TestRedSea2Var.cxx PROPERTIES LANGUAGE "HIP")
#add_executable(TestRedSea2Var TestRedSea2Var.cxx ../Fiber2Var.cxx)
#target_link_libraries(TestRedSea ${VTKm_LIBRARIES} MPI::MPI_CXX)

set_source_files_properties(TestRedSea3Var.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestRedSea3Var TestRedSea3Var.cxx ../Fiber3Var.cxx)
target_link_libraries(TestRedSea3Var ${VTKm_LIBRARIES} MPI::MPI_CXX)

set_source_files_properties(TestRedSea4Var.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestRedSea4Var TestRedSea4Var.cxx ../Fiber4Var.cxx)
target_link_libraries(TestRedSea4Var ${VTKm_LIBRARIES} MPI::MPI_CXX)

set_source_files_properties(TestAteroidImpact2Var.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestAsteroidImpact2Var TestAsteroidImpact2Var.cxx ../Fiber2Var.cxx)
target_link_libraries(TestAsteroidImpact2Var ${VTKm_LIBRARIES} MPI::MPI_CXX)

set_source_files_properties(TestAsteroidImpact3Var.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestAsteroidImpact3Var TestAsteroidImpact3Var.cxx ../Fiber3Var.cxx)
target_link_libraries(TestAsteroidImpact3Var ${VTKm_LIBRARIES} MPI::MPI_CXX)

set_source_files_properties(TestAsteroidImpact4Var.cxx PROPERTIES LANGUAGE "HIP")
add_executable(TestAsteroidImpact4Var TestAsteroidImpact4Var.cxx ../Fiber4Var.cxx)
target_link_libraries(TestAsteroidImpact4Var ${VTKm_LIBRARIES} MPI::MPI_CXX)




elseif(USE_CUDA)

message("---use cuda build---")

set_source_files_properties(../Fiber.h PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../worklet/Fiber.h PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../worklet/ExtractingByNeigoborhoodEnsTwoFields.hpp PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../worklet/ExtractingMinMax.hpp PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(TestSuperNova.cxx PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(TestRedSea.cxx PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../Fiber.cxx PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(temp.cxx PROPERTIES LANGUAGE "CUDA")

#two var

set_source_files_properties(../Fiber2Var.h PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../worklet/Fiber2Var.h PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../Fiber2Var.cxx PROPERTIES LANGUAGE "CUDA")
#set_source_files_properties(TestRedSea2Var.cxx PROPERTIES LANGUAGE "CUDA")
#set_source_files_properties(TestAsteroidImpact2Var.cxx PROPERTIES LANGUAGE "CUDA")

#three var

set_source_files_properties(../Fiber3Var.h PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../worklet/Fiber3Var.h PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../Fiber3Var.cxx PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(TestRedSea3Var.cxx PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(TestAsteroidImpact3Var.cxx PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(TestAsteroidImpact3VarTruth.cxx PROPERTIES LANGUAGE "CUDA")

#four var

set_source_files_properties(../Fiber4Var.h PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../worklet/Fiber4Var.h PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(../Fiber4Var.cxx PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(TestRedSea4Var.cxx PROPERTIES LANGUAGE "CUDA")
set_source_files_properties(TestAsteroidImpact4Var.cxx PROPERTIES LANGUAGE "CUDA")



add_library(fiber ../Fiber.cxx)
target_link_libraries(fiber ${VTKm_LIBRARIES})
set_target_properties(fiber PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

add_executable(TestSuperNova TestSuperNova.cxx)
target_link_libraries(TestSuperNova fiber)
set_target_properties(TestSuperNova PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
# DKokkos_ENABLE_CUDA_RELOCATABLE_DEVICE_CODE=ON can fix multiple defination issue

add_executable(TestRedSea TestRedSea.cxx)
target_link_libraries(TestRedSea fiber)
set_target_properties(TestRedSea PROPERTIES CUDA_SEPARABLE_COMPILATION ON)



add_executable(temp temp.cxx)
target_link_libraries(temp ${VTKm_LIBRARIES})
set_target_properties(temp PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

else()

add_executable(TestFiber TestFiber.cxx ../Fiber.cxx)
target_link_libraries(TestFiber ${VTKm_LIBRARIES})

add_executable(TestSuperNova TestSuperNova.cxx ../Fiber.cxx)
target_link_libraries(TestSuperNova ${VTKm_LIBRARIES})

add_executable(TestSuperNovaComparison TestSuperNovaComparison.cxx ../Fiber.cxx)
target_link_libraries(TestSuperNovaComparison ${VTKm_LIBRARIES})

add_executable(FiberSurfaceExample FiberSurfaceExample.cxx ../Fiber.cxx)
target_link_libraries(FiberSurfaceExample ${VTKm_LIBRARIES})

add_executable(DecomposeSuperNovaToEns DecomposeSuperNovaToEns.cxx)
target_link_libraries(DecomposeSuperNovaToEns ${VTKm_LIBRARIES})

add_executable(TestRedSea TestRedSea.cxx ../Fiber.cxx)
target_link_libraries(TestRedSea ${VTKm_LIBRARIES})

add_executable(TestRedSea3Var TestRedSea3Var.cxx ../Fiber3Var.cxx)
target_link_libraries(TestRedSea3Var ${VTKm_LIBRARIES})

add_executable(TestRedSea4Var TestRedSea4Var.cxx ../Fiber4Var.cxx)
target_link_libraries(TestRedSea4Var ${VTKm_LIBRARIES})

add_executable(TestRedSeaMean TestRedSeaMean.cxx ../MeanField2Var.cxx)
target_link_libraries(TestRedSeaMean ${VTKm_LIBRARIES})

add_executable(TestRedSeaComparison TestRedSeaComparison.cxx ../Fiber.cxx)
target_link_libraries(TestRedSeaComparison ${VTKm_LIBRARIES})

add_executable(TestRedSea2Var TestRedSea2Var.cxx ../Fiber2Var.cxx)
target_link_libraries(TestRedSea2Var ${VTKm_LIBRARIES})

add_executable(TestAsteroidImpact2Var TestAsteroidImpact2Var.cxx ../Fiber2Var.cxx)
target_link_libraries(TestAsteroidImpact2Var ${VTKm_LIBRARIES})

add_executable(TestAsteroidImpact3Var TestAsteroidImpact3Var.cxx ../Fiber3Var.cxx)
target_link_libraries(TestAsteroidImpact3Var ${VTKm_LIBRARIES})


add_executable(TestAsteroidImpact3VarTruth TestAsteroidImpact3VarTruth.cxx ../Fiber3Var.cxx)
target_link_libraries(TestAsteroidImpact3VarTruth ${VTKm_LIBRARIES})

add_executable(TestAsteroidImpact4Var TestAsteroidImpact4Var.cxx ../Fiber4Var.cxx)
target_link_libraries(TestAsteroidImpact4Var ${VTKm_LIBRARIES})

add_executable(temp temp.cxx)
target_link_libraries(temp ${VTKm_LIBRARIES})

endif()
