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
    const auto &output_file_ptr              = ((op_data_t*) op_data)->output_file_ptr;
    const auto &input_file_2_ptr             = ((op_data_t*) op_data)->input_file_2_ptr;
    const auto &ndsets                       = ((op_data_t*) op_data)->ndsets;
    const auto &dsets_basenames_in_1_vec_ptr = ((op_data_t*) op_data)->dsets_basenames_in_1_vec_ptr;
    const auto &dsets_basenames_in_2_vec_ptr = ((op_data_t*) op_data)->dsets_basenames_in_2_vec_ptr;
    const auto &dsets_basenames_out_vec_ptr  = ((op_data_t*) op_data)->dsets_basenames_out_vec_ptr;
          auto &dset_rename_lists_ptr        = ((op_data_t*) op_data)->dset_rename_lists_ptr;

    const string object_name_str = object_name;

    // Erase and rewrite the dataset telling which datasets are in the file
    /* TODO: this doesn't seem necessary for any relevant purpose, plus
     *       checkpoint files don't always list all of the actual datasets they
     *       contain                                                            */
    /*if (object_name_str == "Parameters and Global Attributes") {
        const auto metadata_group = output_file_ptr->openGroup(object_name_str);
        metadata_group.unlink("Datasets");
        const StrType str_type(0, out_dset_name_size);
        const auto new_dset = metadata_group.createDataSet("Datasets", str_type, DataSpace(H5S_SCALAR));
        new_dset.write(out_dset_name, str_type);
        return 0;  // Keep on iterating over loc_id
    }*/

    for (auto n = decltype(ndsets){0}; n < ndsets; ++n) {
        ostringstream regex_ss;
        regex_ss << dsets_basenames_in_1_vec_ptr->at(n)
                 << " it=([0-9]+) tl=([0-9]+)(?: m=)?([0-9]+)? rl=([0-9]+) c=([0-9]+)";

        try {
            const regex rule(regex_ss.str());
            smatch matches;

            // Try to match the object name against the regex
            if (regex_match(object_name_str, matches, rule)) {
                assert(matches.size() == 6);
                const auto dset1_name = matches.str(0);
                const auto it         = matches.str(1);
                const auto tl         = matches.str(2);
                const auto m          = (matches[3].length() == 0) ? to_string(0) : matches.str(3);
                const auto rl         = matches.str(4);
                const auto c          = matches.str(5);

                const auto dset1 = output_file_ptr->openDataSet(dset1_name);
                DataSet dset2;

                /* Add the name of the current dataset to the appropriate list
                 * of datasets in the output file to be renamed                 */
                dset_rename_lists_ptr->at(n).push_back(object_name_str);

                /* Try to open dataset 2 with the same iteration, map,
                 * refinement level and component as dataset 1                  */
                try {
                    ostringstream dset2_name_ss;
                    dset2_name_ss << dsets_basenames_in_2_vec_ptr->at(n)
                                  << " it=" << it << " tl=" << tl << " rl=" << rl << " c=" << c;
                    dset2 = input_file_2_ptr->openDataSet(dset2_name_ss.str());
                }

                catch (const FileIException &e1) {
                    try {
                        ostringstream dset2_name_ss_again;
                        dset2_name_ss_again << dsets_basenames_in_2_vec_ptr->at(n)
                            << " it=" << it << " tl=" << tl << " m=" << m << " rl=" << rl << " c=" << c;
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

                for (int d = 0; d < ndims; ++d) {
                    const auto &dims1_d = dims1.at(d);
                    const auto &dims2_d = dims2.at(d);
                    if (dims1_d <= 0 or dims2_d <= 0 or dims1_d != dims2_d) {
                        cout << "Invalid or inconsistent dataset size along direction " << d
                             << " (dims1[" << d << "] = " << dims1_d
                             << ", (dims2[" << d << "] = " << dims2_d << ")" << endl;
                    }
                }

                const auto &dims = dims1;


                const auto dset1_type = dset1.getTypeClass();
                const auto dset2_type = dset2.getTypeClass();

                if (dset1_type != H5T_FLOAT or dset2_type != H5T_FLOAT) {
                    cout << "Dataset type is not double nor float (dset1_type = "
                         << dset1_type << ", dset2_type = " << dset2_type << ")" << endl;
                }


                /* Modify dataset 1 using dataset 2 and write the result back
                 * to dataset 1                                                 */
                const string operation = OPERATION;

                if (ndims == 2) {
                    /* NOTE: order is y/x, not x/y
                     * NOTE: may be xy, xz or yz                                */
                    const auto &nx   = dims.at(1);
                    const auto &ny   = dims.at(0);
                    const auto  ntot = ny*nx;
                    vector<double> buf1(ntot), buf2(ntot);

                    dset1.read(buf1.data(), PredType::NATIVE_DOUBLE);
                    dset2.read(buf2.data(), PredType::NATIVE_DOUBLE);

                    for (auto j = decltype(ny){0}; j < ny; ++j) {
                        const auto j_idx = nx*j;
                        for (auto i = decltype(nx){0}; i < nx; ++i) {
                            const auto  ij      = i + j_idx;
                                  auto &buf1_ij = buf1.at(ij);
                            const auto &buf2_ij = buf2.at(ij);

                            if (operation == "linear combination") {
                                buf1_ij = LINCOMB_A1*buf1_ij + LINCOMB_A2*buf2_ij;
                            } else if (operation == "product") {
                                buf1_ij *= buf2_ij;
                                buf1_ij *= PROD_RATIO_FAC;
                            } else if (operation == "ratio") {
                                buf1_ij /= buf2_ij;
                                buf1_ij *= PROD_RATIO_FAC;
                            } else {
                                cout << "Unrecognized operation '"
                                     << operation << "'" << endl;
                                return -1;  // Stop iterating over loc_id
                            }
                        }
                    }

                    dset1.write(buf1.data(), PredType::NATIVE_DOUBLE);
                }

                else if (ndims == 3) {
                    // NOTE: order is z/y/x, not x/y/z
                    const auto &nx   = dims.at(2);
                    const auto &ny   = dims.at(1);
                    const auto &nz   = dims.at(0);
                    const auto  ntot = nz*ny*nx;
                    vector<double> buf1(ntot), buf2(ntot);

                    dset1.read(buf1.data(), PredType::NATIVE_DOUBLE);
                    dset2.read(buf2.data(), PredType::NATIVE_DOUBLE);

                    for (auto k = decltype(nz){0}; k < nz; ++k) {
                        const auto k_idx = ny*k;
                        for (auto j = decltype(ny){0}; j < ny; ++j) {
                            const auto jk_idx = nx*(j + k_idx);
                            for (auto i = decltype(nx){0}; i < nx; ++i) {
                                const auto  ijk      = i + jk_idx;
                                      auto &buf1_ijk = buf1.at(ijk);
                                const auto &buf2_ijk = buf2.at(ijk);

                                if (operation == "linear combination") {
                                    buf1_ijk = LINCOMB_A1*buf1_ijk + LINCOMB_A2*buf2_ijk;
                                } else if (operation == "product") {
                                    buf1_ijk *= buf2_ijk;
                                    buf1_ijk *= PROD_RATIO_FAC;
                                } else if (operation == "ratio") {
                                    buf1_ijk /= buf2_ijk;
                                    buf1_ijk *= PROD_RATIO_FAC;
                                } else {
                                    cout << "Unrecognized operation '"
                                         << operation << "'" << endl;
                                    return -1;  // Stop iterating over loc_id
                                }
                            }
                        }
                    }

                    dset1.write(buf1.data(), PredType::NATIVE_DOUBLE);
                }

                else {
                    cout << "Only 2D and 3D datasets are supported ndims = "
                         << ndims << ")" << endl;
                    return -1;  // Stop iterating over loc_id
                }


                // Erase and rewrite the 'name' attribute in dataset 1
                dset1.removeAttr("name");
                const StrType str_type(0, dsets_basenames_out_vec_ptr->at(n).size());
                const auto new_attr = dset1.createAttribute("name", str_type, DataSpace(H5S_SCALAR));
                new_attr.write(str_type, dsets_basenames_out_vec_ptr->at(n));

                #if (VERBOSE)
                cout << "Done processing dataset '"
                     << object_name_str << "'" << endl;
                #endif
            }


            else {
                #if (VERBOSE)
                cout << "WARNING: string '" << object_name_str
                     << "' didn't match against regex '"
                     << regex_ss.str() << "', proceeding" << endl;
                #endif
            }
        }


        catch (const regex_error &e) {
            cout << "Regex exception caught: " << e.what()
                 << " (error code " << e.code() << ")" << endl;
            return -1;  // Not reached, but it would stop iterating over loc_id
        }
    }

    return 0;  // Keep on iterating over loc_id
}
