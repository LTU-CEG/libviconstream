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

/* Data includes. */
#include <string>
#include <vector>

/* Threading includes. */
#include <thread>
#include <mutex>
#include <condition_variable>

/* Vicon include. */
#include "Client.h"

#ifndef _VICONSTREAM_H
#define _VICONSTREAM_H

using namespace ViconDataStreamSDK::CPP;

namespace ViconStream
{
    class ViconStream;
}

class ViconStream::ViconStream
{

private:
    Client _vicon_client;
    std::string _host_name;
    std::chrono::high_resolution_clock::time_point _tp_start;
    std::ostream &_log;
    std::mutex _log_lock;
    std::thread _frame_grabber;
    volatile bool shutdown;

    void logString(const std::string log);
    void viconFrameGrabberWorker();

public:
    ViconStream(std::string hostname, std::ostream &log_output);
    ~ViconStream();
    bool enableStream(bool enableSegmentData = true,
                      bool enableMarkerData = false,
                      bool enableUnlabeledMarkerData = false,
                      bool enableDeviceData = false,
                      StreamMode::Enum streamMode = StreamMode::ServerPush);
    void disableStream();

};

#endif
