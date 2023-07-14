#include <cstdlib>
#include <iostream>
#include <sstream>
#include <H5Cpp.h>

#include "include/Macros.hh"
#include "include/Types.hh"
#include "include/Declarations.hh"
#include "Parameters.hh"


// Macro checking for errors coming from routines returning error codes
#define CHECK_ERROR(routine)                     \
do {                                             \
    const int err = routine;                     \
    if (err < 0) {                               \
        cout << "Routine '" << #routine          \
             << "' returned error code " << err  \
             << endl;                            \
        std::exit(EXIT_FAILURE);                 \
    }                                            \
} while (0)


using namespace std;
using namespace H5;



int main() {
    /* Copy the first input file into a new file, so that the latter inherits
     * the full input file structure (datasets and their attributes, metadata
     * group) automatically                                                     */
    ostringstream cmd;
    cmd << "cp " << INPUT_FILE_1 << " " << OUTPUT_FILE;
    system(cmd.str().c_str());
    cout << "Copied '" << INPUT_FILE_1 << "' into '"
         << OUTPUT_FILE << "'" << endl; 


    // Edit the output file using input file 2
    H5File output_file(OUTPUT_FILE,   H5F_ACC_RDONLY);
    H5File input_file_2(INPUT_FILE_2, H5F_ACC_RDONLY);

    const op_data_t op_data = {&output_file, &input_file_2};

    CHECK_ERROR(H5Literate(output_file.getId(),
                           H5_INDEX_NAME, H5_ITER_NATIVE, nullptr,
                           combine_Cactus_HDF5, (void*) &op_data));

    cout << "Done" << endl;

    return 0;
}
