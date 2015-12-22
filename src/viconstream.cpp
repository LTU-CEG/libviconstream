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
#include "viconstream/viconstream.h"


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

    void ViconStream::frameGrabberWorker()
    {
        logString("Frame grabber thread started!");

        Output_GetFrame f;

        while (!_shutdown)
        {
            /* Check so there is an active connection. */
            if (_vicon_client.IsConnected().Connected)
            {
                f = _vicon_client.GetFrame();

                if ((f.Result == Result::Success) &&
                    (_vicon_client.GetFrameNumber().FrameNumber > 0))
                {
                    /* Since all information is stored in the Client object it
                       will be passed by reference for the used to extract the
                       needed data, but not to run more code in the callback.
                    */
                    std::lock_guard<std::mutex> locker(_id_cblock);

                    for (auto &cb : callbacks)
                        cb.second(_vicon_client);
                }
                else
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else
            {
                logString("Frame grabber is running but no connection... \
                        Something is horribly wrong!");

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    /*********************************
     * Public members
     ********************************/

    ViconStream::ViconStream(std::string hostname, std::ostream &log_output)
        : _host_name(hostname), _log(log_output)
    {
        _tp_start = std::chrono::high_resolution_clock::now();
    }

    ViconStream::~ViconStream()
    {
        if (_shutdown == false)
            disableStream();
    }

    bool ViconStream::enableStream(bool enableSegmentData,
                                   bool enableMarkerData,
                                   bool enableUnlabeledMarkerData,
                                   bool enableDeviceData,
                                   StreamMode::Enum streamMode)
    {
        _shutdown = false;

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
            logString("Segment Data:            enabled");
        }
        else
        {
            _vicon_client.DisableSegmentData();
            logString("Segment Data:            disabled");
        }

        if (enableMarkerData)
        {
            _vicon_client.EnableMarkerData();
            logString("Marker Data:             enabled");
        }
        else
        {
            _vicon_client.DisableMarkerData();
            logString("Marker Data:             disabled");
        }

        if (enableUnlabeledMarkerData)
        {
            _vicon_client.EnableUnlabeledMarkerData();
            logString("Unlabeled Marker Data:   enabled");
        }
        else
        {
            _vicon_client.DisableUnlabeledMarkerData();
            logString("Unlabeled Marker Data:   disabled");
        }

        if (enableDeviceData)
        {
            _vicon_client.EnableDeviceData();
            logString("Device Data:             enabled");
        }
        else
        {
            _vicon_client.DisableDeviceData();
            logString("Device Data:             disabled");
        }

        /* Set stream mode */
        _vicon_client.SetStreamMode(streamMode);

        if (streamMode == StreamMode::ServerPush)
            logString("Stream mode:             ServerPush");
        else if (streamMode == StreamMode::ClientPull)
            logString("Stream mode:             ClientPull");
        else
            logString("Stream mode:             ClientPullPreFetch");

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
                /* Checking if end of look. */
                if (i == 9)
                {
                    logString("Frame grabber startup failed, aborting!");
                    _vicon_client.Disconnect();

                    return false;
                }
            }
        }

        std::stringstream s;
        s << _vicon_client.GetFrameRate().FrameRateHz;
        logString("Frame rate:              " + s.str() + " Hz") ;

        /* Start the frame grabber/data pump thread. */
        logString("Starting the frame grabber thread...");
        _frame_grabber = std::thread(&ViconStream::frameGrabberWorker, this);

        return true;
    }

    void ViconStream::disableStream()
    {

        if (_vicon_client.IsConnected().Connected)
        {
            logString("Terminating the frame grabber...");

            _shutdown = true;
            _frame_grabber.join();

            logString("Frame grabber terminated!");

            _vicon_client.Disconnect();

            logString("Connection to " + _host_name + " closed.");
        }

    }

    unsigned int ViconStream::registerCallback(viconstream_callback callback)
    {
        std::lock_guard<std::mutex> locker(_id_cblock);

        /* Add the callback to the list. */
        callbacks.emplace(_id, callback);

        return _id++;
    }

    bool ViconStream::unregisterCallback(const unsigned int id)
    {
        std::lock_guard<std::mutex> locker(_id_cblock);

        /* Delete the callback with correct ID. */
        if (callbacks.erase(id) > 0)
            return true;
        else
            /* No match, return false. */
            return false;
    }
}
