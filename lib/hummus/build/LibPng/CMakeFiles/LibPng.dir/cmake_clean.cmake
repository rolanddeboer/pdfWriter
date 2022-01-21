file(REMOVE_RECURSE
  "libLibPng.pdb"
  "libLibPng.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/LibPng.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
