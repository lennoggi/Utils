#ifndef MACROS_HH
#define MACROS_HH


// Macro checking for errors coming from routines returning error codes
#define CHECK_ERROR(routine)                     \
do {                                             \
    const int err = routine;                     \
    if (err < 0) {                               \
        cout << "Routine '" << #routine          \
             << "' returned error code " << err  \
             << endl;                            \
        std::exit(EXIT_FAILURE);                 \
    }                                            \
} while (0)


#endif  // MACROS_HH
