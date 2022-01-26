#ifndef TEST_HEADER

#define TEST_HEADER

// inclustions
#include <iostream>
#include <cassert>
#include <string>

// using
using std::cout;
using std::string;

// constants

// input paths
const string test_os_file_path = "./test_input/test_os";
const string test_proc_meminfo = "./test_input/test_proc_meminfo";
const string test_proc_stat = "./test_input/test_proc_stat";

// input values
const string test_os_key = "PRETTY_NAME";
const string test_stat_processes = "processes";

// expected values
const string test_exp_os_value = "Ubuntu 20.04.3 LTS";
const int test_exp_processes = 41622;

#endif