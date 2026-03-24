cmake_minimum_required(VERSION 3.24)

project(ChordBro VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(JUCE_DIR "" CACHE PATH "Path to a JUCE source tree or JUCE config package")
set(CHORDBRO_PLUGIN_FORMATS VST3)

if(APPLE)
    list(APPEND CHORDBRO_PLUGIN_FORMATS AU)
endif()

if(JUCE_DIR AND EXISTS "${JUCE_DIR}/CMakeLists.txt")
    add_subdirectory("${JUCE_DIR}" JUCE)
elseif(JUCE_DIR)
    list(PREPEND CMAKE_PREFIX_PATH "${JUCE_DIR}")
endif()

if(NOT TARGET juce::juce_audio_utils)
    find_package(JUCE CONFIG REQUIRED)
endif()

file(GLOB_RECURSE CHORDBRO_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

file(GLOB CHORDBRO_ASSETS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Assets/*.svg"
)

juce_add_binary_data(ChordBroAssets
    SOURCES
        ${CHORDBRO_ASSETS}
)

juce_add_plugin(ChordBro
    COMPANY_NAME "FL Plugins"
    COMPANY_WEBSITE "https://chordbro.com"
    BUNDLE_ID "com.flplugins.chordbro"
    PLUGIN_MANUFACTURER_CODE FLPI
    PLUGIN_CODE ChBr
    FORMATS ${CHORDBRO_PLUGIN_FORMATS}
    PRODUCT_NAME "ChordBro"
    COPY_PLUGIN_AFTER_BUILD FALSE
    IS_SYNTH TRUE
    IS_MIDI_EFFECT FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES Instrument Tools
)

juce_generate_juce_header(ChordBro)

target_sources(ChordBro PRIVATE ${CHORDBRO_SOURCES})

target_include_directories(ChordBro PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Components"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Messaging"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Process"
)

target_compile_definitions(ChordBro
    PRIVATE
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(ChordBro
    PRIVATE
        ChordBroAssets
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_plugin_client
        juce::juce_cryptography
        juce::juce_gui_extra
        juce::juce_opengl
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
