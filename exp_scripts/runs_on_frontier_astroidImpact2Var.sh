#!/bin/bash
#SBATCH -A csc143
#SBATCH -J FiberUncertainty
#SBATCH -o %x-%j.out
#SBATCH -t 00:30:00
#SBATCH -p batch
#SBATCH -N 1


TestNum=$1
DATADIR=/lustre/orion/scratch/athawaletm/csc143/VisPerfData/uncertainty/
RUNDIR=/lustre/orion/scratch/athawaletm/csc143/FiberUncertainty_RedSea_Tushar
CURRDIR=$(pwd)


mkdir -p $RUNDIR


cd $RUNDIR


rm TestRedSea2Var
#rm TestRedSeaComparison
ln -s $CURRDIR/../install_scripts/frontier_gpu/install/UCV/uncertainty/testing/TestAsteroidImpact2Var TestAsteroidImpact2Var
#ln -s $CURRDIR/../install_scripts/frontier_gpu/install/UCV/uncertainty/testing/TestRedSea4Var TestRedSea4Var
#ln -s $CURRDIR/../install_scripts/frontier_gpu/install/UCV/uncertainty/testing/TestRedSeaComparison TestRedSeaComparison


export OMP_NUM_THREADS=1


# serial
./TestAsteroidImpact3Var --vtkm-device serial $DATADIR/dataForGautam ClosedForm 5000 &> asteroidImpact3Var_cf_serial.log


#./TestRedSea4Var --vtkm-device serial $DATADIR/dataForGautam ClosedForm 5000 &> redsea_cf_serial.log


#./TestRedSea --vtkm-device serial $DATADIR/redsea MonteCarlo 5000 &> redsea_mc_serial.log



# kokkos
./TestAsteroidImpact3Var --vtkm-device kokkos $DATADIR/dataForGautam ClosedForm 5000 &> asteroidImpact3Var_cf_kokkos.log


#./TestRedSea4Var --vtkm-device kokkos $DATADIR/dataForGautam ClosedForm 5000 &> redsea_cf_kokkos.log


#./TestRedSea --vtkm-device kokkos $DATADIR/redsea ClosedForm 5000 &> redsea_cf_kokkos.log


#./TestRedSea --vtkm-device kokkos $DATADIR/redsea MonteCarlo 5000 &> redsea_mc_kokkos.log



# openmp
export OMP_NUM_THREADS=64


./TestAsteroidImpact3Var --vtkm-device openmp $DATADIR/dataForGautam ClosedForm 5000 &> asteroidImpact3Var_cf_openmp.log


#./TestRedSea4Var --vtkm-device openmp $DATADIR/dataForGautam ClosedForm 5000 &> redsea_cf_openmp.log


#./TestRedSea --vtkm-device openmp $DATADIR/redsea MonteCarlo 5000 &> redsea_mc_openmp.log



# Testing comparison


#./TestRedSeaComparison --vtkm-device kokkos $DATADIR/redsea 1000 &> redsea_comp_samples_1000.log


#./TestRedSeaComparison --vtkm-device kokkos $DATADIR/redsea 2000 &> redsea_comp_samples_2000.log


#./TestRedSeaComparison --vtkm-device kokkos $DATADIR/redsea 3000 &> redsea_comp_samples_3000.log


#./TestRedSeaComparison --vtkm-device kokkos $DATADIR/redsea 4000 &> redsea_comp_samples_4000.log


#./TestRedSeaComparison --vtkm-device kokkos $DATADIR/redsea 5000 &> redsea_comp_samples_5000.log


#./TestRedSeaComparison --vtkm-device serial $DATADIR/redsea 5000 &> redsea_comp_samples_serial_5000.log