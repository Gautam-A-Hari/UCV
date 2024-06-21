#include <iostream>
#include <random>
#include <vtkm/Pair.h>
#include <vtkm/cont/Initialize.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/Algorithm.h>
#include "../worklet/ExtractingMinMaxFromMeanDev.hpp"
//#include <vtkm/io/VTKDataSetWriter.h>


#include "../Fiber4Var.h"
//#include <vtkm/filter/uncertainty/FiberMultiVar.h>
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
    //int NumEns = 20;

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



    //3rd variable 
    std::string MeanVelFile = dataFolder + "/velocityMagnitude/meanVol/meanVelocityMagnitude.vtk";
    std::string DevVelFile = dataFolder + "/velocityMagnitude/devVol/devVelocityMagnitude.vtk";

    //4rd variable
    std::string MeanTempFile = dataFolder + "/temperature/meanVol/meanTemperature.vtk";
    std::string DevTempFile = dataFolder + "/temperature/devVol/devTemperature.vtk";

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
    

    // get mean for the velocity
    vtkm::io::VTKDataSetReader MeanVelReader(MeanVelFile);
    vtkm::cont::DataSet MeanVelData = MeanVelReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> MeanVelDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(MeanVelData.GetField("meanVelocityMagnitude").GetData(), MeanVelDataArray);

    // get dev for the velocity
    vtkm::io::VTKDataSetReader DevVelReader(DevVelFile);
    vtkm::cont::DataSet DevVelData = DevVelReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> DevVelDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(DevVelData.GetField("devVelocityMagnitude").GetData(), DevVelDataArray);


    // get mean for the temperature
    vtkm::io::VTKDataSetReader MeanTempReader(MeanTempFile);
    vtkm::cont::DataSet MeanTempData = MeanTempReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> MeanTempDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(MeanTempData.GetField("meanTemperature").GetData(), MeanTempDataArray);

    // get dev for the temperature
    vtkm::io::VTKDataSetReader DevTempReader(DevTempFile);
    vtkm::cont::DataSet DevTempData = DevTempReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> DevTempDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(DevTempData.GetField("devTemperature").GetData(), DevTempDataArray);


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


    // compute the min and max for the velocity
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> minField3;
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> maxField3;
    invoke(ExtractingMinMaxFromMeanDev{}, MeanVelDataArray, DevVelDataArray, minField3, maxField3);

    // compute the min and max for the temperature
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> minField4;
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> maxField4;
    invoke(ExtractingMinMaxFromMeanDev{}, MeanTempDataArray, DevTempDataArray, minField4, maxField4);


    // user specify the field

    //TODO change to 3 variables
    vtkm::filter::uncertainty::Fiber4Var filter;
    // curlz -15 -1
    // vorticity 1 15
    // big user specified rectangle need more monte carlo sampling
    vtkm::Vec<vtkm::Float64, 4> minAxis(-15.0, 0.0, 0.0, 10.0);

    //old 
    //vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> topRight(-0.1, 20);
    //new value matching paper
    vtkm::Vec<vtkm::Float64, 4> maxAxis(-0.1, 15, 0.4, 29);

    // vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> bottomLeft(-5.0, 0.0);
    // vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> topRight(5.0, 6.0);

    filter.SetMinAxis(minAxis);
    filter.SetMaxAxis(maxAxis);


    // create the data based on min and max array
    //do I change the pointDims to 4?
    const vtkm::Vec<vtkm::Id, 3> dims(pointDims[0], pointDims[1], pointDims[2]);
    vtkm::cont::DataSetBuilderUniform dataSetBuilder;
    vtkm::cont::DataSet dataSetForFilter = dataSetBuilder.Create(dims);

    // TODO, update spacing based on the original dataset

    dataSetForFilter.AddPointField("ensemble_min_one", minField1);
    dataSetForFilter.AddPointField("ensemble_max_one", maxField1);

    dataSetForFilter.AddPointField("ensemble_min_two", minField2);
    dataSetForFilter.AddPointField("ensemble_max_two", maxField2);

    dataSetForFilter.AddPointField("ensemble_min_three", minField3);
    dataSetForFilter.AddPointField("ensemble_max_three", maxField3);

    dataSetForFilter.AddPointField("ensemble_min_four", minField4);
    dataSetForFilter.AddPointField("ensemble_max_four", maxField4);



    //dataSetForFilter.AddPointField("MeanCurlDataArray", MeanCurlDataArray);
    //dataSetForFilter.AddPointField("DevCurlDataArray", DevCurlDataArray);
    //dataSetForFilter.AddPointField("MeanVorDataArray", MeanVorDataArray);
    //dataSetForFilter.AddPointField("DevVorDataArray", DevVorDataArray);
    //dataSetForFilter.AddPointField("MeanVelDataArray", MeanVelDataArray);
    //dataSetForFilter.AddPointField("DevVelDataArray", DevVelDataArray);
    //dataSetForFilter.AddPointField("MeanTempDataArray", MeanTempDataArray);
    //dataSetForFilter.AddPointField("DevTempDataArray", DevTempDataArray);

    // call the fiber filter
    filter.SetMinX("ensemble_min_one");
    filter.SetMaxX("ensemble_max_one");
    filter.SetMinY("ensemble_min_two");
    filter.SetMaxY("ensemble_max_two");
    filter.SetMinZ("ensemble_min_three");
    filter.SetMaxZ("ensemble_max_three");
    filter.SetMinW("ensemble_min_four");
    filter.SetMaxW("ensemble_max_four");

    
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