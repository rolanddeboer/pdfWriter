file(REMOVE_RECURSE
  "libFreeType.pdb"
  "libFreeType.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/FreeType.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
