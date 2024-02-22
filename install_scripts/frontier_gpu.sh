#!/bin/bash
set -e
set -x

module load rocm
module load cmake
module load git-lfs
module load ninja

build_jobs=8
mkdir -p frontier_gpu
cd frontier_gpu

HERE=`pwd`
source $HERE/../settings.sh

SOFTWARE_SRC_DIR="$HERE/src"
SOFTWARE_BUILD_DIR="$HERE/build"
SOFTWARE_INSTALL_DIR="$HERE/install"

mkdir -p $SOFTWARE_SRC_DIR
mkdir -p $SOFTWARE_BUILD_DIR
mkdir -p $SOFTWARE_INSTALL_DIR

echo "====> Install Kokkos"
kokkos_src_dir=${SOFTWARE_SRC_DIR}/kokkos
kokkos_build_dir=${SOFTWARE_BUILD_DIR}/kokkos
kokkos_install_dir=${SOFTWARE_INSTALL_DIR}/kokkos

if [ -d $kokkos_install_dir ]; then
    echo "====> skip, $kokkos_install_dir already exists," \
             "please remove it if you want to reinstall it"
else
rm -rf ${kokkos_src_dir}
git clone -b master https://github.com/kokkos/kokkos.git ${kokkos_src_dir}
rm -rf ${kokkos_build_dir}
cmake -S ${kokkos_src_dir} -B ${kokkos_build_dir} \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON\
  -DKokkos_ARCH_VEGA90A=ON \
  -DCMAKE_CXX_COMPILER=hipcc \
  -DKokkos_ENABLE_HIP=ON \
  -DKokkos_ENABLE_SERIAL=ON \
  -DKokkos_ENABLE_HIP_RELOCATABLE_DEVICE_CODE=OFF \
  -DCMAKE_INSTALL_PREFIX=${kokkos_install_dir} \
  -DCMAKE_CXX_FLAGS="--amdgpu-target=gfx90a"
cmake --build ${kokkos_build_dir} -j10
cmake --install ${kokkos_build_dir}
fi

echo "====> Installing vtk-m"
VTKM_SRC_DIR="${SOFTWARE_SRC_DIR}/vtk-m"
VTKM_BUILD_DIR="${SOFTWARE_BUILD_DIR}/vtk-m"
VTKM_INSTALL_DIR="${SOFTWARE_INSTALL_DIR}/vtk-m"

    echo $VTKM_SRC_DIR
    echo $VTKM_BUILD_DIR
    echo $VTKM_INSTALL_DIR

# check the install dir
if [ -d $VTKM_SRC_DIR ]; then
    echo "====> skip, $VTKM_SRC_DIR already exists," \
             "please remove it if you want to reinstall it"
else
    echo $VTKM_SRC_DIR
    echo $VTKM_BUILD_DIR
    echo $VTKM_INSTALL_DIR
    # check vktm source dir
    if [ ! -d $VTKM_SRC_DIR ]; then
    # clone the source
    cd $SOFTWARE_SRC_DIR
    git clone $VTKM_MASTER_REPO
    cd $VTKM_SRC_DIR
    git checkout $VTKM_MASTER_VERSION
    fi
fi
    cd $HERE

    # build and install
    echo "**** Building vtk-m"

if [ -d $VTKM_INSTALL_DIR ]; then
    echo "====> skip, $VTKM_INSTALL_DIR already exists," \
             "please remove it if you want to reinstall it"
else

    # putting the -B before the -S may causing some issues sometimes
    mkdir -p ${VTKM_BUILD_DIR}
    rm -rf ${VTKM_BUILD_DIR}/CMakeCache.txt
    cd ${VTKM_BUILD_DIR}    
    cmake -S ${VTKM_SRC_DIR} \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=${VTKM_INSTALL_DIR} \
    -DBUILD_SHARED_LIBS=OFF \
    -DVTKm_USE_DEFAULT_TYPES_FOR_ASCENT=ON \
    -DVTKm_USE_DOUBLE_PRECISION=ON \
    -DVTKm_NO_DEPRECATED_VIRTUAL=ON \
    -DVTKm_USE_64BIT_IDS=OFF \
    -DVTKm_ENABLE_MPI=ON \
    -DVTKm_ENABLE_OPENMP=ON \
    -DVTKm_ENABLE_LOGGING=ON \
    -DVTKm_ENABLE_RENDERING=OFF \
    -DVTKm_ENABLE_KOKKOS=ON \
    -DVTKm_ENABLE_TESTING=OFF \
    -DCMAKE_HIP_ARCHITECTURES=gfx90a \
    -DCMAKE_PREFIX_PATH=${kokkos_install_dir} \
    -DCMAKE_CXX_COMPILER=hipcc -DCMAKE_C_COMPILER=hipcc

    cd ${VTKM_BUILD_DIR}
    make -j${build_jobs}
    make install
fi

echo "====> Installing vtk-m, ok"

echo "vtkm install dir ${VTKM_INSTALL_DIR}"
