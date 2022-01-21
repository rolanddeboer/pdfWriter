file(REMOVE_RECURSE
  "libLibAesgm.pdb"
  "libLibAesgm.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/LibAesgm.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
