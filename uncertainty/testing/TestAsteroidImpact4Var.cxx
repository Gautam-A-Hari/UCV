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
    //std::string CurlField = "curlZ";
    //std::string VorField = "vorticityMagnitude";

    // the name of the file is mistyped, the value is actually the curl
    std::string meanWaterFractionFile = dataFolder + "/meanWaterFraction.vtk";
    std::string devWaterFraction = dataFolder + "/devWaterFraction.vtk";

    std::string meanTemp = dataFolder + "/meanTemperature.vtk";
    std::string devTemp = dataFolder + "/devTemperature.vtk";

    std::string meanPressure = dataFolder + "/meanPressure.vtk";
    std::string devPressure = dataFolder + "/devPressure.vtk";

    std::string meanAsteroidFraction = dataFolder + "/meanAsteroidFraction.vtk";
    std::string devAsteroidFraction = dataFolder + "/devAsteroidFraction.vtk";




    
    vtkm::io::VTKDataSetReader meanWaterFractionReader(meanWaterFractionFile);
    vtkm::cont::DataSet meanWaterFraction = meanWaterFractionReader.ReadDataSet();
    // get the cellset
    auto cellSet = meanWaterFraction.GetCellSet();
    vtkm::cont::CellSetStructured<3> structCellSet =
        cellSet.AsCellSet<vtkm::cont::CellSetStructured<3>>();

    vtkm::Id3 pointDims = structCellSet.GetPointDimensions();
    std::cout << "------" << std::endl;
    std::cout << "point dim: " << pointDims[0] << " " << pointDims[1] << " " << pointDims[2] << std::endl;

    // get mean for the curl


    vtkm::cont::ArrayHandle<vtkm::FloatDefault> meanWaterFractionArray;
    vtkm::cont::ArrayCopyShallowIfPossible(meanWaterFraction.GetField("meanWaterFraction").GetData(), meanWaterFractionArray);

    // get dev for the curl
    vtkm::io::VTKDataSetReader devWaterFractionReader(devWaterFraction);
    vtkm::cont::DataSet DevCurlData = devWaterFractionReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> devWaterFractionDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(DevCurlData.GetField("devWaterFraction").GetData(), devWaterFractionDataArray);

    // get mean for the vorticity
    vtkm::io::VTKDataSetReader meanTempReader(meanTemp);
    vtkm::cont::DataSet meanVorData = meanTempReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> meanTempDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(meanVorData.GetField("meanTemperature").GetData(), meanTempDataArray);

    // get dev for the vorticity
    vtkm::io::VTKDataSetReader devTempReader(devTemp);
    vtkm::cont::DataSet devTempData = devTempReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> devTempDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(devTempData.GetField("devTemperature").GetData(), devTempDataArray);

    // get mean for the pressure
    vtkm::io::VTKDataSetReader meanPressureReader(meanPressure);
    vtkm::cont::DataSet meanPressureData = meanPressureReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> meanPressureDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(meanPressureData.GetField("meanPressure").GetData(), meanPressureDataArray);

    // get dev for the pressure
    vtkm::io::VTKDataSetReader devPressureReader(devPressure);
    vtkm::cont::DataSet devPressureData = devPressureReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> devPressureDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(devPressureData.GetField("devPressure").GetData(), devPressureDataArray);


    // get mean for the asteroid fraction
    vtkm::io::VTKDataSetReader meanAsteroidFractionReader(meanAsteroidFraction);
    vtkm::cont::DataSet meanAsteroidFractionData = meanAsteroidFractionReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> meanAsteroidFractionDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(meanAsteroidFractionData.GetField("meanAsteroidFraction").GetData(), meanAsteroidFractionDataArray);

    // get dev for the asteroid fraction
    vtkm::io::VTKDataSetReader devAsteroidFractionReader(devAsteroidFraction);
    vtkm::cont::DataSet devAsteroidFractionData = devAsteroidFractionReader.ReadDataSet();

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> devAsteroidFractionDataArray;
    vtkm::cont::ArrayCopyShallowIfPossible(devAsteroidFractionData.GetField("devAsteroidFraction").GetData(), devAsteroidFractionDataArray);

    // print summary
    // vtkm::cont::printSummary_ArrayHandle(meanWaterFractionArray, std::cout);
    // vtkm::cont::printSummary_ArrayHandle(devWaterFractionDataArray, std::cout);
    // vtkm::cont::printSummary_ArrayHandle(meanTempDataArray, std::cout);
    // vtkm::cont::printSummary_ArrayHandle(devTempDataArray, std::cout);

    // compute the min and max for the curl
    vtkm::cont::Invoker invoke;

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> minField1;
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> maxField1;

    invoke(ExtractingMinMaxFromMeanDev{}, meanWaterFractionArray, devWaterFractionDataArray, minField1, maxField1);

    // compute the min and max for the vorticity

    vtkm::cont::ArrayHandle<vtkm::FloatDefault> minField2;
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> maxField2;
    invoke(ExtractingMinMaxFromMeanDev{}, meanTempDataArray, devTempDataArray, minField2, maxField2);

    // compute the min and max for the pressure
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> minField3;
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> maxField3;

    invoke(ExtractingMinMaxFromMeanDev{}, meanPressureDataArray, devPressureDataArray, minField3, maxField3);

    // compute the min and max for the asteroid fraction
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> minField4;
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> maxField4;

    invoke(ExtractingMinMaxFromMeanDev{}, meanAsteroidFractionDataArray, devAsteroidFractionDataArray, minField4, maxField4);

    // user specify the field
    vtkm::filter::uncertainty::Fiber4Var filter;
    // curlz -15 -1
    // vorticity 1 15
    // big user specified rectangle need more monte carlo sampling
    vtkm::Vec4f minAxisValue(0, 0.01809121295809746, 34000, 0);

    //old 
    //vtkm::Pair<vtkm::FloatDefault, vtkm::FloatDefault> maxAxisValue(-0.1, 20);
    //new value matching paper
    vtkm::Vec4f maxAxisValue(0.5, 0.25, 400000000, 0);

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

    dataSetForFilter.AddPointField("ensemble_min_three", minField3);
    dataSetForFilter.AddPointField("ensemble_max_three", maxField3);

    dataSetForFilter.AddPointField("ensemble_min_four", minField4);
    dataSetForFilter.AddPointField("ensemble_max_four", maxField4);

    //dataSetForFilter.AddPointField("meanWaterFractionArray", meanWaterFractionArray);
    //dataSetForFilter.AddPointField("devWaterFractionDataArray", devWaterFractionDataArray);
    //dataSetForFilter.AddPointField("meanTempDataArray", meanTempDataArray);
    //dataSetForFilter.AddPointField("devTempDataArray", devTempDataArray);

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

    //vtkm::cont::Timer timer{initResult.Device};
    //std::cout << "timer device: " << timer.GetDevice().GetName() << std::endl;

    // run filter five times
    //for (int i = 1; i <= 5; i++)
    //{        
        //std::cout << "------" << std::endl;
        //std::cout << std::to_string(i) << "th run" << std::endl;
        //timer.Start();
        vtkm::cont::DataSet output = filter.Execute(dataSetForFilter);
        std::string outputFilename = "TestAsteroidImpact4Var"+Approach+std::to_string(NumSamples)+".vtk"; 
        vtkm::io::VTKDataSetWriter writer(outputFilename);
        writer.WriteDataSet(output);
        std::cout << "output file: " << outputFilename << std::endl;
        //timer.Synchronize();
        //timer.Stop();
        //vtkm::Float64 elapsedTime = timer.GetElapsedTime();
        //std::cout << "total elapsedTime:" << elapsedTime << std::endl;
    //}
}