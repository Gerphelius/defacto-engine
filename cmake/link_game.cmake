# ===================================================================================
#
# This script is used to compile and link game dll separately from main cmake config
# so it can be called from external command at runtime for hot reloading.
# It also generates unique game bdb file each build to bypass vs file locking.
#
# Inputs:
#   SOURCE_FILE     - path to game translation unit
#   INCLUDE_ENGINE  - engine include directory
#   OUT_DIR         - directory where to put compiled game dll
#   COMPILER        - path to compiler
#   LINKER          - path to linker
#
# ===================================================================================

# Generate timestamp for game .pdb file so vs can block it as long as it want
execute_process(
    COMMAND powershell -NoProfile -Command "Get-Date -Format yyyyMMdd_HHmmss"
    OUTPUT_VARIABLE TIMESTAMP
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(PDB_PATH  "${OUT_DIR}/game_${TIMESTAMP}.pdb")

# Delete old .pdb files
file(GLOB OLD_PDBS "${OUT_DIR}/game_*.pdb")
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
        /FS
        /Fd${PDB_PATH}
        /std:c++20
        "/Fo${OUT_DIR}/game.cpp.obj"
    RESULT_VARIABLE COMPILE_RESULT
)

if(NOT COMPILE_RESULT EQUAL 0)
    message(FATAL_ERROR "Game dll compilation failed (exit ${COMPILE_RESULT})")
endif()

execute_process(
    COMMAND ${LINKER}
        /nologo
        /DLL
        "/OUT:${OUT_DIR}/game.dll"
        "/IMPLIB:${OUT_DIR}/game.lib"
        /DEBUG
        /PDB:${PDB_PATH}
        /INCREMENTAL:NO
        /OPT:REF
        /MACHINE:X64
        "${OUT_DIR}/game.cpp.obj"
    RESULT_VARIABLE LINK_RESULT
)

if(NOT LINK_RESULT EQUAL 0)
    message(FATAL_ERROR "Game dll linking failed (exit ${LINK_RESULT})")
endif()
