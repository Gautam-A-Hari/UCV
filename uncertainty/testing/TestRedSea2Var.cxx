#include <iostream>
#include <random>
#include <vtkm/Pair.h>
#include <vtkm/cont/Initialize.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/Algorithm.h>
#include "../worklet/ExtractingMinMaxFromMeanDev.hpp"
//#include <vtkm/io/VTKDataSetWriter.h>


#include "../Fiber2Var.h"
#include <vtkm/io/VTKDataSetReader.h>
#include <vtkm/io/VTKDataSetWriter.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/Timer.h>

int main(int argc, char *argv[])
{
    vtkm::cont::InitializeResult initResult = vtkm::cont::Initialize(
        argc, argv, vtkm::cont::InitializeOptions::DefaultAnyDevice);
    std::cout << "initResult.Device: " << initResult.Device.GetName() << std::endl;

    if (argc != 4)
    {
        std::cout << "<executable> <DataFolder> <Approach> <NumSamples>" << std::endl;
        exit(0);
    }

    std::string dataFolder = std::string(argv[1]);


    std::string Approach = std::string(argv[2]);
    int NumSamples = std::stoi(argv[3]); // this only work when appraoch is MonteCarlo

    if (Approach == "MonteCarlo" || Approach == "ClosedForm" || Approach == "Mean")
    {
    }
    else
    {
        std::cout << "Approach should be MonteCarlo or ClosedFrom" << std::endl;
        exit(0);
    }

    // compute the min and max through the mean+-stdev for two variables
    std::string CurlField = "curlZ";
    std::string VorField = "vorticityMagnitude";

    // the name of the file is mistyped, the value is actually the curl
    std::string MeanCurlFile = dataFolder + "/curlZ/meanVol/meanCurl.vtk";
    std::string DevCurlFile = dataFolder + "/curlZ/devVol/devCurl.vtk";

    std::string MeanVorFile = dataFolder + "/vorticityMagnitude/meanVol/meanVorticity.vtk";
    std::string DevVorFile = dataFolder + "/vorticityMagnitude/devVol/devVorticity.vtk";

    
    vtkm::io::VTKDataSetReader MeanCurlReader(MeanCurlFile);
    vtkm::cont::DataSet MeanCurlData = MeanCurlReader.ReadDataSet();
    // get the cellset
    auto cellSet = MeanCurlData.GetCellSet();
    vtkm::cont::CellSetStructured<3> structCellSet =
        cellSet.AsCellSet<vtkm::cont::CellSetStructured<3>>();

    vtkm::Id3 pointDims = structCellSet.GetPointDimensions();
    std::cout << "------" << std::endl;
    std::cout << "point dim: " << pointDims[0] << " " << pointDims[1] << " " << pointDims[2] << std::endl;

    // get mean for the curl


    vtkm::cont::ArrayHandle<vtkm::FloatDefault> MeanCurlDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(MeanCurlData.GetField("meanVorticity").GetData(), MeanCurlDataArray);

    // get dev for the curl
    vtkm::io::VTKDataSetReader DevCurlReader(DevCurlFile);
    vtkm::cont::DataSet DevCurlData = DevCurlReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> DevCurlDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(DevCurlData.GetField("devVorticity").GetData(), DevCurlDataArray);

    // get mean for the vorticity
    vtkm::io::VTKDataSetReader MeanVorReader(MeanVorFile);
    vtkm::cont::DataSet MeanVorData = MeanVorReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> MeanVorDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(MeanVorData.GetField("meanVorticity").GetData(), MeanVorDataArray);

    // get dev for the vorticity
    vtkm::io::VTKDataSetReader DevVorReader(DevVorFile);
    vtkm::cont::DataSet DevVorData = DevVorReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> DevVorDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(DevVorData.GetField("devVorticity").GetData(), DevVorDataArray);

    // print summary
    // vtkm::cont::printSummary_ArrayHandle(MeanCurlDataArray, std::cout);
    // vtkm::cont::printSummary_ArrayHandle(DevCurlDataArray, std::cout);
    // vtkm::cont::printSummary_ArrayHandle(MeanVorDataArray, std::cout);
    // vtkm::cont::printSummary_ArrayHandle(DevVorDataArray, std::cout);

    // compute the min and max for the curl
    vtkm::cont::Invoker invoke;

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> minField1;
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> maxField1;

    invoke(ExtractingMinMaxFromMeanDev{}, MeanCurlDataArray, DevCurlDataArray, minField1, maxField1);

    // compute the min and max for the vorticity

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> minField2;
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> maxField2;
    invoke(ExtractingMinMaxFromMeanDev{}, MeanVorDataArray, DevVorDataArray, minField2, maxField2);

    // user specify the field
    vtkm::filter::uncertainty::FiberMean filter;
    // curlz -15 -1
    // vorticity 1 15
    // big user specified rectangle need more monte carlo sampling
    vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> minAxisValue(-15.0, 0.6);

    //old 
    //vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> maxAxisValue(-0.1, 20);
    //new value matching paper
    vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> maxAxisValue(-0.3, 15);

    // vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> minAxisValue(-5.0, 0.0);
    // vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> maxAxisValue(5.0, 6.0);

    filter.SetMinAxis(minAxisValue);
    filter.SetMaxAxis(maxAxisValue);

    // create the data based on min and max array
    const vtkm::Id3 dims(pointDims[0], pointDims[1], pointDims[2]);
    vtkm::cont::DataSetBuilderUniform dataSetBuilder;
    vtkm::cont::DataSet dataSetForFilter = dataSetBuilder.Create(dims);

    // TODO, update spacing based on the original dataset

    dataSetForFilter.AddPointField("ensemble_min_one", minField1);
    dataSetForFilter.AddPointField("ensemble_max_one", maxField1);

    dataSetForFilter.AddPointField("ensemble_min_two", minField2);
    dataSetForFilter.AddPointField("ensemble_max_two", maxField2);

    dataSetForFilter.AddPointField("MeanCurlDataArray", MeanCurlDataArray);
    dataSetForFilter.AddPointField("DevCurlDataArray", DevCurlDataArray);
    dataSetForFilter.AddPointField("MeanVorDataArray", MeanVorDataArray);
    dataSetForFilter.AddPointField("DevVorDataArray", DevVorDataArray);

    // call the fiber filter
    filter.SetMinX("ensemble_min_one");
    filter.SetMaxX("ensemble_max_one");
    filter.SetMinY("ensemble_min_two");
    filter.SetMaxY("ensemble_max_two");

    filter.SetApproach(Approach);
    if (Approach == "MonteCarlo")
    {
        filter.SetNumSamples(NumSamples);
    }

    vtkm::cont::Timer timer{initResult.Device};
    std::cout << "timer device: " << timer.GetDevice().GetName() << std::endl;

    for (int i = 1; i <= 5; i++)
    {        
        std::cout << "------" << std::endl;
        std::cout << std::to_string(i) << "th run" << std::endl;
        timer.Start();
        vtkm::cont::DataSet output = filter.Execute(dataSetForFilter);

        timer.Synchronize();
        timer.Stop();
        vtkm::Float64 elapsedTime = timer.GetElapsedTime();
        std::cout << "total elapsedTime:" << elapsedTime << std::endl;
    }
}