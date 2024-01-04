#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <regex>
#include <chrono>

#include <H5Cpp.h>

#include "include/Macros.hh"
#include "include/Types.hh"
#include "include/Declarations.hh"
#include "Parameters.hh"


using namespace std;
using namespace H5;



int main() {
    /* Copy the first input file into a new file, so that the latter inherits
     * the full input file structure (datasets and their attributes, metadata
     * group) automatically                                                     */
    ostringstream cmd;
    cmd << "cp " << INPUT_FILE_1 << " " << OUTPUT_FILE;

    cout << endl << "Copying '" << INPUT_FILE_1 << "' into '" << OUTPUT_FILE << "'..." << endl;

    const auto copy_start = chrono::high_resolution_clock::now();

    system(cmd.str().c_str());

    const auto copy_end  = chrono::high_resolution_clock::now();
    const auto copy_time = chrono::duration_cast<chrono::seconds>(copy_end - copy_start).count();

    cout << "Done, elapsed time: " << copy_time << "s" << endl << endl;


    // Edit the output file using input file 2
    H5File output_file(OUTPUT_FILE,   H5F_ACC_RDWR);
    H5File input_file_2(INPUT_FILE_2, H5F_ACC_RDONLY);

    const string out_dset_name      = DATASET_OUTPUT;
    const auto   out_dset_name_size = out_dset_name.size();

    vector<string> dset_list;

    const op_data_t op_data = {
        &output_file, &input_file_2,
        out_dset_name, out_dset_name_size,
        &dset_list
    };

    cout << "Editing datasets in output file '" << OUTPUT_FILE << "'..." << endl;

    const auto iterate_start = chrono::high_resolution_clock::now();

    CHECK_ERROR(H5Literate(output_file.getId(),
                           H5_INDEX_NAME, H5_ITER_NATIVE, nullptr,
                           combine_Cactus_HDF5, (void*) &op_data));

    const auto iterate_end  = chrono::high_resolution_clock::now();
    const auto iterate_time = chrono::duration_cast<chrono::seconds>(iterate_end - iterate_start).count();

    cout << "Done, elapsed time: " << iterate_time << "s" << endl << endl;


    // Rename all datasets in the output file
    const auto &outfile_id = output_file.getId();
    cout << "Renaming datasets in output file '" << OUTPUT_FILE << "'..." << endl;
    const auto rename_start = chrono::high_resolution_clock::now();

    for (const auto &dset_name : dset_list) {
        ostringstream regex_ss;
        regex_ss << "(?:[a-zA-Z0-9_:]+) it=([0-9]+) tl=0(?: m=)?([0-9]+)? rl=([0-9]+) c=([0-9]+)";

        try {
            const regex rule(regex_ss.str());
            smatch matches;

            if (regex_match(dset_name, matches, rule)) {
                assert(matches.size() == 5);
                const auto dset_name_regex = matches.str(0);
                const auto it   = matches.str(1);
                const auto map  = (matches[2].length() == 0) ? to_string(0) : matches.str(2);
                const auto rlvl = matches.str(3);
                const auto comp = matches.str(4);

                assert(dset_name_regex == dset_name);

                ostringstream new_dset_name_ss;
                new_dset_name_ss << out_dset_name << " it=" << it << " tl=0";

                if ((matches[2].length() != 0)) {
                    new_dset_name_ss << " m=" << map;
                }

                new_dset_name_ss << " rl=" << rlvl << " c=" << comp;

                CHECK_ERROR(H5Lmove(outfile_id, dset_name.c_str(),
                                    outfile_id, new_dset_name_ss.str().c_str(),
                                    H5P_DEFAULT, H5P_DEFAULT));
            }

            else {
                cout << "Dataset name '" << dset_name << "' didn't match against regex" << endl;
                return 1;
            }
        }

        catch (const regex_error &e) {
            cout << "Regex exception caught: " << e.what()
                 << " (error code " << e.code() << ")" << endl;
            return -1;
        }
    }

    const auto rename_end  = chrono::high_resolution_clock::now();
    const auto rename_time = chrono::duration_cast<chrono::seconds>(rename_end - rename_start).count();

    cout << "Done, elapsed time: " << rename_time << "s" << endl << endl;


    const auto total_time = chrono::duration_cast<chrono::seconds>(rename_end - copy_start).count();

    cout << "All done, total elapsed time: " << total_time << "s" << endl << endl;


    return 0;
}
