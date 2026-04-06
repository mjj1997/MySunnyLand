include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)


macro(MySunnyLand_supports_sanitizers)
  # Emscripten doesn't support sanitizers
  if(EMSCRIPTEN)
    set(SUPPORTS_UBSAN OFF)
    set(SUPPORTS_ASAN OFF)
  elseif((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)

    message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)

    if(HAS_UBSAN_LINK_SUPPORT)
      message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
      set(SUPPORTS_UBSAN ON)
    else()
      message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if (NOT WIN32)
      message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(MySunnyLand_setup_options)
  option(MySunnyLand_ENABLE_HARDENING "Enable hardening" ON)
  option(MySunnyLand_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    MySunnyLand_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    MySunnyLand_ENABLE_HARDENING
    OFF)

  MySunnyLand_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR MySunnyLand_PACKAGING_MAINTAINER_MODE)
    option(MySunnyLand_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(MySunnyLand_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(MySunnyLand_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(MySunnyLand_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(MySunnyLand_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(MySunnyLand_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(MySunnyLand_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(MySunnyLand_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(MySunnyLand_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(MySunnyLand_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(MySunnyLand_ENABLE_PCH "Enable precompiled headers" OFF)
    option(MySunnyLand_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(MySunnyLand_ENABLE_IPO "Enable IPO/LTO" ON)
    option(MySunnyLand_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(MySunnyLand_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(MySunnyLand_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(MySunnyLand_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(MySunnyLand_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(MySunnyLand_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(MySunnyLand_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(MySunnyLand_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(MySunnyLand_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(MySunnyLand_ENABLE_PCH "Enable precompiled headers" OFF)
    option(MySunnyLand_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      MySunnyLand_ENABLE_IPO
      MySunnyLand_WARNINGS_AS_ERRORS
      MySunnyLand_ENABLE_SANITIZER_ADDRESS
      MySunnyLand_ENABLE_SANITIZER_LEAK
      MySunnyLand_ENABLE_SANITIZER_UNDEFINED
      MySunnyLand_ENABLE_SANITIZER_THREAD
      MySunnyLand_ENABLE_SANITIZER_MEMORY
      MySunnyLand_ENABLE_UNITY_BUILD
      MySunnyLand_ENABLE_CLANG_TIDY
      MySunnyLand_ENABLE_CPPCHECK
      MySunnyLand_ENABLE_COVERAGE
      MySunnyLand_ENABLE_PCH
      MySunnyLand_ENABLE_CACHE)
  endif()

  MySunnyLand_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (MySunnyLand_ENABLE_SANITIZER_ADDRESS OR MySunnyLand_ENABLE_SANITIZER_THREAD OR MySunnyLand_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(MySunnyLand_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(MySunnyLand_global_options)
  if(MySunnyLand_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    MySunnyLand_enable_ipo()
  endif()

  MySunnyLand_supports_sanitizers()

  if(MySunnyLand_ENABLE_HARDENING AND MySunnyLand_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR MySunnyLand_ENABLE_SANITIZER_UNDEFINED
       OR MySunnyLand_ENABLE_SANITIZER_ADDRESS
       OR MySunnyLand_ENABLE_SANITIZER_THREAD
       OR MySunnyLand_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${MySunnyLand_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${MySunnyLand_ENABLE_SANITIZER_UNDEFINED}")
    MySunnyLand_enable_hardening(MySunnyLand_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(MySunnyLand_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(MySunnyLand_warnings INTERFACE)
  add_library(MySunnyLand_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  MySunnyLand_set_project_warnings(
    MySunnyLand_warnings
    ${MySunnyLand_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  include(cmake/Linker.cmake)
  # Must configure each target with linker options, we're avoiding setting it globally for now

  if(NOT EMSCRIPTEN)
    include(cmake/Sanitizers.cmake)
    MySunnyLand_enable_sanitizers(
      MySunnyLand_options
      ${MySunnyLand_ENABLE_SANITIZER_ADDRESS}
      ${MySunnyLand_ENABLE_SANITIZER_LEAK}
      ${MySunnyLand_ENABLE_SANITIZER_UNDEFINED}
      ${MySunnyLand_ENABLE_SANITIZER_THREAD}
      ${MySunnyLand_ENABLE_SANITIZER_MEMORY})
  endif()

  set_target_properties(MySunnyLand_options PROPERTIES UNITY_BUILD ${MySunnyLand_ENABLE_UNITY_BUILD})

  if(MySunnyLand_ENABLE_PCH)
    target_precompile_headers(
      MySunnyLand_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(MySunnyLand_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    MySunnyLand_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(MySunnyLand_ENABLE_CLANG_TIDY)
    MySunnyLand_enable_clang_tidy(MySunnyLand_options ${MySunnyLand_WARNINGS_AS_ERRORS})
  endif()

  if(MySunnyLand_ENABLE_CPPCHECK)
    MySunnyLand_enable_cppcheck(${MySunnyLand_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(MySunnyLand_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    MySunnyLand_enable_coverage(MySunnyLand_options)
  endif()

  if(MySunnyLand_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(MySunnyLand_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(MySunnyLand_ENABLE_HARDENING AND NOT MySunnyLand_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR MySunnyLand_ENABLE_SANITIZER_UNDEFINED
       OR MySunnyLand_ENABLE_SANITIZER_ADDRESS
       OR MySunnyLand_ENABLE_SANITIZER_THREAD
       OR MySunnyLand_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    MySunnyLand_enable_hardening(MySunnyLand_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
