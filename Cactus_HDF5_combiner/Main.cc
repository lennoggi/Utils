#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <regex>
#include <chrono>

#include <H5Cpp.h>
#include <mpi.h>

#include "include/Macros.hh"
#include "include/Types.hh"
#include "include/Declarations.hh"
#include "Parameters.hh"


using namespace std;
using namespace H5;



int main(int argc, char **argv) {
    const auto total_start = chrono::high_resolution_clock::now();
    assert(MPI_Init(&argc, &argv) == MPI_SUCCESS);

    int rankid, nranks;
    assert(MPI_Comm_rank(MPI_COMM_WORLD, &rankid) == MPI_SUCCESS);  // Can't call CHECK_ERROR before knowing rankid
    CHECK_ERROR(rankid, MPI_Comm_size(MPI_COMM_WORLD, &nranks));
    assert(rankid < nranks);

    // Redirect stdout and stderr to rank-specific files if desired
    ostringstream new_stdout_ss;
    new_stdout_ss << "rank" << rankid << ".out";
    FILE* new_stdout;
    new_stdout = freopen(new_stdout_ss.str().c_str(), "w", stdout);
    assert(new_stdout != nullptr);

    ostringstream new_stderr_ss;
    new_stderr_ss << "rank" << rankid << ".err";
    FILE* new_stderr;
    new_stderr = freopen(new_stderr_ss.str().c_str(), "w", stderr);
    assert(new_stderr != nullptr);


    #if (MULTIPLE_FILES)
    const auto nfiles = MAX_FILE_INDEX - MIN_FILE_INDEX + 1;

    if (nranks > nfiles) {
        ERROR(rankid, "nranks (" << nranks << ") > nfiles (" << nfiles << ")");
        return 1;  // Not reached
    }

    if (nfiles % nranks != 0) {
        ERROR(rankid, "nfiles (" << nfiles << ") \\% nranks (" << nranks << ") = " << nfiles % nranks << " is non-zero");
        return 1;  // Not reached
    }

    const auto file_idx_chunk = nfiles/nranks;
    const auto min_file_idx   = rankid*file_idx_chunk;
    const auto max_file_idx   = (rankid + 1)*file_idx_chunk - 1;
    assert(max_file_idx >= min_file_idx);

    for (int file_idx = min_file_idx; file_idx <= max_file_idx; ++file_idx) {
        ostringstream infile1_ss, infile2_ss, outfile_ss;
        infile1_ss << INFILE1_BASENAME << ".file_" << file_idx << ".h5";
        infile2_ss << INFILE2_BASENAME << ".file_" << file_idx << ".h5";
        outfile_ss << OUTFILE_BASENAME << ".file_" << file_idx << ".h5";
    #else
        ostringstream infile1_ss, infile2_ss, outfile_ss;
        infile1_ss << INFILE1_BASENAME << ".h5";
        infile2_ss << INFILE2_BASENAME << ".h5";
        outfile_ss << OUTFILE_BASENAME << ".h5";
    #endif  // #if (MULTIPLE_FILES)

        const auto infile1_str = infile1_ss.str();
        const auto infile2_str = infile2_ss.str();
        const auto outfile_str = outfile_ss.str();


        /* Copy the first input file into a new file, so that the latter
         * inherits the full input file structure (datasets and their
         * attributes, metadata group) automatically                            */
        ostringstream cmd;
        cmd << "cp " << infile1_str << " " << outfile_str;
        INFO(rankid, "Copying '" << infile1_str << "' into '" << outfile_str << "'...");

        const auto copy_start = chrono::high_resolution_clock::now();
        system(cmd.str().c_str());
        const auto copy_end  = chrono::high_resolution_clock::now();
        const auto copy_time = chrono::duration_cast<chrono::seconds>(copy_end - copy_start).count();
        INFO(rankid, "Done, elapsed time: " << copy_time << "s");


        // Edit the output file using input file 2
        H5File outfile(outfile_str, H5F_ACC_RDWR);
        H5File infile2(infile2_str, H5F_ACC_RDONLY);

        stringstream indsets1_basenames_ss, indsets2_basenames_ss, outdsets_basenames_ss;
        indsets1_basenames_ss << INDSETS1_BASENAMES;
        indsets2_basenames_ss << INDSETS2_BASENAMES;
        outdsets_basenames_ss << OUTDSETS_BASENAMES;

        string dummy_str;
        vector<string> indsets1_basenames_vec, indsets2_basenames_vec, outdsets_basenames_vec;

        while (indsets1_basenames_ss >> dummy_str) {
            indsets1_basenames_vec.push_back(dummy_str);
        }

        while (indsets2_basenames_ss >> dummy_str) {
            indsets2_basenames_vec.push_back(dummy_str);
        }

        while (outdsets_basenames_ss >> dummy_str) {
            outdsets_basenames_vec.push_back(dummy_str);
        }

        const auto &ndsets = indsets1_basenames_vec.size();
        assert(ndsets > 0);

        if (indsets2_basenames_vec.size() != ndsets or
            outdsets_basenames_vec.size()  != ndsets) {
            ERROR(rankid, "Inconsistent number of input/output datasets provided:" << endl <<
                          "  INDSETS1_BASENAMES = '" << indsets1_basenames_ss.str() << "' has " << ndsets                        << " elements" << endl <<
                          "  INDSETS2_BASENAMES = '" << indsets2_basenames_ss.str() << "' has " << indsets2_basenames_vec.size() << " elements" << endl <<
                          "  OUTDSETS_BASENAMES = '" << outdsets_basenames_ss.str() << "' has " << outdsets_basenames_vec.size() << " elements");
            return 1;  // Not reached
        }

        vector<vector<string>> dset_rename_lists(ndsets);

        const op_data_t op_data = {
            &outfile, &infile2, ndsets,
            &indsets1_basenames_vec,
            &indsets2_basenames_vec,
            &outdsets_basenames_vec,
            &dset_rename_lists
        };

        INFO(rankid, "Editing datasets in output file '" << outfile_str << "'...");
        const auto iterate_start = chrono::high_resolution_clock::now();

        const auto outfile_id = outfile.getId();
        CHECK_ERROR(rankid, H5Literate(outfile_id, H5_INDEX_NAME, H5_ITER_NATIVE,
                                       nullptr, combine_Cactus_HDF5, (void*) &op_data));

        const auto iterate_end  = chrono::high_resolution_clock::now();
        const auto iterate_time = chrono::duration_cast<chrono::seconds>(iterate_end - iterate_start).count();
        INFO(rankid, "Done, elapsed time: " << iterate_time << "s");


        // Rename all datasets in the output file
        INFO(rankid, "Renaming datasets in output file '" << outfile_str << "'...");
        const auto rename_start = chrono::high_resolution_clock::now();

        for (auto dset_idx = decltype(ndsets){0}; dset_idx < ndsets; ++dset_idx) {
            for (const auto &dset_name : dset_rename_lists.at(dset_idx)) {
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
                        new_dset_name_ss << outdsets_basenames_vec.at(dset_idx)
                                         << " it=" << it << " tl=" << tl;

                        if ((matches[3].length() != 0)) {
                            new_dset_name_ss << " m=" << m;
                        }

                        new_dset_name_ss << " rl=" << rl << " c=" << c;
                        const auto new_dset_name_str = new_dset_name_ss.str();

                        /* Only rename if the new name is actually different
                         * from the old one, otherwise H5Lmove fails            */
                        if (dset_name != new_dset_name_str) {
                            #if (VERBOSE)
                            INFO(rankid, "Changing '" << dset_name <<
                                         "' into '"   << new_dset_name_str << "'");
                            #endif
                            CHECK_ERROR(rankid,H5Lmove(outfile_id, dset_name.c_str(),
                                                       outfile_id, new_dset_name_str.c_str(),
                                                       H5P_DEFAULT, H5P_DEFAULT));
                        } else {
                            WARNING(rankid, "Same names '" << dset_name << "' and '" << new_dset_name_str <<
                                            "', not going to rename the dataset");
                        }
                    }

                    else {
                        ERROR(rankid, "Dataset name '" << dset_name <<
                                      "' didn't match against regex");
                        return 1;  // Not reached
                    }
                }

                catch (const regex_error &e) {
                    ERROR(rankid, "Regex exception caught: " << e.what() <<
                                  " (error code " << e.code() << ")");
                    return 1;  // Not reached
                }
            }
        }

        const auto rename_end  = chrono::high_resolution_clock::now();
        const auto rename_time = chrono::duration_cast<chrono::seconds>(rename_end - rename_start).count();

        INFO(rankid, "Done, elapsed time: " << rename_time << "s");
    #if (MULTIPLE_FILES)
    }
    #endif

    const auto total_end  = chrono::high_resolution_clock::now();
    const auto total_time = chrono::duration_cast<chrono::seconds>(total_end - total_start).count();
    INFO(rankid, "All done, total elapsed time: " << total_time << "s");

    CHECK_ERROR(rankid, MPI_Finalize());
    return 0;
}
