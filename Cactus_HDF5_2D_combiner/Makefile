# Include and parse the option list, but don't do that when running 'make clean'
ifeq (, ${filter clean, ${MAKECMDGOALS}})
	include ${options}

	# Disable optimization if so requested by the user
	ifeq (${OPTIMIZE}, no)
		CXX_OPTIMIZE_FLAGS =
	else ifneq (${OPTIMIZE}, yes)
		$(error Set option OPTIMIZE to either yes or no in the optionlist (current setting: ${OPTIMIZE}))
	endif

	# Disable warnings if so requested by the user
	ifeq (${WARN}, no)
		CXX_WARN_FLAGS =
	else ifneq (${WARN}, yes)
		$(error Set option WARN to either yes or no in the optionlist (current setting: ${WARN}))
	endif

	# Disable debug mode if so requested by the user
	ifeq (${DEBUG}, no)
		CXX_DEBUG_FLAGS =
	else ifneq (${DEBUG}, yes)
		$(error Set option DEBUG to either yes or no in the optionlist (current setting: ${DEBUG}))
	endif
endif


# icpx generates *.o.tmp files which should be removed when cleaning
ifeq (${CXX}, icpx)
	ICPX_OTMP = *.o.tmp
else
	ICPX_OTMP =
endif


# Objects to be built
OBJ = Main.o Combine_Cactus_HDF5.o
EXE = Cactus_HDF5_2D_combiner_exe


# Build all targets
build: ${OBJ}
	${CXX} -o ${EXE} ${OBJ} ${CXXFLAGS} ${CXX_OPTIMIZE_FLAGS} ${CXX_WARN_FLAGS} ${CXX_DEBUG_FLAGS} ${LDFLAGS} -I${HDF5_INC_DIR} -L${HDF5_LIBS_DIR} ${HDF5_LIBS}

Main.o: Main.cc Parameters.hh include/Declarations.hh include/Macros.hh include/Types.hh
	${CXX} -c Main.cc ${CXXFLAGS} ${CXX_OPTIMIZE_FLAGS} ${CXX_WARN_FLAGS} ${CXX_DEBUG_FLAGS} ${LDFLAGS} -I${HDF5_INC_DIR}

Combine_Cactus_HDF5.o: Combine_Cactus_HDF5.cc Parameters.hh include/Declarations.hh include/Macros.hh include/Types.hh
	${CXX} -c Combine_Cactus_HDF5.cc ${CXXFLAGS} ${CXX_OPTIMIZE_FLAGS} ${CXX_WARN_FLAGS} ${CXX_DEBUG_FLAGS} ${LDFLAGS} -I${HDF5_INC_DIR}


# Remove the executable and all object files
.PHONY : clean
clean:
	${RM} ${EXE} ${OBJ} ${ICPX_OTMP}
