#include <iostream>
#include <string>

#include <irods/filesystem.hpp>
#include <irods/rodsClient.h>

namespace fs = irods::filesystem;

int main(int _argc, char* _argv[])
{
    /*
    {
        fs::path p = "/tempZone///./home/rods/col.d/../////";

        std::cout << "path             => " << p << '\n';
        std::cout << "parent path      => " << p.parent_path() << '\n';
        std::cout << "root collection  => " << p.root_collection() << '\n';
        std::cout << "object name      => " << p.object_name() << '\n';
        std::cout << "lexically normal => " << p.lexically_normal() << '\n';
        std::cout << "lexically normal => " << fs::path{"foo/./bar/.."}.lexically_normal() << '\n';
        std::cout << "stem             => " << fs::path{"/foo/bar.txt"}.stem() << '\n';
        std::cout << "extension        => " << fs::path{"/foo/bar.txt"}.extension() << '\n';

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
    }
    */

    {
        rodsEnv env;
        if (auto ec = getRodsEnv(&env); ec < 0)
        {
            std::cout << "error getting env.\n";
            return 1;
        }

        rErrMsg_t errors;
        auto* conn = rcConnect(env.rodsHost, env.rodsPort, env.rodsUserName,
                               env.rodsZone, 0, &errors);
        if (!conn)
        {
            std::cout << "connection error.\n";
            return 1;
        }

        char passwd[] = "rods";
        if (auto ec = clientLoginWithPassword(conn, passwd); ec != 0)
        {
            std::cout << "login error.\n";
            return 1;
        }

        fs::path home = "/tempZone/home/kory";
        auto dobj = home / "main.cpp";
        auto new_col = home / "col1.d";
        auto new_cols = home / "col2.d/col3.d/col4.d";

        std::cout << std::boolalpha;

        std::cout << "is_collection(" << home << ") => " << fs::is_collection(conn, home) << '\n';
        std::cout << "is_collection(" << dobj << ") => " << fs::is_collection(conn, dobj) << '\n';

        std::cout << '\n';
        std::cout << "is_data_object(" << dobj << ") => " << fs::is_data_object(conn, dobj) << '\n';
        std::cout << "is_data_object(" << home << ") => " << fs::is_data_object(conn, home) << '\n';

        std::cout << '\n';
        std::cout << "data_object_size(" << dobj << ") => " << fs::data_object_size(conn, dobj) << '\n';

        std::cout << '\n';
        std::cout << "create_collection(" << new_col << ") => " << fs::create_collection(conn, new_col) << '\n';
        std::cout << "create_collections(" << new_cols << ") => " << fs::create_collections(conn, new_cols) << '\n';

        try
        {
            std::cout << '\n';
            std::cout << "is_empty(" << dobj << ") => " << fs::is_empty(conn, dobj) << '\n';
            std::cout << "is_empty(" << home << ") => " << fs::is_empty(conn, home) << '\n';
            std::cout << "is_empty(" << new_cols << ") => " << fs::is_empty(conn, new_cols) << '\n';

            std::cout << '\n';
            std::cout << "remove(" << new_cols << ") => " << fs::remove(conn, new_cols) << '\n';
            std::cout << "exists(" << new_cols << ") => " << fs::exists(conn, new_cols) << '\n';

            std::cout << '\n';
            auto col2d = new_cols.parent_path().parent_path();
            std::cout << "create_collections(" << new_cols << ") => " << fs::create_collections(conn, new_cols) << '\n';
            std::cout << "remove_all(" << col2d << ") => " << fs::remove_all(conn, col2d) << '\n';
            std::cout << "exists(" << col2d << ") => " << fs::exists(conn, col2d) << '\n';
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }

        rcDisconnect(conn);
        std::cout << "disconnected.\n";
    }

    return 0;
}

