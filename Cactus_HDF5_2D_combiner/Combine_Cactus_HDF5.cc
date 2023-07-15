#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>

#include <H5Cpp.h>

#include "include/Macros.hh"
#include "include/Types.hh"
#include "include/Declarations.hh"
#include "Parameters.hh"

using namespace std;
using namespace H5;


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
             << " it=([0-9]+) tl=0(?: m=)?([0-9]+)? rl=([0-9]+) c=([0-9]+)";

    try {
        const regex rule(regex_ss.str());
        smatch matches;

        // Try to match the object name against the regex
        if (regex_match(object_name_str, matches, rule)) {
            assert(matches.size() == 5);
            const auto dset1_name = matches.str(0);
            const auto it         = matches.str(1);
            const auto map        = (matches[2].length() == 0) ? to_string(0) : matches.str(2);
            const auto rlvl       = matches.str(3);
            const auto comp       = matches.str(4);

            const auto dset1 = output_file_ptr->openDataSet(dset1_name);
            DataSet dset2;

            /* Try to open dataset 2 with the same iteration, map, refinement
             * level and component as dataset 1                                 */
            try {
                ostringstream dset2_name_ss;
                dset2_name_ss << DATASET_BASENAME_2
                    << " it=" << it << " tl=0 rl=" << rlvl << " c=" << comp;
                dset2 = input_file_2_ptr->openDataSet(dset2_name_ss.str());
            }

            catch (const FileIException &e1) {
                try {
                    ostringstream dset2_name_ss_again;
                    dset2_name_ss_again << DATASET_BASENAME_2
                        << " it=" << it << " tl=0 m=" << map << " rl=" << rlvl << " c=" << comp;
                    dset2 = input_file_2_ptr->openDataSet(dset2_name_ss_again.str());
                }

                catch (const FileIException &e2) {
                    cout << e2.getDetailMsg() << endl;
                    return -1;  // Stop iterating over loc_id
                }
            }


            // Read the datasets
            const auto dspace1 = dset1.getSpace();
            const auto dspace2 = dset2.getSpace();

            const auto ndims1 = dspace1.getSimpleExtentNdims();
            const auto ndims2 = dspace2.getSimpleExtentNdims();

            if (ndims1 <= 0 or ndims2 <= 0 or ndims1 != ndims2) {
                cout << "Invalid or inconsistent dataset dimensions (ndims1 = "
                     << ndims1 << ", ndims2 = " << ndims2 << ")" << endl;
                return -1;  // Stop iterating over loc_id
            }


            vector<hsize_t> dims1(ndims1), dims2(ndims2);
            const auto ndims1_again = dspace1.getSimpleExtentDims(dims1.data(), nullptr);
            const auto ndims2_again = dspace2.getSimpleExtentDims(dims2.data(), nullptr);

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


            // TODO: generalize to support ndims != 2
            if (ndims != 2) {
                cout << "Sorry, only 2D datasets are supported right now (ndims = "
                     << ndims << ")" << endl;
                return -1;  // Stop iterating over loc_id
            }

            double buffer1[dims.at(0)][dims.at(1)];
            double buffer2[dims.at(0)][dims.at(1)];

            dset1.read(buffer1, PredType::NATIVE_DOUBLE);
            dset2.read(buffer2, PredType::NATIVE_DOUBLE);


            /* Modify dataset 1 using dataset 2 and write the result back to
             * dataset 1                                                        */
            const string operation = OPERATION;

            if (operation == "linear combination") {
                for (hsize_t i = 0; i < dims.at(0); ++i) {
                    for (hsize_t j = 0; j < dims.at(1); ++j) {
                        auto &buffer1_ij = buffer1[i][j];
                        buffer1_ij = LINCOMB_A1*buffer1_ij + LINCOMB_A2*buffer2[i][j];
                    }
                }
            } else if (operation == "product") {
                for (hsize_t i = 0; i < dims.at(0); ++i) {
                    for (hsize_t j = 0; j < dims.at(1); ++j) {
                        auto &buffer1_ij = buffer1[i][j];
                        buffer1_ij *= buffer2[i][j];
                        buffer1_ij *= PROD_RATIO_FAC;
                    }
                }
            } else if (operation == "ratio") {
                for (hsize_t i = 0; i < dims.at(0); ++i) {
                    for (hsize_t j = 0; j < dims.at(1); ++j) {
                        if (buffer2[i][j] == 0.) {
                            cout << "Division by zero encountered" << endl;
                            return -1;  // Stop iterating over loc_id
                        }

                        auto &buffer1_ij = buffer1[i][j];
                        buffer1_ij /= buffer2[i][j];
                        buffer1_ij *= PROD_RATIO_FAC;
                    }
                }
            } else {
                cout << "Unrecognized operation '"
                     << operation << "'" << endl;
                return -1;  // Stop iterating over loc_id
            }


            dset1.write(buffer1, PredType::NATIVE_DOUBLE);

            #if (VERBOSE)
            cout << "Done processing dataset '"
                 << object_name_str << "'" << endl;
            #endif

            return 0;  // Success, keep iterating over loc_id
        }


        else {
            cout << "String '" << object_name_str
                 << "' didn't match against regex '" << regex_ss.str()
                 << "'; is it a Cactus dataset at all?" << endl;
            return -1;  // Not reached, but it would stop iterating over loc_id
        }
    }


    catch (const regex_error &e) {
        cout << "Regex exception caught: " << e.what()
             << " (error code " << e.code() << ")" << endl;
        return -1;  // Not reached, but it would stop iterating over loc_id
    }
}
