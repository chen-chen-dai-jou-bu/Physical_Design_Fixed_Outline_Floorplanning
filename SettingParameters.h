#ifndef SETTING_PARAMETERS
#define SETTING_PARAMETERS

#include<chrono>

// Soft Blocks Aspect Ratio
const double INITAL_MINIMUM_ASPECT_RATIO = 0.5;
const double INITAL_MAXIMUM_ASPECT_RATIO = 2;

// Time Limit in this program. This program will be limited to finish within 5 minutes.
std::chrono::milliseconds TIME_LIMIT(570000); // 9 minutes 30 seconds
std::chrono::high_resolution_clock::time_point RUNNING_START = std::chrono::high_resolution_clock::now();

bool ENABLE_ONLY_ONE_FEASIBLE = false;
bool ENABLE_WRITE_OUT_FILE = false;
long MAXIMUM_THREADS = 5;

// For Read/Write File
std::string GLOBAL_INPUT_FILE_PATH = "";
std::string GLOBAL_OUTPUT_FILE_PATH = "";

#endif /* SETTING_PARAMETERS */