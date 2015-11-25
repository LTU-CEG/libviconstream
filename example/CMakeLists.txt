############################################################################
#
# Copyright (C) 2015 Emil Fresk.
# All rights reserved.
#
# This file is part of the cppViconStream library.
#
# GNU Lesser General Public License Usage
# This file may be used under the terms of the GNU Lesser
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE included in the
# packaging of this file.  Please review the following information to
# ensure the GNU Lesser General Public License version 3.0 requirements
# will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
#
# If you have questions regarding the use of this file, please contact
# Emil Fresk at emil.fresk@gmail.com.
#
############################################################################

########################################
# Add the example executable
########################################
add_executable(vs_example example.cpp)

########################################
# Library linking
########################################
target_link_libraries(vs_example viconstream)
