/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2010-2012, Multicoreware, Inc., all rights reserved.
// Copyright (C) 2010-2012, Advanced Micro Devices, Inc., all rights reserved.
// Third party copyrights are property of their respective owners.

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other oclMaterials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors as is and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "perf_precomp.hpp"

int main(int argc, const char *argv[])
{
    const char *keys =
        "{ h | help    | false | print help message }"
        "{ f | filter  |       | filter for test }"
        "{ w | workdir |       | set working directory }"
        "{ l | list    | false | show all tests }"
        "{ d | device  | 0     | device id }"
        "{ i | iters   | 10    | iteration count }"
        "{ m | warmup  | 1     | gpu warm up iteration count}"
        "{ t | xtop    | 1.1   | xfactor top boundary}"
        "{ b | xbottom | 0.9   | xfactor bottom boundary}"
        "{ v | verify  | false | only run gpu once to verify if problems occur}";

    redirectError(cvErrorCallback);
    CommandLineParser cmd(argc, argv, keys);
    if (cmd.get<bool>("help"))
    {
        cout << "Avaible options:" << endl;
        cmd.printParams();
        return 0;
    }

    // OpenCV version info
    cout << "OpenCV version: " << CV_VERSION << endl;
    string buildInfo( cv::getBuildInformation() );
    size_t pos1 = buildInfo.find("Version control");
    size_t pos2 = buildInfo.find('\n', pos1);
    if(pos1 != string::npos && pos2 != string::npos)
    {
        size_t value_start = buildInfo.rfind(' ', pos2) + 1;
        string ver( buildInfo.substr(value_start, pos2 - value_start) );
        cout << "OpenCV VCS version: " << ver << endl;
    }

    // get ocl devices
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 4 && CV_SUBMINOR_VERSION == 6
    bool use_cpu = cmd.get<bool>("c");
    vector<ocl::Info> oclinfo;
    int num_devices = 0;
    if(use_cpu)
        num_devices = getDevice(oclinfo, ocl::CVCL_DEVICE_TYPE_CPU);
    else
        num_devices = getDevice(oclinfo);
    if (num_devices < 1)
    {
        cerr << "no device found\n";
        return -1;
    }

    // show device info
    int devidx = 0;
    for (size_t i = 0; i < oclinfo.size(); i++)
    {
        for (size_t j = 0; j < oclinfo[i].DeviceName.size(); j++)
        {
            cout << "device " << devidx++ << ": " << oclinfo[i].DeviceName[j] << endl;
        }
    }

    int device = cmd.get<int>("device");
    if (device < 0 || device >= num_devices)
    {
        cerr << "Invalid device ID" << endl;
        return -1;
    }

    // set this to overwrite binary cache every time the test starts
    ocl::setBinaryDiskCache(ocl::CACHE_UPDATE);

    devidx = 0;
    for (size_t i = 0; i < oclinfo.size(); i++)
    {
        for (size_t j = 0; j < oclinfo[i].DeviceName.size(); j++, devidx++)
        {
            if (device == devidx)
            {
                ocl::setDevice(oclinfo[i], (int)j);
                TestSystem::instance().setRecordName(oclinfo[i].DeviceName[j]);
                cout << "use " << devidx << ": " <<oclinfo[i].DeviceName[j] << endl;
                goto END_DEV;
            }
        }
    }
END_DEV:

#elif CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 4 && CV_SUBMINOR_VERSION > 6
    ocl::DevicesInfo di;
    if( getOpenCLDevices(di) < 1)
    {
        cerr << "Error: no OCL device found;" << endl;
        return -1;
    }

    // show device info
    for (size_t i = 0; i < di.size(); i++)
    {
        cout << "Device " << i << ": " << di[i]->deviceName << " (unified memory: " << (di[i]->isUnifiedMemory ? "yes)":"no)" ) << endl;
    }

    int device = cmd.get<int>("device");
    if (device < 0 || device >= di.size())
    {
        cerr << "Invalid device ID: " << device << endl;
        return -1;
    }

    cout << "Using device " << device << ": " << di[device]->deviceName << endl;
    ocl::setDevice(di[device]);
    TestSystem::instance().setRecordName(di[device]->deviceName);
#else
#error Unsupported OpenCV version
#endif // CV_MAJOR_VERSION ...

    if (cmd.get<bool>("verify"))
    {
        TestSystem::instance().setNumIters(1);
        TestSystem::instance().setGPUWarmupIters(0);
        TestSystem::instance().setCPUIters(0);
    }

    string filter = cmd.get<string>("filter");
    string workdir = cmd.get<string>("workdir");
    bool list = cmd.get<bool>("list");
    int iters = cmd.get<int>("iters");
    int wu_iters = cmd.get<int>("warmup");
    double x_top = cmd.get<double>("xtop");
    double x_bottom = cmd.get<double>("xbottom");

    TestSystem::instance().setTopThreshold(x_top);
    TestSystem::instance().setBottomThreshold(x_bottom);

    if (!filter.empty())
    {
        TestSystem::instance().setTestFilter(filter);
    }

    if (!workdir.empty())
    {
        if (workdir[workdir.size() - 1] != '/' && workdir[workdir.size() - 1] != '\\')
        {
            workdir += '/';
        }

        cout << "Working dir: " << workdir << endl;
        TestSystem::instance().setWorkingDir(workdir);
    }

    if (list)
    {
        TestSystem::instance().setListMode(true);
    }

    TestSystem::instance().setNumIters(iters);
    TestSystem::instance().setGPUWarmupIters(wu_iters);

    TestSystem::instance().run();

    return 0;
}
