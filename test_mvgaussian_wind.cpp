
#include <vtkm/io/VTKDataSetWriter.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/worklet/DispatcherReduceByKey.h>
#include <vtkm/worklet/DispatcherMapTopology.h>
#include <vtkm/cont/ArrayHandleSOA.h>
#include <vtkm/cont/Initialize.h>

#include "ucvworklet/CreateNewKey.hpp"
// #include "ucvworklet/MVGaussianWithEnsemble2D.hpp"
#include "ucvworklet/MVGaussianWithEnsemble2DTryLialg.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using SupportedTypesVec = vtkm::List<vtkm::Vec<double, 15>>;

void exampleDataSet(int pointNum, std::vector<std::vector<double>> &data)
{

    int version = 15;

    for (int i = 0; i < version; i++)
    {
        std::vector<double> d;
        for (int j = 0; j < pointNum * pointNum; j++)
        {
            d.push_back((j + 0.1) + (i + 1) * j);
        }
        data.push_back(d);
    }
}

void initBackend()
{
  // init the vtkh device
  char const *tmp = getenv("UCV_VTKM_BACKEND");
  std::string backend;
  if (tmp == nullptr)
  {
    std::cout << "no UCV_VTKM_BACKEND env, use openmp" << std::endl;
    backend = "openmp";
  }
  else
  {
    backend = std::string(tmp);
  }

  //if (rank == 0)
  //{
    std::cout << "vtkm backend is:" << backend << std::endl;
  //}

  if (backend == "serial")
  {
      vtkm::cont::RuntimeDeviceTracker &device_tracker
    = vtkm::cont::GetRuntimeDeviceTracker();
  device_tracker.ForceDevice(vtkm::cont::DeviceAdapterTagSerial());
    
  }
  else if (backend == "openmp")
  {
      vtkm::cont::RuntimeDeviceTracker &device_tracker
    = vtkm::cont::GetRuntimeDeviceTracker();
  device_tracker.ForceDevice(vtkm::cont::DeviceAdapterTagOpenMP());
    
  }
  else if (backend == "cuda")
  {
      vtkm::cont::RuntimeDeviceTracker &device_tracker
    = vtkm::cont::GetRuntimeDeviceTracker();
  device_tracker.ForceDevice(vtkm::cont::DeviceAdapterTagCuda());
    
  }
  else
  {
    std::cerr << " unrecognized backend " << backend << std::endl;
  }
  return;
}

int main(int argc, char *argv[])
{
    vtkm::cont::Initialize(argc, argv);

    // set backend
    initBackend();

    // assuming the ensemble data set is already been extracted out
    // we test results by this dataset
    // https://github.com/MengjiaoH/Probabilistic-Marching-Cubes-C-/tree/main/datasets/txt_files/wind_pressure_200
    // to make sure the reuslts are correct

    // load data set, the dim is m*n and for each point there are k ensemble values
    // the data set comes from here https://github.com/MengjiaoH/Probabilistic-Marching-Cubes-C-/tree/main/datasets/txt_files/wind_pressure_200

    // vtkm::Id xdim = 240;
    // vtkm::Id ydim = 121;

    vtkm::Id gxdim = 240;
    vtkm::Id gydim = 121;
    
    //there are some memory issue on cuda if larger than 150*120
    //vtkm::Id xdim = 2;
    //vtkm::Id ydim = 2;
    vtkm::Id xdim = 240;
    vtkm::Id ydim = 121;
    //vtkm::Id zdim = 1;
     
     //there are cuda errors start with the xdim=40 ydim =40
     //vtkm::Id xdim = 40;
     //vtkm::Id ydim = 40;
     vtkm::Id zdim = 1;

    const vtkm::Id3 dims(xdim, ydim, zdim);
    vtkm::cont::DataSetBuilderUniform dataSetBuilder;

    vtkm::cont::DataSet vtkmDataSet = dataSetBuilder.Create(dims);

    std::string windDataDir = "./wind_pressure_200/";
     double isovalue = 0.2;
    //double isovalue = 1.5;
    // 15 files each contains all data in one file
    // std::vector<vtkm::cont::ArrayHandle<vtkm::Float64>> componentArrays;
    std::vector<std::vector<vtkm::Float64>> dataArray;

    using Vec15 = vtkm::Vec<double, 15>;

    vtkm::cont::ArrayHandle<Vec15> dataArraySOA;

    dataArraySOA.Allocate(xdim * ydim);

    for (vtkm::IdComponent i = 0; i < 15; i++)
    {
        std::string filename = windDataDir + "Lead_33_" + std::to_string(i) + ".txt";

        // load the data
        std::ifstream fin(filename.c_str());

        std::vector<double> d;
        float element;
        // put data into one vector
        while (fin >> element)
        {
            // put whole picture into the vector
            d.push_back(element);
        }

        // ArrayHandleSOA will group the components in each array and
        // store them into Vec15 automatically.
        // transfer vector into the array handle
        // componentArrays.push_back(vtkm::cont::make_ArrayHandleMove(std::move(d)));
        dataArray.push_back(d);
    }

    // for synthetic dataset
    // exampleDataSet(xdim,dataArray);

    // change aos array to soa array
    // for each points, there are 15 version
    int index = 0;
    
    //for (vtkm::IdComponent i = 0; i < gxdim * gydim; i++)
    for (vtkm::IdComponent i = 0; i < xdim * ydim; i++)
    {
        Vec15 ensemble;
        //if (i == 0 || i == 1 || i == 240 || i == 241)
        {
            // only insert for specific one for testing
            for (vtkm::IdComponent j = 0; j < 15; j++)
            {
                ensemble[j] = dataArray[j][i];
            }
            dataArraySOA.WritePortal().Set(index, ensemble);
            index++;
        }
    }

    vtkmDataSet.AddPointField("ensembles", dataArraySOA);

    std::cout << "checking input dataset" << std::endl;
    vtkmDataSet.PrintSummary(std::cout);

    // check results

    // std::string outputFileNameOriginal = "./wind_pressure_200_original.vtk";
    // vtkm::io::VTKDataSetWriter write(outputFileNameOriginal);
    // write.WriteDataSet(vtkmDataSet);

    // let the data set go through the multivariant gaussian filter
    // using WorkletType = MVGaussianWithEnsemble2D;
    using WorkletType = MVGaussianWithEnsemble2DTryLialg;
    using DispatcherType = vtkm::worklet::DispatcherMapTopology<WorkletType>;

    vtkm::cont::ArrayHandle<vtkm::Float64> crossProbability;

    auto resolveType = [&](const auto &concrete)
    {
        // DispatcherType dispatcher(MVGaussianWithEnsemble2D{isovalue});

        DispatcherType dispatcher(MVGaussianWithEnsemble2DTryLialg{isovalue});

        dispatcher.Invoke(vtkmDataSet.GetCellSet(), concrete, crossProbability);
    };

    vtkmDataSet.GetField("ensembles").GetData().CastAndCallForTypes<SupportedTypesVec, VTKM_DEFAULT_STORAGE_LIST>(resolveType);

    // check results
    vtkmDataSet.AddCellField("cross_prob", crossProbability);
    std::string outputFileName = "./wind_pressure_200.vtk";
    vtkm::io::VTKDataSetWriter writeCross(outputFileName);
    writeCross.WriteDataSet(vtkmDataSet);
}