#include <cassert>
#include <vector>
#include <regex>
#include <hdf5.h>

#include "include/Macros.hh"
#include "include/Types.hh"
#include "include/Declarations.hh"
#include "Parameters.hh.hh"

using namespace std;


herr_t combine_Cactus_HDF5(hid_t             loc_id,
                           const char       *object_name,
                           const H5L_info_t *info,  // Not used
                           void             *op_data) {
    // Don't use 'Parameters and Global Attributes'
    const string object_name_str = object_name;
    if (object_name_str == "Parameters and Global Attributes") {
        return 0;
    }

    const auto output_file_ptr  = ((op_data_t*) op_data)->output_file_ptr;
    const auto input_file_2_ptr = ((op_data_t*) op_data)->input_file_2_ptr;

    ostringstream regex_ss;
    regex_ss << DATASET_BASENAME_1
             << "it=([0-9]+) tl=0(?: m=)?([0-9]+)? rl=([0-9]+) c=([0-9]+)"

    try {
        const regex rule(regex_ss.str());
        smatch matches;

        // Try to match the object name against the regex
        if (regex_match(object_name_str, matches, rule)) {
            assert(matches.size() == 5);
            const auto dset1_name = matches.str(0);
            const auto it         = matches.str(1);
            const auto map        = (matches.str(2).empty()) ? 0 : matches.str(2);
            const auto rlvl       = matches.str(3);
            const auto comp       = matches.str(4);

            const auto dset1 = output_file_ptr->openDataSet(dset1_name);
            DataSet dset2;

            /* Try to open dataset 2 with the same iteration, map, refinement
             * level and component as dataset 1                                 */
            try {
                ostringstream dset_input_file_2_name_ss;
                dset2_name_ss << DATASET_BASENAME_2
                    << " it=" << it << " tl=0 rl=" << rlvl << " c=" << comp;
                dset2 = input_file_2_ptr->openDataSet(dset2_name_ss.str());
            }

            catch (const FileIException &e1) {
                try {
                    ostringstream dset_input_file_2_name_ss_again;
                    dset2_name_ss_again << DATASET_BASENAME_2
                        << " it=" << it << " tl=0 m=" << map << " rl=" << rlvl << " c=" << comp;
                    dset2 = input_file_2_ptr->openDataSet(dset2_name_ss.str());
                }

                catch (const FileIException &e2) {
                    cout << e2.getDetailMsg() << endl;
                    return -1;  // Stop iterating over loc_id
                }
            }


            // Read the datasets
            const auto ndims1 = dset1.getSimpleExtentNdims();
            const auto ndims2 = dset2.getSimpleExtentNdims();

            if (ndims1 <= 0 or ndims2 <= 0 or ndims1 != ndims2) {
                cout << "Invalid or inconsistent dataset dimensions (ndims1 = "
                     << ndims1 << ", ndims2 = " << ndims2 << ")" << endl;
                return -1;  // Stop iterating over loc_id
            }


            vector<hsize_t> dims1(ndims1), dims2(ndims2);
            const auto ndims1_again = dset1.getSimpleExtentdims(dims1.data(), nullptr);
            const auto ndims2_again = dset2.getSimpleExtentdims(dims2.data(), nullptr);

            if (ndims1_again <= 0 or ndims2_again <= 0 or ndims1_again != ndims2_again) {
                cout << "Invalid or inconsistent dataset dimensions (ndims1_again = "
                     << ndims1_again << ", ndims2_again = " << ndims2_again << ")" << endl;
                return -1;  // Stop iterating over loc_id
            }


            const auto &ndims = ndims1;

            for (int n = 0; n < ndims; ++n) {
                const auto &dims1_n = dims1.at(n);
                const auto &dims2_n = dims2.at(n);
                if (dims1_n <= 0 or dims2_n <= 0 or dims1_n != dims2_n) {
                    cout << "Invalid or inconsistent dataset size along direction " << n
                         << " (dims1[" << n << "] = " << dims1_n
                         << ", (dims2[" << n << "] = " << dims2_n << ")" << endl;
                }
            }

            const auto &dims = dims1;


            const auto dset1_type = dset1.getTypeClass();
            const auto dset2_type = dset2.getTypeClass();

            if (dset1_type != H5T_FLOAT or dset2_type != H5T_FLOAT) {
                cout << "Dataset type is not double nor float (dset1_type = "
                     << dset1_type << ", dset2_type = " << dset2_type << ")" << endl;
            }


            auto dims_tot = dims.at(0);

            if (ndims > 1) {
                for (int n = 1; n < ndims; ++n) {
                    dims_tot *= dims.at(n);
                }
            }

            vector<double> buffer1(dims_tot), buffer2(dims_tot);

            dset1.read(buffer1, PredType::NATIVE_DOUBLE, H5S_ALL, H5S_ALL);
            dset2.read(buffer2, PredType::NATIVE_DOUBLE, H5S_ALL, H5S_ALL);


            // Modify dataset 1 using dataset 2
            const string operation = OPERATION;

            if (operation == "+") {
                for (int m = 0; m < dims_tot; ++m) {
                    buffer1.at(m) += buffer2.at(m);
                }
            } else if (operation == "-") {
                for (int m = 0; m < dims_tot; ++m) {
                    buffer1.at(m) += buffer2.at(m);
                }
            } else if (operation == "*") {
                for (int m = 0; m < dims_tot; ++m) {
                    buffer1.at(m) *= buffer2.at(m);
                }
            } else if (operation == "/") {
                for (int m = 0; m < dims_tot; ++m) {
                    if (buffer2.at(m) == 0.) {
                        cout << "Division by zero encountered" << endl;
                        return -1;  // Stop iterating over loc_id
                    }

                    buffer1.at(m) /= buffer2.at(m);
                }
            } else {
                cout << "Unrecognized operation '"
                     << operation << "'" << endl;
                return -1;  // Stop iterating over loc_id
            }

            return 0;  // Success, keep iterating over loc_id
        }


        else {
            cout << "String '" << object_name_str
                 << "' didn't match against regex '" << regex_ss.str()
                 << "'; is it a Cactus dataset at all?" << endl,
            return -1;  // Not reached, but it would stop iterating over loc_id
        }
    }


    catch (const regex_error &e) {
        cout << "Regex exception caught: " << e.what()
             << " (error code " << e.code() << ")" << endl;
        return -1;  // Not reached, but it would stop iterating over loc_id
    }
}
