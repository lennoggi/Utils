#ifndef TYPES_HH
#define TYPES_HH

#include <string>
#include <vector>
#include <H5Cpp.h>


// Custom datatype to store all objects needed by get_first_last_its()
typedef struct {
    H5::H5File *output_file_ptr;
    H5::H5File *input_file_2_ptr;
    size_t ndsets;
    std::vector<std::string> *dsets_basenames_in_1_vec_ptr;
    std::vector<std::string> *dsets_basenames_in_2_vec_ptr;
    std::vector<std::string> *dsets_basenames_out_vec_ptr;
    std::vector<std::vector<std::string>> *dset_rename_lists_ptr;
} op_data_t;


#endif  // TYPES_HH
