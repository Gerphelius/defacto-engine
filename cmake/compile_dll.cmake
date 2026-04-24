# ===================================================================================
#
# This script is used to compile and link dll separately from main cmake config
# so it can be called from external command at runtime for hot reloading.
# It also generates unique bdb file each build to bypass vs file locking.
#
# Inputs:
#   FILENAME        - name of a dll file
#   SOURCE_FILE     - path to library translation unit
#   INCLUDE_ENGINE  - engine include directory
#   OUT_DIR         - directory where to put compiled dll
#   COMPILER        - path to compiler
#   LINKER          - path to linker
#   LIBRARIES       - list of libraries to link
#
# ===================================================================================

# Generate timestamp for library .pdb file so vs can block it as long as it want
execute_process(
    COMMAND powershell -NoProfile -Command "Get-Date -Format yyyyMMdd_HHmmss"
    OUTPUT_VARIABLE TIMESTAMP
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(PDB_PATH  "${OUT_DIR}/${FILENAME}_${TIMESTAMP}.pdb")

# Delete old .pdb files
file(GLOB OLD_PDBS "${OUT_DIR}/${FILENAME}_*.pdb")
foreach(OLD_PDB ${OLD_PDBS})
    file(REMOVE "${OLD_PDB}")
endforeach()

execute_process(
    COMMAND ${COMPILER}
        /nologo /TP /c ${SOURCE_FILE}
        /I${INCLUDE_ENGINE}
        /GR- /EHsc /Od /Oi /WX /W4
        /wd4201 /wd4100 /wd4189 /wd4456 /wd4505
        /MTd
        /Zi
        /Zc:nrvo-
        /FS
        /Fd${PDB_PATH}
        /std:c++20
        "/Fo${OUT_DIR}/${FILENAME}.cpp.obj"
    RESULT_VARIABLE COMPILE_RESULT
)

if(NOT COMPILE_RESULT EQUAL 0)
    message(FATAL_ERROR "${FILENAME} dll compilation failed (exit ${COMPILE_RESULT})")
endif()

execute_process(
    COMMAND ${LINKER}
        /nologo
        /DLL
        "/OUT:${OUT_DIR}/${FILENAME}.dll"
        "/IMPLIB:${OUT_DIR}/${FILENAME}.lib"
        /DEBUG
        /PDB:${PDB_PATH}
        /INCREMENTAL:NO
        /OPT:REF
        /MACHINE:X64
        /LIBPATH:${OUT_DIR}
        ${LIBRARIES}
        "${OUT_DIR}/${FILENAME}.cpp.obj"
    RESULT_VARIABLE LINK_RESULT
)

if(NOT LINK_RESULT EQUAL 0)
    message(FATAL_ERROR "${FILENAME} dll linking failed (exit ${LINK_RESULT})")
endif()
