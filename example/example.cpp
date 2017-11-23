//          Copyright Emil Fresk 2015-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include "libviconstream/viconstream.h"

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

