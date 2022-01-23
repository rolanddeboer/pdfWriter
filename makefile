    pdfWriter: src/main.cpp \
src/classes/DataReader.cpp src/classes/PdfMaker.cpp src/classes/EntryFormMaker.cpp \
src/headers/dataReader.h src/headers/pdfTypes.h src/headers/entryFormTypes.h
	g++ \
	-o pdfWriter src/main.cpp \
	-std=c++20 \
	-O2 \
	-I include/hummus \
	-L lib \
	-lPDFWriter \
	-lLibPng \
	-lFreeType \
	-lLibAesgm \
	-lZlib \
	-lLibJpeg \
	-Wall -Wextra