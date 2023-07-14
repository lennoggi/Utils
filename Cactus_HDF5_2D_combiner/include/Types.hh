#ifndef TYPES_HH
#define TYPES_HH

#include <hdf5.h>


// Custom datatype to store all objects needed by get_first_last_its()
typedef struct {
    H5::H5File *output_file_ptr;
    H5::H5File *input_file_2_ptr;
} op_data_t;


#endif  // TYPES_HH
