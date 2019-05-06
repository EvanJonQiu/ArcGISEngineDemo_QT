
# Example Makefile for ArcEngine C++ Programming with Qt on Linux

#
# The PROGRAM macro defines the name of the program or project.  It
# allows the program name to be changed by editing in only one
# location
#

PROGRAM = GeoDataDesktop

#
# The INCLUDEDIRS macro contains a list of include directories
# to pass to the compiler so it can find necessary header files.
#
# The LIBDIRS macro contains a list of library directories
# to pass to the linker so it can find necessary libraries.
#
# The LIBS macro contains a list of libraries that the the
# executable must be linked against.
#

INCLUDEDIRS = \
	-I$(AGSDEVKITHOME)/include \
	-I/usr/X11R6/include \
	-I$(QTDIR)/include


LIBDIRS = \
	-L$(AGSENGINEJAVA)/bin \
	-L/usr/X11R6/lib \
	-L$(QTDIR)/lib

LIBS = \
	-larcsdk -lqtctl -laoctl \
	-lqt-mt

#
# The CXXSOURCES macro contains a list of source files.
#
# The CXXOBJECTS macro converts the CXXSOURCES macro into a list
# of object files.
#
# The CXXFLAGS macro contains a list of options to be passed to
# the compiler.  Adding "-g" to this line will cause the compiler
# to add debugging information to the executable.
#
# The CXX macro defines the C++ compiler.
#
# The LDFLAGS macro contains all of the library and library
# directory information to be passed to the linker.
#
MOC_HEADERS = MainWindow.h CreateGDBDlg.h TopologyReportDlg.h
MOC_SOURCES = $(MOC_HEADERS:.h=.moc.cpp)
MOC = moc
CXXSOURCES = main.cpp LicenseUtilities.cpp MainWindow.cpp PathUtilities.cpp \
	CreateGDBDlg.cpp TopologyReportDlg.cpp
CXXOBJECTS = $(CXXSOURCES:.cpp=.o) $(MOC_SOURCES:.cpp=.o)
CXXFLAGS = -DESRI_UNIX $(INCLUDEDIRS)
CXX = g++

LDFLAGS = $(LIBDIRS) $(LIBS)

#
# Default target: the first target is the default target.
# Just type "make -f Makefile.LinuxQt" to build it.
#

all: $(PROGRAM)

#
# Link target: automatically builds its object dependencies before
# executing its link command.
#

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) $(LDFLAGS)

#
# Object targets: rules that define objects, their dependencies, and
# a list of commands for compilation.
#
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.moc.cpp: %.h
	$(MOC) $< -o $@
#
# Clean target: "make -f Makefile.LinuxQt clean" to remove unwanted objects and executables.
#

clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)

#
# Run target: "make -f Makefile.LinuxQt run" to execute the application
#

run:
	./$(PROGRAM) *.out


