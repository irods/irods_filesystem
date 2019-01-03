#include <catch.hpp>

#include <irods/filesystem/path.hpp>

#include <string>

TEST_CASE("paths", "[path]")
{
    namespace fs = irods::experimental::filesystem;

    SECTION("default construction")
    {
        fs::path p;
        REQUIRE(p.empty());
    }

    SECTION("construction")
    {
        std::string path_string = "/tempZone/home/rods";
        fs::path p{path_string};
        REQUIRE(p.string() == path_string);
    }
}

