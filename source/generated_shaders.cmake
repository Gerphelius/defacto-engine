# Script generates a header file with an unordered_map
# where the key is the shader filename and the value is the shader's source code.

# Inputs:
#   OUTFILE           - file to write the content to
#   SHADER_FILE_0...N - indexed shader file from 0 to N
#   SHADERS_COUNT     - last shader file index, because foreach <stop> is inclusive

# Reads SHADER_FILE_0 - SHADER_FILE_N variables passed in
# and creates a list of files (paths)
foreach(I RANGE 0 ${SHADERS_COUNT})
    if(DEFINED SHADER_FILE_${I})
        list(APPEND SHADER_FILES "${SHADER_FILE_${I}}")
    endif()
endforeach()

file(WRITE "${OUTFILE}" "#pragma once\n")
file(APPEND "${OUTFILE}" "#include <string>\n")
file(APPEND "${OUTFILE}" "#include <unordered_map>\n")
file(APPEND "${OUTFILE}" "namespace DF::Generated {\n")
file(APPEND "${OUTFILE}" "inline const std::unordered_map<std::string, std::string> shaders = {\n")

foreach(FILE ${SHADER_FILES})
    get_filename_component(NAME "${FILE}" NAME)
    file(READ "${FILE}" CONTENTS)
    # Escape quotes
    string(REPLACE "\"" "\\\"" CONTENTS "${CONTENTS}")

    file(APPEND "${OUTFILE}" "  { \"${NAME}\", R\"(${CONTENTS})\" },\n")
endforeach()

file(APPEND "${OUTFILE}" "};\n}")
