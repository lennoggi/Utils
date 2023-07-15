#include <cstdlib>
#include <iostream>
#include <sstream>
#include <chrono>

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

    const auto copy_start = chrono::high_resolution_clock::now();

    system(cmd.str().c_str());

    const auto copy_end  = chrono::high_resolution_clock::now();
    const auto copy_time = chrono::duration_cast<chrono::seconds>(copy_end - copy_start).count();

    cout << "Copied '" << INPUT_FILE_1 << "' into '"
         << OUTPUT_FILE << "' (elapsed time: " << copy_time << "s)" << endl; 


    // Edit the output file using input file 2
    H5File output_file(OUTPUT_FILE,   H5F_ACC_RDWR);
    H5File input_file_2(INPUT_FILE_2, H5F_ACC_RDONLY);

    const op_data_t op_data = {&output_file, &input_file_2};

    const auto iterate_start = chrono::high_resolution_clock::now();

    CHECK_ERROR(H5Literate(output_file.getId(),
                           H5_INDEX_NAME, H5_ITER_NATIVE, nullptr,
                           combine_Cactus_HDF5, (void*) &op_data));

    const auto iterate_end  = chrono::high_resolution_clock::now();
    const auto iterate_time = chrono::duration_cast<chrono::seconds>(iterate_end - iterate_start).count();

    cout << "Done (processing time: " << iterate_time << "s)" << endl;

    return 0;
}
