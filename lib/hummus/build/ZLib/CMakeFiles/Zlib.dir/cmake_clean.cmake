file(REMOVE_RECURSE
  "libZlib.pdb"
  "libZlib.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/Zlib.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
