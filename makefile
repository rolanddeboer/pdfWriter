pdfWriter: src/main.cpp \
src/classes/DataReader.cpp src/classes/PdfMaker.cpp src/classes/EntryFormMaker.cpp \
src/headers/dataReader.h src/headers/pdfTypes.h src/headers/entryFormTypes.h
	g++ \
	-O2 \
	-L lib/hummus/build \
	-I lib/hummus \
	-I lib/hummus/FreeType/include \
	-o pdfWriter src/main.cpp \
	-l:PDFWriter/libPDFWriter.a \
	-l:LibPng/libLibPng.a \
	-l:FreeType/libFreeType.a \
	-l:LibAesgm/libLibAesgm.a \
	-l:ZLib/libZlib.a \
	-static-libstdc++ \
	-Wall -Wextra