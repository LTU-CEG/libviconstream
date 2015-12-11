/****************************************************************************
*
* Copyright (C) 2015 Emil Fresk.
* All rights reserved.
*
* This file is part of the cppViconStream library.
*
* GNU Lesser General Public License Usage
* This file may be used under the terms of the GNU Lesser
* General Public License version 3.0 as published by the Free Software
* Foundation and appearing in the file LICENSE included in the
* packaging of this file.  Please review the following information to
* ensure the GNU Lesser General Public License version 3.0 requirements
* will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
*
* If you have questions regarding the use of this file, please contact
* Emil Fresk at emil.fresk@gmail.com.
*
****************************************************************************/

#include <iostream>
#include "../viconstream.h"

using namespace std;
using namespace ViconDataStreamSDK::CPP;

void test_cb(const Client &frame)
{
    cout << "Frame: " << frame.GetFrameNumber().FrameNumber << endl;
}

int main(int argc, char *argv[])
{

    /* Get an address to the Vicon system. */
    std::string ip = "vicon.research.ltu.se:801";

    if(argc > 1)
        ip = argv[1];

    /* Create the vicon stream object with logging to cout. */
    ViconStream::ViconStream vs(ip, std::cout);

    /* Register a callback. */
    vs.registerCallback(test_cb);

    /* Enable the stream with default parameters. */
    vs.enableStream();

    this_thread::sleep_for(chrono::seconds(1));

    /* Disable stream (optional, is disabled when vs goes out of scope). */
    //vs.disableStream();

    return 0;
}

