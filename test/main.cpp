#include <iostream>

#include <irods/filesystem/path.hpp>

namespace fs = irods::filesystem;

int main(int _argc, char* _argv[])
{
    fs::path p = "/tempZone/home/rods/";

    std::cout << "path => " << p << '\n';

    std::cout << "\nforward iterator:\n";
    for (const auto& e : p)
        std::cout << "  path-element => " << e << '\n';

    std::cout << "\nbackwards iterator:\n";
    auto it = std::end(p);
    while (it-- != std::begin(p))
        std::cout << "  path-element => " << *it << '\n';

    std::cout << "\nbackwards iterator (using reverse_iterator):\n";
    for (auto it = std::rbegin(p); it != std::rend(p); ++it)
        std::cout << "  path-element => " << *it << '\n';

    return 0;
}

