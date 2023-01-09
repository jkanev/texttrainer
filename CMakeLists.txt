project(texttrainer LANGUAGES CXX)
cmake_minimum_required(VERSION 3.7.0)

set(CMAKE_PREFIX_PATH "")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin CACHE PATH "Single Directory for all Executables.")

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} include)

set(texttrainer_SRCS src/qtexttrainer.cpp src/word.cpp src/meaning.cpp)
set(texttrainer_HDRS include/meaning.h include/phrase.h include/qtexttrainer.h include/word.h)
set(texttrainer_FORM include/texttrainer.ui  include/texttrainer-android.ui)
set(texttrainer_RESS resources/texttrainer.qrc)

#qt5_generate_moc(${texttrainer_SRCS})
#qt5_wrap_cpp(texttrainer_HDRS_MOC ${texttrainer_HDRS})
#qt5_wrap_ui(texttrainer_FORM_HDRS ${texttrainer_FORM})
#qt_add_resources(texttrainer_RESS_RCC ${texttrainer_RESS})

add_executable(texttrainer
	${texttrainer_FORM}
	${texttrainer_SRCS}
	${texttrainer_HDRS}
	${texttrainer_RESS}
)
target_link_libraries(texttrainer Qt5::Widgets)
install(TARGETS texttrainer RUNTIME DESTINATION bin)

