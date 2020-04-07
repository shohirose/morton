find_package(GTest REQUIRED)

function(add_unit_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name}
    PRIVATE
      morton
      GTest::GTest
      GTest::Main
    )
  add_test(
    NAME ${name}
    COMMAND ${name}
    )
endfunction()

add_unit_test(morton2d_test)
add_unit_test(morton3d_test)