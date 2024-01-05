#ifndef PARAMETERS_HH
#define PARAMETERS_HH


/* Full paths to the input files to be processed
 * NOTE: INPUT_FILE_2 can be the same as INPUT_FILE_1, typically if dealing with
 *       checkpoint files                                                       */
//#define INPUT_FILE_1 "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_3D/smallb2.xyz.file_0.h5"
#define INPUT_FILE_1 "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/Checkpoint/checkpoint.chkpt.it_1468416.file_0.h5"

//#define INPUT_FILE_2 "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_3D/rho_b.xyz.file_0.h5"
//#define INPUT_FILE_2 "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_2D/P.xy.h5"
#define INPUT_FILE_2 "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/Checkpoint/checkpoint.chkpt.it_1468416.file_0.h5"


// Full path to the output file
//#define OUTPUT_FILE "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_3D/b2_over_rho.xyz.file_0.TEST.h5"
//#define OUTPUT_FILE "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_2D/b2_over_2P.xy.h5"
#define OUTPUT_FILE "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/Checkpoint/checkpoint.chkpt.it_1468416_Avec0Filled.file_0.h5"


/* Names of the datasets to be processed in each input file
 * NOTE: escape square brackets with a DOUBLE backslash (e.g.
 *       "HYDROBASE::Avec\\[0\\]"), otherwise regex matching doesn't work       */
//#define DATASET_BASENAME_1 "SMALLBPOYNET::smallb2"
#define DATASET_BASENAME_1 "HYDROBASE::Avec\\[0\\]"

//#define DATASET_BASENAME_2 "ILLINOISGRMHD::rho_b"
//#define DATASET_BASENAME_2 "ILLINOISGRMHD::P"
#define DATASET_BASENAME_2 "ILLINOISGRMHD::Ax"


/* New name for the datasets in the output file (usually having the same name as
 * the output file)
 * NOTE: DO NOT escape square brackets with any backslash (no regex matching
 *       involved)                                                              */
//#define DATASET_OUTPUT "ILLINOISGRMHD::b2_over_rho"
//#define DATASET_OUTPUT "ILLINOISGRMHD::b2_over_2P"
#define DATASET_OUTPUT "HYDROBASE::Avec[0]"


// Choose among "linear combination", "product", or "ratio"
//#define OPERATION "ratio"
#define OPERATION "linear combination"


/* Linear combination: LINCOMB_A1*dataset1 + LINCOMB_A2*dataset2
 * NOTE: replacing dataset1 with dataset2 can be achieved with the following:
 *       #define LINCOMB_A1 0.
 *       #define LINCOMB_A2 1.                                                  */
#define LINCOMB_A1 0.
#define LINCOMB_A2 1.


// Overall multiplicative factor when computing a product or ratio
//#define PROD_RATIO_FAC 1.
#define PROD_RATIO_FAC 0.5


/* Tip: just make the code verbose during debug, otherwise the execution is
 * slowed down                                                                  */
#define VERBOSE true


#endif  // PARAMETERS_HH
