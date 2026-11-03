# Keep fixes to the fetched dependency reproducible across clean builds.
# Accept an already patched source, but fail if the upstream code has changed.
function(googletest_replace relative_path original replacement)
  set(source "${googletest_SOURCE_DIR}/${relative_path}")
  file(READ "${source}" contents)
  string(FIND "${contents}" "${original}" original_position)
  if(NOT original_position EQUAL -1)
    string(REPLACE "${original}" "${replacement}" contents "${contents}")
    file(WRITE "${source}" "${contents}")
  else()
    string(FIND "${contents}" "${replacement}" replacement_position)
    if(replacement_position EQUAL -1)
      message(FATAL_ERROR "GoogleTest fix no longer applies to ${relative_path}")
    endif()
  endif()
endfunction()

# MISRAC2012-DIR_4_1-j: use equality for traversal within a string and
# remember whether a wildcard permits retrying, without ordering pointers.
googletest_replace(googletest/src/gtest.cc
  "  const char* const name_begin = name;"
  "  bool has_wildcard = false;")
googletest_replace(googletest/src/gtest.cc
  "pattern < pattern_end" "pattern != pattern_end")
googletest_replace(googletest/src/gtest.cc
  "name < name_end" "name != name_end")
googletest_replace(googletest/src/gtest.cc
  "          name_next = name + 1;"
  "          name_next = name;\n          has_wildcard = true;")
googletest_replace(googletest/src/gtest.cc
  "    if (name_begin < name_next && name_next <= name_end) {\n      pattern = pattern_next;"
  "    if (has_wildcard && name_next != name_end) {\n      ++name_next;\n      pattern = pattern_next;")

# Shared ownership provides an ordering without comparing unrelated addresses.
googletest_replace(googlemock/include/gmock/gmock-spec-builders.h
  "return lhs.expectation_base_.get() < rhs.expectation_base_.get();"
  "return lhs.expectation_base_.owner_before(rhs.expectation_base_);")
