file(REMOVE_RECURSE
  "libPDFWriter.pdb"
  "libPDFWriter.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/PDFWriter.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
