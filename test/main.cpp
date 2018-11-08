#include <iostream>
#include <string>

#include <irods/filesystem.hpp>

namespace fs = irods::filesystem;

int main(int _argc, char* _argv[])
{
    fs::path p = "/tempZone///./home/rods/col.d/../////";

    std::cout << "path             => " << p << '\n';
    std::cout << "parent path      => " << p.parent_path() << '\n';
    std::cout << "root collection  => " << p.root_collection() << '\n';
    std::cout << "object name      => " << p.object_name() << '\n';
    std::cout << "lexically normal => " << p.lexically_normal() << '\n';
    std::cout << "lexically normal => " << fs::path{"foo/./bar/.."}.lexically_normal() << '\n';

    std::string ps = p; // Implicit cast

    std::cout << "\nforward iterator over 'path':\n";
    for (const auto& e : p)
        std::cout << "  path-element => " << e << '\n';

    std::cout << "\nbackwards iterator over 'path':\n";
    auto it = std::end(p);
    while (it-- != std::begin(p))
        std::cout << "  path-element => " << *it << '\n';

    std::cout << "\nbackwards iterator over 'path' (using reverse_iterator):\n";
    for (auto it = std::rbegin(p); it != std::rend(p); ++it)
        std::cout << "  path-element => " << *it << '\n';

    return 0;
}

