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

    stringstream dsets_basenames_in_1_ss, dsets_basenames_in_2_ss, dsets_basenames_out_ss;
    dsets_basenames_in_1_ss << DATASETS_BASENAMES_IN_1;
    dsets_basenames_in_2_ss << DATASETS_BASENAMES_IN_2;
    dsets_basenames_out_ss  << DATASETS_BASENAMES_OUT;
    string dummy_str;
    vector<string> dsets_basenames_in_1_vec, dsets_basenames_in_2_vec, dsets_basenames_out_vec;

    while (dsets_basenames_in_1_ss >> dummy_str) {
        dsets_basenames_in_1_vec.push_back(dummy_str);
    }

    while (dsets_basenames_in_2_ss >> dummy_str) {
        dsets_basenames_in_2_vec.push_back(dummy_str);
    }

    while (dsets_basenames_out_ss >> dummy_str) {
        dsets_basenames_out_vec.push_back(dummy_str);
    }

    const auto &ndsets = dsets_basenames_in_1_vec.size();
    assert(ndsets > 0);

    if (dsets_basenames_in_2_vec.size() != ndsets or
        dsets_basenames_out_vec.size()  != ndsets) {
        cout << "ERROR: inconsistent number of input/output datasets provided:" << endl
             << "  DATASETS_BASENAMES_IN_1 = '" << dsets_basenames_in_1_ss.str() << "' has " << ndsets                          << " elements" << endl
             << "  DATASETS_BASENAMES_IN_2 = '" << dsets_basenames_in_2_ss.str() << "' has " << dsets_basenames_in_2_vec.size() << " elements" << endl
             << "  DATASETS_BASENAMES_OUT  = '" << dsets_basenames_out_ss.str()  << "' has " << dsets_basenames_out_vec.size()  << " elements" << endl;
        return 1;
    }

    vector<vector<string>> dset_rename_lists(ndsets);

    const op_data_t op_data = {
        &output_file, &input_file_2, ndsets,
        &dsets_basenames_in_1_vec,
        &dsets_basenames_in_2_vec,
        &dsets_basenames_out_vec,
        &dset_rename_lists
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

    for (auto n = decltype(ndsets){0}; n < ndsets; ++n) {
        for (const auto &dset_name : dset_rename_lists.at(n)) {
            ostringstream regex_ss;
            regex_ss << "(?:[a-zA-Z0-9_:\\[\\]]+) it=([0-9]+) tl=([0-9]+)(?: m=)?([0-9]+)? rl=([0-9]+) c=([0-9]+)";

            try {
                const regex rule(regex_ss.str());
                smatch matches;

                if (regex_match(dset_name, matches, rule)) {
                    assert(matches.size() == 6);
                    const auto dset_name_regex = matches.str(0);
                    const auto it = matches.str(1);
                    const auto tl = matches.str(2);
                    const auto m  = (matches[3].length() == 0) ? to_string(0) : matches.str(3);
                    const auto rl = matches.str(4);
                    const auto c  = matches.str(5);

                    assert(dset_name_regex == dset_name);

                    ostringstream new_dset_name_ss;
                    new_dset_name_ss << dsets_basenames_out_vec.at(n)
                                     << " it=" << it << " tl=" << tl;

                    if ((matches[3].length() != 0)) {
                        new_dset_name_ss << " m=" << m;
                    }

                    new_dset_name_ss << " rl=" << rl << " c=" << c;
                    const auto new_dset_name_str = new_dset_name_ss.str();

                    /* Only rename if the new name is actually different from the
                     * old one, otherwise H5Lmove fails                         */
                    if (dset_name != new_dset_name_str) {
                        #if (VERBOSE)
                        cout << "Changing '" << dset_name
                             << "' into '"   << new_dset_name_str << "'" << endl;
                        #endif
                        CHECK_ERROR(H5Lmove(outfile_id, dset_name.c_str(),
                                            outfile_id, new_dset_name_str.c_str(),
                                            H5P_DEFAULT, H5P_DEFAULT)); }
                    else {
                        cout << "Same names '" << dset_name
                             << "' and '"      << new_dset_name_str
                             << "', not going to rename the dataset" << endl;
                    }
                }

                else {
                    cout << "Dataset name '" << dset_name
                         << "' didn't match against regex" << endl;
                    return 1;
                }
            }

            catch (const regex_error &e) {
                cout << "Regex exception caught: " << e.what()
                     << " (error code " << e.code() << ")" << endl;
                return -1;
            }
        }
    }

    const auto rename_end  = chrono::high_resolution_clock::now();
    const auto rename_time = chrono::duration_cast<chrono::seconds>(rename_end - rename_start).count();

    cout << "Done, elapsed time: " << rename_time << "s" << endl << endl;


    const auto total_time = chrono::duration_cast<chrono::seconds>(rename_end - copy_start).count();

    cout << "All done, total elapsed time: " << total_time << "s" << endl << endl;


    return 0;
}
