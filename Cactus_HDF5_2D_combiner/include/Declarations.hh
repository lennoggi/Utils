#ifndef DECLARATIONS_HH
#define DECARATIONS_HH

#include <hdf5.h>

herr_t combine_Cactus_HDF5(hid_t             loc_id,
                           const char       *object_name,
                           const H5L_info_t *info,
                           void             *op_data);

#endif  // DECLARATIONS_HH
