TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
CONFIG += testcase

INCLUDEPATH += ../incl
INCLUDEPATH += helpers

# Link against tgraph library
LIBS += -L../lib

# Try both release and debug versions
exists(../lib/libtgraph.a) {
    LIBS += -ltgraph
}
exists(../lib/libtgraph_debug.a) {
    LIBS += -ltgraph_debug
}

# Google Test
# You may need to adjust these paths based on your system
unix {
    # Try to find gtest in common locations
    GTEST_INCLUDE = /usr/include/gtest
    exists($$GTEST_INCLUDE) {
        INCLUDEPATH += /usr/include
        LIBS += -lgtest -lgtest_main -lpthread
    } else {
        # Alternative: use pkg-config
        CONFIG += link_pkgconfig
        PKGCONFIG += gtest
        LIBS += -lpthread
    }
}

macx {
    # macOS specific
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lgtest -lgtest_main -lpthread
}

# Test helper library sources
HEADERS += \
    helpers/graph_builders.h \
    helpers/graph_comparators.h \
    helpers/property_validators.h

SOURCES += \
    helpers/graph_builders.cpp \
    helpers/graph_comparators.cpp \
    helpers/property_validators.cpp

# Unit test sources
SOURCES += \
    unit/core/test_graph_container.cpp

# Integration test sources
SOURCES += \
    integration/test_planar_pipeline.cpp

# Set target name
TARGET = pigale_tests

# Output directory
DESTDIR = .

# Build configuration
CONFIG(debug, debug|release) {
    TARGET = pigale_tests_debug
    OBJECTS_DIR = ./debug
} else {
    TARGET = pigale_tests
    OBJECTS_DIR = ./release
}

# Enable warnings
QMAKE_CXXFLAGS += -Wall -Wextra

message("Building Pigale test suite")
message("Target: $$TARGET")
