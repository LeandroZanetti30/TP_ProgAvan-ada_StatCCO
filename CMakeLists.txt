cmake_minimum_required(VERSION 3.10)
project(code_cco)
set(CMAKE_CXX_STANDARD 17)
add_executable(cco_tp1
    src/main.cpp
    src/cco/CCOEngine.cpp
    src/cco/Tree.cpp
    src/cco/Geometry.cpp
    src/cco/Hemodynamics.cpp
    src/cco/IO.cpp
)
target_include_directories(cco_tp1 PRIVATE src)

# --- Alvo make run ---
add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cco_tp1
            ${CMAKE_SOURCE_DIR}/params/default_params.json
            ${CMAKE_SOURCE_DIR}/output/tree_metrics.csv
            ${CMAKE_SOURCE_DIR}/output/tree.svg
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS cco_tp1
    COMMENT "Executando CCO TP1 com parâmetros padrão..."
)


add_custom_target(analyze
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/analyze.py ${CMAKE_SOURCE_DIR}/output/tree_metrics.csv
    COMMENT "Gerando gráficos de análise..."
)
