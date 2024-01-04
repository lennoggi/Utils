#ifndef TYPES_HH
#define TYPES_HH

#include <string>
#include <vector>
#include <H5Cpp.h>


// Custom datatype to store all objects needed by get_first_last_its()
typedef struct {
    H5::H5File *output_file_ptr;
    H5::H5File *input_file_2_ptr;
    std::string out_dset_name;
    size_t      out_dset_name_size;
    std::vector<std::string> *dset_list_ptr;
} op_data_t;


#endif  // TYPES_HH
