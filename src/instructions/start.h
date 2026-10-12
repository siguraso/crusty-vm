#pragma once

#ifndef START_H
#define START_H

#include "../class_file_parser/class_file_parser.h"
#include <stdlib.h>

method_info_t* locate_main(class_data_t* class_data);

// idk how to implement this quite yet
// int start(class_data_t* class_data, size_t main_id);

#endif // START_H
