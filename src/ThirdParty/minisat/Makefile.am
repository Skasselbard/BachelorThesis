noinst_LIBRARIES = libminisat.a

libminisat_a_SOURCES = \
	core/Dimacs.h \
	core/Solver.cc \
	core/Solver.h \
	core/SolverTypes.h \
	mtl/Alg.h \
	mtl/Alloc.h \
	mtl/IntTypes.h \
	mtl/Map.h \
	mtl\Queue.h \
	mtl/Sort.h \
	mtl/Vec.h \
	mtl/XAlloc.h \
	simp/Main.cc \
	simp/SimpSolver.cc \
	simp/SimpSolver.h \
	utils/Options.cc \
	utils/Options.h \
	utils/ParseUtils.h \
	utils/System.cc \
	utils/System.h
	

# -D NDEBUG is necessary to have the "right" exit codes (10/20)
libminisat_a_CXXFLAGS = -ffloat-store -D NDEBUG
