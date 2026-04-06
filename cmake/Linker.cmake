macro(MySunnyLand_configure_linker project_name)
  set(MySunnyLand_USER_LINKER_OPTION
    "DEFAULT"
      CACHE STRING "Linker to be used")
    set(MySunnyLand_USER_LINKER_OPTION_VALUES "DEFAULT" "SYSTEM" "LLD" "GOLD" "BFD" "MOLD" "SOLD" "APPLE_CLASSIC" "MSVC")
  set_property(CACHE MySunnyLand_USER_LINKER_OPTION PROPERTY STRINGS ${MySunnyLand_USER_LINKER_OPTION_VALUES})
  list(
    FIND
    MySunnyLand_USER_LINKER_OPTION_VALUES
    ${MySunnyLand_USER_LINKER_OPTION}
    MySunnyLand_USER_LINKER_OPTION_INDEX)

  if(${MySunnyLand_USER_LINKER_OPTION_INDEX} EQUAL -1)
    message(
      STATUS
        "Using custom linker: '${MySunnyLand_USER_LINKER_OPTION}', explicitly supported entries are ${MySunnyLand_USER_LINKER_OPTION_VALUES}")
  endif()

  set_target_properties(${project_name} PROPERTIES LINKER_TYPE "${MySunnyLand_USER_LINKER_OPTION}")
endmacro()
