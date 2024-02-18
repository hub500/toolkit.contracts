add_contract(xchain.proxy xchain.proxy ${CMAKE_CURRENT_SOURCE_DIR}/src/xchain.proxy.cpp)

if(DEFINED ENV{DAY_SECONDS_FOR_TEST})
   message(WARNING "ENV{DAY_SECONDS_FOR_TEST}=$ENV{DAY_SECONDS_FOR_TEST} should use only for test!!!")
   target_compile_definitions(xchain.proxy PUBLIC "DAY_SECONDS_FOR_TEST=$ENV{DAY_SECONDS_FOR_TEST}")
endif()


target_include_directories(xchain.proxy
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../xchain.common/include
   
)

set_target_properties(xchain.proxy
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/xchain.proxy.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/xchain.proxy.contracts.md @ONLY )

target_compile_options( xchain.proxy PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )