project(texttrainer)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
set(CMAKE_CXX_FLAGS "-g -Wall")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin CACHE PATH "Single Directory for all Executables.")

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} include)

set(texttrainer_SRCS src/qtexttrainer.cpp src/word.cpp src/meaning.cpp)
set(texttrainer_HDRS include/meaning.h include/phrase.h include/qtexttrainer.h include/word.h)
set(texttrainer_FORM ui/texttrainer.ui)
set(texttrainer_RESS resources/texttrainer.qrc)

qt4_automoc(${texttrainer_SRCS})
QT4_WRAP_CPP(texttrainer_HDRS_MOC ${texttrainer_HDRS})
QT4_WRAP_UI(texttrainer_FORM_HDRS ${texttrainer_FORM})
QT4_ADD_RESOURCES(texttrainer_RESS_RCC ${texttrainer_RESS})

add_executable(texttrainer ${texttrainer_SRCS}
	${texttrainer_HDRS_MOC}
	${texttrainer_FORM_HDRS}
	${texttrainer_RESS_RCC}
)
target_link_libraries(texttrainer ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS texttrainer RUNTIME DESTINATION bin)
