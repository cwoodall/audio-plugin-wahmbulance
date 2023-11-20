#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <plugin_processor.h>

class WahmbulanceProcessorTestFixture {
protected:
    WahmbulanceProcessor testPlugin;

public:
    WahmbulanceProcessorTestFixture() : testPlugin() {
    }
};

TEST_CASE_METHOD(WahmbulanceProcessorTestFixture, "Plugin instance name", "[name]") {
    CHECK_THAT(testPlugin.getName().toStdString(),
               Catch::Matchers::Equals("Wahmbulance"));
}
