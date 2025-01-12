# ##  CMakeLists.txt; coding: utf-8 ---

#set(PYFILES conftest.py)
#install(FILES ${PYFILES} DESTINATION ${FEELPP_PYTHON_MODULE_PATH}/feelpp)

add_custom_target(notebooks ALL)

if ( FEELPP_ENABLE_PYTHON3_VENV )
    add_dependencies(notebooks venv)

    add_test(NAME feelpp-notebook-core
        COMMAND ${CMAKE_COMMAND} -E env 
            CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} 
            ${PYTHON} -m pytest -s --nbmake ${CMAKE_CURRENT_SOURCE_DIR}/notebook-expr.ipynb
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_test(NAME feelpp-notebook-toolbox-cfpdes
        COMMAND ${CMAKE_COMMAND} -E env
        CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        ${PYTHON} -m pytest -s --nbmake ${CMAKE_CURRENT_SOURCE_DIR}/notebook-toolbox-cfpdes.ipynb
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_test(NAME feelpp-notebook-toolbox-heat
        COMMAND ${CMAKE_COMMAND} -E env
            CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} 
            ${PYTHON} -m pytest -s --nbmake ${CMAKE_CURRENT_SOURCE_DIR}/notebook-toolbox-heat.ipynb
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_tests_properties(feelpp-notebook-core PROPERTIES FIXTURES_REQUIRED notebooks)
    set_tests_properties(feelpp-notebook-toolbox-heat PROPERTIES FIXTURES_REQUIRED notebooks)
    set_tests_properties(feelpp-notebook-toolbox-cfpdes PROPERTIES FIXTURES_REQUIRED notebooks)
endif(FEELPP_ENABLE_PYTHON3_VENV)