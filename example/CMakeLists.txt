###          Copyright Emil Fresk 2015-2017.
### Distributed under the Boost Software License, Version 1.0.
###    (See accompanying file LICENSE.md or copy at
###          http://www.boost.org/LICENSE_1_0.txt)

########################################
# Add the example executable
########################################
add_executable(vs_example example.cpp)

########################################
# Library linking
########################################
target_link_libraries(vs_example libviconstream)
