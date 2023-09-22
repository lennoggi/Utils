#ifndef PARAMETERS_HH
#define PARAMETERS_HH


// Full paths to the input files to be processed
#define INPUT_FILE_1 "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0000/HDF5_2D/smallb2.xz.h5"
#define INPUT_FILE_2 "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0000/HDF5_2D/rho_b.xz.h5"
//define INPUT_FILE_2 "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0000/HDF5_2D/P.xz.h5"


// Full path to the output file
#define OUTPUT_FILE "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0000/HDF5_2D/b2_over_rho.xz.h5"
//#define OUTPUT_FILE "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0000/HDF5_2D/b2_over_2P.xz.h5"


// Names of the datasets to be processed in each input file
#define DATASET_BASENAME_1 "SMALLBPOYNET::smallb2"
#define DATASET_BASENAME_2 "ILLINOISGRMHD::rho_b"
//#define DATASET_BASENAME_2 "ILLINOISGRMHD::P"


/* New name for the datasets in the output file (usually having the same name as
 * the output file)                                                             */
#define DATASET_OUTPUT "ILLINOISGRMHD::b2_over_rho"


// Choose among "linear combination", "product", or "ratio"
#define OPERATION "ratio"


// Linear combination: LINCOMB_A1*dataset1 + LINCOMB_A2*dataset2
#define LINCOMB_A1 1.
#define LINCOMB_A2 1.


// Overall multiplicative factor when computing a product or ratio
#define PROD_RATIO_FAC 1.
//#define PROD_RATIO_FAC 0.5


/* Tip: just make the code verbose during debug, otherwise the execution is
 * slowed down                                                                  */
#define VERBOSE false


#endif  // PARAMETERS_HH
