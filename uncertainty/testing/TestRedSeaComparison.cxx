#include <iostream>
#include <random>
#include <vtkm/Pair.h>
#include <vtkm/cont/Initialize.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/Algorithm.h>
#include "../worklet/ExtractingMinMaxFromMeanDev.hpp"
#include "../worklet/ComputeDiff.hpp"

#include "../Fiber.h"
#include <vtkm/io/VTKDataSetReader.h>
#include <vtkm/io/VTKDataSetWriter.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/Timer.h>

int main(int argc, char *argv[])
{
    vtkm::cont::InitializeResult initResult = vtkm::cont::Initialize(
        argc, argv, vtkm::cont::InitializeOptions::DefaultAnyDevice);
    std::cout << "initResult.Device: " << initResult.Device.GetName() << std::endl;

    if (argc != 3)
    {
        std::cout << "<executable> <DataFolder> <NumSample>" << std::endl;
        exit(0);
    }

    std::string dataFolder = std::string(argv[1]);
    int NumSamples = std::stoi(argv[2]);

    // compute the min and max through the mean+-stdev for two variables
    std::string CurlField = "curlZ";
    std::string VorField = "vorticityMagnitude";

    // the name of the file is mistyped, the value is actually the curl
    std::string MeanCurlFile = dataFolder + "/curlZ/meanVol/meanVorticity.vtk";
    std::string DevCurlFile = dataFolder + "/curlZ/devVol/devVorticity.vtk";

    std::string MeanVorFile = dataFolder + "/vorticityMagnitude/meanVol/meanVorticity.vtk";
    std::string DevVorFile = dataFolder + "/vorticityMagnitude/devVol/devVorticity.vtk";

    // get mean for the curl
    vtkm::io::VTKDataSetReader MeanCurlReader(MeanCurlFile);
    vtkm::cont::DataSet MeanCurlData = MeanCurlReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> MeanCurlDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(MeanCurlData.GetField("meanVorticity").GetData(), MeanCurlDataArray);

    // get the cellset
    auto cellSet = MeanCurlData.GetCellSet();
    vtkm::cont::CellSetStructured<3> structCellSet =
        cellSet.AsCellSet<vtkm::cont::CellSetStructured<3>>();

    vtkm::Id3 pointDims = structCellSet.GetPointDimensions();
    std::cout << "------" << std::endl;
    std::cout << "point dim: " << pointDims[0] << " " << pointDims[1] << " " << pointDims[2] << std::endl;

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
    vtkm::filter::uncertainty::Fiber filter;
    // curlz -15 -1
    // vorticity 1 15
    // big user specified rectangle need more monte carlo sampling
    vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> minAxisValue(-15.0, 0.0);
    vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> maxAxisValue(-0.1, 20);

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

    // dataSetForFilter.AddPointField("MeanCurlDataArray", MeanCurlDataArray);
    // dataSetForFilter.AddPointField("DevCurlDataArray", DevCurlDataArray);
    // dataSetForFilter.AddPointField("MeanVorDataArray", MeanVorDataArray);
    // dataSetForFilter.AddPointField("DevVorDataArray", DevVorDataArray);

    // call the fiber filter
    filter.SetMinOne("ensemble_min_one");
    filter.SetMaxOne("ensemble_max_one");
    filter.SetMinTwo("ensemble_min_two");
    filter.SetMaxTwo("ensemble_max_two");

    filter.SetApproach("ClosedForm");
    vtkm::cont::Timer timer{initResult.Device};
    std::cout << "timer device: " << timer.GetDevice().GetName() << std::endl;
    timer.Start();
    vtkm::cont::DataSet output = filter.Execute(dataSetForFilter);
    timer.Stop();
    std::cout << "total elapsedTime 1:" << timer.GetElapsedTime() << std::endl;

    timer.Start();
    vtkm::cont::DataSet output1 = filter.Execute(dataSetForFilter);
    timer.Stop();
    std::cout << "total elapsedTime 2:" << timer.GetElapsedTime() << std::endl;

    vtkm::io::VTKDataSetWriter writer("./out_fiber_redsea_uncertainty_closedform_" + initResult.Device.GetName() + ".vtk");
    writer.WriteDataSet(output1);

    filter.SetApproach("MonteCarlo");
    filter.SetNumSamples(NumSamples);

    timer.Start();
    output = filter.Execute(dataSetForFilter);
    timer.Stop();
    std::cout << "total elapsedTime 2:" << timer.GetElapsedTime() << std::endl;

    timer.Start();
    auto output2 = filter.Execute(dataSetForFilter);
    timer.Stop();
    std::cout << "total elapsedTime 2:" << timer.GetElapsedTime() << std::endl;

    vtkm::io::VTKDataSetWriter writerMCarlo("./out_fiber_redsea_uncertainty_montecarlo_" + std::to_string(NumSamples) + "_" + initResult.Device.GetName() + ".vtk");
    writerMCarlo.WriteDataSet(output2);

    // compute the difference between two output

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> diff;

    auto prob1 = output1.GetField("OutputProbability").GetData().AsArrayHandle<vtkm::cont::ArrayHandle<vtkm::FloatDefault>>();
    auto prob2 = output2.GetField("OutputProbability").GetData().AsArrayHandle<vtkm::cont::ArrayHandle<vtkm::FloatDefault>>();

    invoke(ComputeDiff{}, prob1, prob2, diff);
    vtkm::FloatDefault MaxDiff = vtkm::cont::Algorithm::Reduce(diff, 0.0, vtkm::Maximum());

    std::cout << "MaxDiff is for samples " << NumSamples << " is " << MaxDiff << std::endl;
}