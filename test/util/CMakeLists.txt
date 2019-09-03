add_executable(gtest_util main.cpp
  gtest_command.cpp
  gtest_currency.cpp
  gtest_lru_cache.cpp
  gtest_safe_map.cpp
  gtest_singleton.cpp
  gtest_persistent_type.cpp
  )

target_link_libraries(gtest_util nbre_rt ${gtest_lib})
