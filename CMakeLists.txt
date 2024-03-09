cmake_minimum_required(VERSION 3.10)
project(pdfWriter VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PDFHUMMUS_NO_DCT  "Whether to drop support for DCT streams parsing (will not use LibJpeg)" True)
set(PDFHUMMUS_NO_TIFF "Whether to drop TIFF Images support (will not use LibTiff)" True)

include(FetchContent)

FetchContent_Declare(
  PDFHummus
  DOWNLOAD_EXTRACT_TIMESTAMP FALSE
  FIND_PACKAGE_ARGS
  URL https://github.com/galkahana/PDF-Writer/archive/refs/tags/v4.6.2.tar.gz
  URL_HASH SHA256=0a36815ccc9d207028567f90039785c824b211169ba5da68de84d0c15455ab62
)

FetchContent_MakeAvailable(PDFHummus)

include_directories(include)

add_executable(pdfWriter src/main.cpp)

target_link_libraries(pdfWriter PDFHummus::PDFWriter)
