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
#include <chrono>
#include <sstream>
#include <iomanip>
#include "viconstream.h"


namespace ViconStream
{
    /*********************************
     * Private members
     ********************************/

    void ViconStream::logString(const std::string log)
    {
        /* Check the execution time. */
        auto tp = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::duration<double>>
            (tp - _tp_start).count();

        /* Convert to desired format (6 decimals). */
        std::stringstream s;

        s << std::fixed <<
            std::setprecision(std::numeric_limits<double>::digits10) << diff;

        /* Convert to string. */
        std::string res = s.str();
        size_t dotIndex = res.find(".");
        res = res.substr(0, dotIndex + 7);

        /* Lock the log output. */
        std::lock_guard<std::mutex> locker(_log_lock);

        /* Send it to the log ostream. */
        _log << "[" <<  res << "] ViconLog: " << log << std::endl;
    }

    void ViconStream::viconFrameGrabberWorker()
    {
        logString("Frame grabber thread started!");

        while (!shutdown)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }


    /*********************************
     * Public members
     ********************************/

    ViconStream::ViconStream(std::string hostname, std::ostream &log_output)
        : _host_name(hostname), _log(log_output)
    {
        _tp_start = std::chrono::high_resolution_clock::now();
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
        logString("Connecting to " + _host_name + "...");

        int cnt = 0;

        /* Try to connect to the Vicon host. */
        while(!_vicon_client.IsConnected().Connected)
        {
            /* Connection failed. */
            if (cnt >= 3)
            {
                logString("Error: Connection failed, aborting!");
                return false;
            }

            if (_vicon_client.Connect( _host_name ).Result != Result::Success)
            {
                logString("Warning: Connection failed, retrying...");
                cnt++;
            }
            else
            {
                logString("Success! Connected to " + _host_name);
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        /*
         * Connection established, apply settings.
         */

        /* Enable data based on the selected inputs. */
        if (enableSegmentData)
        {
            _vicon_client.EnableSegmentData();
            logString("Segment Data enabled.");
        }
        else
        {
            _vicon_client.DisableSegmentData();
            logString("Segment Data disabled.");
        }

        if (enableMarkerData)
        {
            _vicon_client.EnableMarkerData();
            logString("Marker Data enabled.");
        }
        else
        {
            _vicon_client.DisableMarkerData();
            logString("Marker Data disabled.");
        }

        if (enableUnlabeledMarkerData)
        {
            _vicon_client.EnableUnlabeledMarkerData();
            logString("Unlabeled Marker Data enabled.");
        }
        else
        {
            _vicon_client.DisableUnlabeledMarkerData();
            logString("Unlabeled Marker Data disabled.");
        }

        if (enableDeviceData)
        {
            _vicon_client.EnableDeviceData();
            logString("Device Data enabled.");
        }
        else
        {
            _vicon_client.DisableDeviceData();
            logString("Device Data disabled.");
        }

        /* Set stream mode */
        _vicon_client.SetStreamMode(streamMode);

        if (streamMode == StreamMode::ServerPush)
            logString("Stream mode is ServerPush.");
        else if (streamMode == StreamMode::ClientPull)
            logString("Stream mode is ClientPull.");
        else
            logString("Stream mode is ClientPullPreFetch.");

        /* Set axis mapping (Z up) */
        _vicon_client.SetAxisMapping(Direction::Forward,
                                     Direction::Left,
                                     Direction::Up);

        /* Testing the frame grabber. */
        Output_GetFrame f;

        for (int i = 0; i < 10; i++)
        {
            f = _vicon_client.GetFrame();

            if (f.Result == Result::Success)
            {
                break;
            }
            else
            {
                /* Wait a little to not overload the thread. */
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(10)
                );

                /* Checking if end of look. */
                if (i == 9)
                {
                    logString("Frame grabber startup failed, aborting!");
                    _vicon_client.Disconnect();

                    return false;
                }
            }
        }

        /* Start the frame grabber/data pump thread. */
        logString("Starting the frame grabber thread...");
        _frame_grabber = std::thread(&ViconStream::_frame_grabber, this);

        return true;
    }

    void ViconStream::disableStream()
    {
        if (_vicon_client.IsConnected().Connected)
        {
            _vicon_client.Disconnect();
        }
    }
}
