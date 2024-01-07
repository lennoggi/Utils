#ifndef PARAMETERS_HH
#define PARAMETERS_HH


/* If true, ".file_<xxx>.h5" will be appended to INFILE1_BASENAME,
 * INFILE2_BASENAME and OUTFILE_BASENAME (see below), where <xxx> is every
 * number between MIN_FILE_INDEX and MAX_FILE_INDEX (see below); the code will
 * then loop over and process all of the <xxx> files. Otherwise, just ".h5" will
 * be appended to INFILE1_BASENAME, INFILE2_BASENAME and OUTFILE_BASENAME and a
 * single file will be processed                                                */
// TODO: consider reading the 'nioprocs' attribute to find the number of files
#define MULTIPLE_FILES true
#define MIN_FILE_INDEX 0
#define MAX_FILE_INDEX 511

static_assert(MULTIPLE_FILES or not MULTIPLE_FILES);
static_assert(MIN_FILE_INDEX >= 0);
static_assert(MAX_FILE_INDEX >= MIN_FILE_INDEX);  // MAX_FILE_INDEX == MIN_FILE_INDEX is supported


/* Basenames of the input files to be processed
 * NOTE: INFILE2_BASENAME can be the same as INFILE1_BASENAME, typically if
 *       idealing with checkpoint files                                         */
//#define INFILE1_BASENAME "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_2D/smallb2.xy"
#define INFILE1_BASENAME "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/Checkpoint/checkpoint.chkpt.it_1468416"

//#define INFILE2_BASENAME "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_2D/rho_b.xy"
//#define INFILE2_BASENAME "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_2D/P.xy"
#define INFILE2_BASENAME "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/Checkpoint/checkpoint.chkpt.it_1468416"


// Basename of the output file
//#define OUTFILE_BASENAME "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_2D/b2_over_rho.xy"
//#define OUTFILE_BASENAME "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/output-0021/HDF5_2D/b2_over_2P.xy"
#define OUTFILE_BASENAME "/scratch3/07825/lennoggi/BBH_handoff_McLachlan_pp08/Checkpoint/checkpoint.chkpt.it_1468416_AphiAvecFilled_MPI"


/* Basenames of the datasets to be processed in each input file, separated by a
 * blank space
 * NOTE: escape square brackets with a DOUBLE backslash (e.g.
 *       "HYDROBASE::Avec\\[0\\]"), otherwise regex matching doesn't work       */
//#define INDSETS1_BASENAMES "SMALLBPOYNET::smallb2"
#define INDSETS1_BASENAMES "HYDROBASE::Aphi HYDROBASE::Avec\\[0\\] HYDROBASE::Avec\\[1\\] HYDROBASE::Avec\\[2\\]"

//#define INDSETS2_BASENAMES "ILLINOISGRMHD::rho_b"
//#define INDSETS2_BASENAMES "ILLINOISGRMHD::P"
#define INDSETS2_BASENAMES "ILLINOISGRMHD::psi6phi ILLINOISGRMHD::Ax ILLINOISGRMHD::Ay ILLINOISGRMHD::Az"


/* New basenames for the datasets in the output file
 * NOTE: DO NOT escape square brackets with any backslash (no regex matching
 *       involved)                                                              */
//#define OUTDSETS_BASENAMES "ILLINOISGRMHD::b2_over_rho"
//#define OUTDSETS_BASENAMES "ILLINOISGRMHD::b2_over_2P"
#define OUTDSETS_BASENAMES "HYDROBASE::Aphi HYDROBASE::Avec[0] HYDROBASE::Avec[1] HYDROBASE::Avec[2]"


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
