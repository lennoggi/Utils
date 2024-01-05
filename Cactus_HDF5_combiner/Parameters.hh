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
#define OUTPUT_FILE "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/Checkpoint/checkpoint.chkpt.it_1468416_AphiAvecFilled.file_0.h5"


/* Basenames of the datasets to be processed in each input file, separated by a
 * blank space
 * NOTE: escape square brackets with a DOUBLE backslash (e.g.
 *       "HYDROBASE::Avec\\[0\\]"), otherwise regex matching doesn't work       */
//#define DATASETS_BASENAMES_IN_1 "SMALLBPOYNET::smallb2"
#define DATASETS_BASENAMES_IN_1 "HYDROBASE::Aphi HYDROBASE::Avec\\[0\\] HYDROBASE::Avec\\[1\\] HYDROBASE::Avec\\[2\\]"

//#define DATASETS_BASENAMES_IN_2 "ILLINOISGRMHD::rho_b"
//#define DATASETS_BASENAMES_IN_2 "ILLINOISGRMHD::P"
#define DATASETS_BASENAMES_IN_2 "ILLINOISGRMHD::psi6phi ILLINOISGRMHD::Ax ILLINOISGRMHD::Ay ILLINOISGRMHD::Az"


/* New basenames for the datasets in the output file
 * NOTE: DO NOT escape square brackets with any backslash (no regex matching
 *       involved)                                                              */
//#define DATASETS_BASENAMES_OUT "ILLINOISGRMHD::b2_over_rho"
//#define DATASETS_BASENAMES_OUT "ILLINOISGRMHD::b2_over_2P"
#define DATASETS_BASENAMES_OUT "HYDROBASE::Aphi HYDROBASE::Avec[0] HYDROBASE::Avec[1] HYDROBASE::Avec[2]"


// Choose among "linear combination", "product", or "ratio"
//#define OPERATION "ratio"
#define OPERATION "linear combination"


/* Linear combination: LINCOMB_A1*dataset1 + LINCOMB_A2*dataset2
 * NOTE: replacing dataset 1 with dataset 2 can be achieved with the following:
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
