Author : Luc Legeard


Information
-----------

This package MFMlib is a complete  C++ library to decode or encode MFM frame.
It generates a libMFM.a which can be linked to your own code
A usefull executable "MFMtest.exe"  can test you MFM run files or generate MFM frames in a file with random data

Build and Installation
-----------------------

cd build ( if build isn't created , make it! (in this case, beside source directory))
cmake -DCMAKE_INSTALL_PREFIX=../ ../sources/
make
make install



If tinyxml library is not installed
MFM can be compiled without tinyxml 
cmake -DNO_MFMNXML=YES -DCMAKE_INSTALL_PREFIX=../ ../sources/
  
  
or download tynixml from  http://www.grinninglizard.com/tinyxml/.
In tinyxml directory
edit Makefile and for adding -fPIC option change lines 
-----------------------------------------------------------
# Rules for compiling source files to object files
%.o : %.cpp
	${CXX}  -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC}  -c ${CFLAGS} ${INCS} $< -o $@
----------------------------------------------------------------
to
-----------------------------------------------------------
# Rules for compiling source files to object files
%.o : %.cpp
	${CXX} -fPIC -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC} -fPIC -c ${CFLAGS} ${INCS} $< -o $@
	
lib:	tinyxml.o tinyxmlparser.o tinyxmlerror.o tinystr.o
	${CC} -shared -fPIC ${CFLAGS} ${INCS} tinyxml.o tinyxmlparser.o tinyxmlerror.o tinystr.o -o libtinyxml.so
----------------------------------------------------------------


compile with a "make lib" tynixml  
to produce tynixml.so

The new cmake command become
cmake -DMYTINYXMLDIR=/the/directory/where/libtinyxml.so/is/  -DCMAKE_INSTALL_PREFIX=../ ../sources/
  
Usage
-----
 
 usage "MFMtest.exe --help"       for help
 
 
           

 
         



