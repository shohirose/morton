find_package(GTest REQUIRED)

add_executable(morton_test morton_test.cpp)
target_link_libraries(morton_test
  PRIVATE
    morton
    GTest::GTest
    GTest::Main
  )
add_test(
  NAME morton_test
  COMMAND morton_test
  )