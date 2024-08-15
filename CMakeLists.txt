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
  URL https://github.com/galkahana/PDF-Writer/archive/refs/tags/v4.6.6.tar.gz
  URL_HASH SHA256=8343820313e553052df68c75fe2bf35353da2719106e81eb2a8b026ff96c7d7c
)

FetchContent_MakeAvailable(PDFHummus)

include_directories(include)

add_executable(pdfWriter src/main.cpp)

target_link_libraries(pdfWriter PDFHummus::PDFWriter)

install(TARGETS pdfWriter DESTINATION ~/bin COMPONENT pdfWriter)