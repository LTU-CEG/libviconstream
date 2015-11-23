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
#include "viconstream.h"


namespace ViconStream
{
    void ViconStream::viconFrameGrabberWorker()
    {
        while (!shutdown)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    ViconStream::ViconStream(std::string hostname, std::ostream &log)
        : _host_name(hostname), _log(log)
    {
        shutdown = false;
    }

    ViconStream::~ViconStream()
    {
    }

    bool ViconStream::enableStream(bool enableSegmentData,
                                   bool enableMarkerData,
                                   bool enableUnlabeledMarkerData,
                                   bool enableDeviceData,
                                   StreamMode::Enum streamMode)
    {
        _log << "Connecting to " << _host_name << "..." << std::endl;

        int cnt = 0;

        /* Try to connect to the Vicon host. */
        while(!_vicon_client.IsConnected().Connected)
        {
            if (_vicon_client.Connect( _host_name ).Result != Result::Success)
            {
                _log << "Warning: Connection failed, retrying..." << std::endl;
                cnt++;
            }
            else
            {
                break;
            }

            /* Connection failed. */
            if (cnt >= 3)
            {
                _log << "Error: Connection failed, aborting!" << std::endl;
                return false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        /* Connection established, apply settings. */

        /* Enable data */
        if (enableSegmentData)
        {
            _vicon_client.EnableSegmentData();
            _log << "Segment Data enabled." << std::endl;
        }

        if (enableMarkerData)
        {
            _vicon_client.EnableMarkerData();
            _log << "Marker Data enabled." << std::endl;
        }

        if (enableUnlabeledMarkerData)
        {
            _vicon_client.EnableUnlabeledMarkerData();
            _log << "Unlabeled Marker Data enabled." << std::endl;
        }

        if (enableDeviceData)
        {
            _vicon_client.EnableDeviceData();
            _log << "Device Data enabled." << std::endl;
        }

        /* Set stream mode */
        _vicon_client.SetStreamMode(streamMode);

        if (streamMode == StreamMode::ServerPush)
            _log << "Stream mode is ServerPush." << std::endl;
        else if (streamMode == StreamMode::ClientPull)
            _log << "Stream mode is ClientPull." << std::endl;
        else
            _log << "Stream mode is ClientPullPreFetch." << std::endl;

        /* Set axis mapping (Z up) */
        _vicon_client.SetAxisMapping(Direction::Forward,
                                     Direction::Left,
                                     Direction::Up);

        /* Testing the frame grabber. */
        _log << "Testing the frame grabber...";

        Output_GetFrame f;

        for (int i = 0; i < 10; i++)
        {
            f = _vicon_client.GetFrame();

            if (f.Result == Result::Success)
            {
                _log << "OK!" << std::endl;
                break;
            }
            else
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(10)
                );

                if (i == 9)
                {
                    _log << "Failed, aborting!" << std::endl;
                    _vicon_client.Disconnect();

                    return false;
                }
            }
        }

        /* Start the frame grabber/data pump thread. */
        _frame_grabber = std::thread(&ViconStream::_frame_grabber, this);

        return true;
    }

}
