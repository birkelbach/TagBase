#  Copyright (c) 2020 Phil Birkelbach
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

# These tests exercise the system at the library call level.  The tag server
# is started and then various library functions are called to test different
# aspects of the system.

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/tests)

add_definitions(-DTESTING) # Compiles some testing code into the library */


set(test_list basic_test
              tag_avl_test
              tag_handle_test
              basic_read_write_test
              basic_event_test
   )

foreach(test IN LISTS test_list)
    add_executable(${test}  ${test}.c)
    target_link_libraries(${test} tagbase)
    add_test(${test} ${test})
  set_tests_properties(${test} PROPERTIES TIMEOUT 5)
endforeach()

add_executable(avl_test avl_test.c ../src/avl_tree.c)
add_test(avl_test avl_test)
set_tests_properties(avl_test PROPERTIES TIMEOUT 5)
