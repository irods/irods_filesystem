#include <iostream>
#include <string>

#include <irods/filesystem.hpp>
#include <irods/rodsClient.h>

#include <cassert>

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
        //auto* conn = rcConnect(env.rodsHost, env.rodsPort, "rods",
                               //env.rodsZone, 0, &errors);
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

        try
        {
            std::cout << std::boolalpha;

            //fs::path home = "/tempZone";
            fs::path home = "/tempZone/home/kory";
            //auto src = home / "col1.d";
            //auto dst = home / "col1.renamed.d";

            for (auto&& e : home / "main.renamed.cpp")
                std::cout << e << '\n';
            std::cout << '\n';

            std::cout << "is equal : " << (home / "main.cpp" == home / "main.renamed.cpp") << '\n';
            std::cout << "less than: " << (home / "main.cpp" <  home / "main.renamed.cpp") << '\n';
            std::cout << "less than: " << (home / "main.renamed.cpp" >= home) << "\n\n";

            //fs::rename(conn, home / "main.cpp", home / "main.renamed.cpp");
            //fs::rename(conn, home / "col1.renamed.d", home / "test.d");
            fs::remove(conn, home / "src");

            auto mtime = fs::last_write_time(conn, home);
            std::cout << "last_write_time(" << home << "): " << mtime.time_since_epoch().count() << '\n';
                //std::chrono::duration_cast<std::chrono::seconds>(mtime.time_since_epoch()).count() << '\n';

            //fs::rename(conn, src, dst);
            //fs::rename(conn, home / "main.cpp", dst / "main.renamed.cpp");
            //fs::rename(conn, home / "main.cpp", dst);

            fs::path root = "/";
            auto zone = root / "tempZone"; 
            auto trash = root / "tempZone/trash"; 
            //auto col4d = trash / "home/kory/col2.d/col3.d/col4.d";

            std::cout << zone << " is a valid collection  : " << fs::is_collection(conn, zone) << '\n';
            std::cout << trash << " is a valid collection : " << fs::is_collection(conn, trash) << '\n';
            //std::cout << col4d << " is an empty collection? " << fs::is_empty(conn, col4d) << '\n';

            /*
            std::cout << "\nusing ranged-for collection_iterator:\n";
            for (const auto& e : fs::collection_iterator{conn, zone})
                std::cout << e << '\n';
            */

            std::cout << "\nusing ranged-for recursive_collection_iterator:\n";
            fs::recursive_collection_iterator rit{conn, "/tempZone/home"};
            //rit.disable_recursion_pending();
            //for (const auto& e : fs::recursive_collection_iterator{conn, root})
            for (const auto& e : rit)
                std::cout << e << '\n';

            /*
            std::cout << "traditional for-loop using iterators:\n";
            for (fs::collection_iterator it{conn, home}; it != fs::collection_iterator{}; ++it)
                std::cout << it->path() << '\n';
            */

            /*
            auto dobj = home / "main.cpp";
            auto new_col = home / "col1.d";
            auto new_cols = home / "col2.d/col3.d/col4.d";

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

            std::cout << '\n';
            auto new_name = home / "main.renamed.cpp";
            std::cout << "rename(" << dobj << ", " << new_name << ")\n"; fs::rename(conn, dobj, new_name);
            std::cout << "exists(" << dobj << ") => " << fs::exists(conn, dobj) << '\n';
            std::cout << "exists(" << new_name << ") => " << fs::exists(conn, new_name) << '\n';
            std::cout << "rename(" << new_name << ", " << dobj << ")\n"; fs::move(conn, new_name, dobj);
            std::cout << "exists(" << dobj << ") => " << fs::exists(conn, dobj) << '\n';
            std::cout << "exists(" << new_name << ") => " << fs::exists(conn, new_name) << '\n';

            auto new_col_name = home / "col1.renamed.d";
            std::cout << "rename(" << new_col << ", " << new_col_name << ")\n"; fs::rename(conn, new_col, new_col_name);
            std::cout << "exists(" << new_col << ") => " << fs::exists(conn, new_col) << '\n';
            std::cout << "exists(" << new_col_name << ") => " << fs::exists(conn, new_col_name) << '\n';
            */

            /*
            std::cout << '\n';
            auto src = home / "src";
            auto dst = home / "dst";
            auto dst1 = src / "dst";
            std::cout << "create_collection(" << src << ") => " << fs::create_collection(conn, src) << '\n';
            std::cout << "exists(" << src << ") => " << fs::exists(conn, src) << '\n';
            std::cout << "rename(" << src << ", " << dst << ")\n"; fs::rename(conn, src, dst);
            std::cout << "exists(" << src << ") => " << fs::exists(conn, src) << '\n';
            std::cout << "exists(" << dst << ") => " << fs::exists(conn, dst) << '\n';
            */

            /*
            fs::path p = home / "sandbox";
            auto from = p / "from";
            auto to = p / "to";

            fs::create_collections(conn, from);
            fs::create_collection(conn, to);

            fs::rename(conn, from, p / "to/");                  // error: "to" is a collection.
            fs::rename(conn, from / "f1.txt", to / "f2.txt");   // OK
            fs::rename(conn, from, to);                         // error: "to" is not empty.
            fs::rename(conn, from, to / "subcol");              // OK

            fs::remove_all(conn, p, fs::remove_options::no_trash);
            */

            /*
            std::cout << fs::path{"foo/./bar/.."}.lexically_normal() << '\n';
            std::cout << fs::path{"foo/.///bar/../"}.lexically_normal() << '\n';
            std::cout << fs::path{"/../foo/.///bar/.."}.lexically_normal() << '\n';
            std::cout << fs::path{"/../foo/.///bar/../"}.lexically_normal() << '\n';

            fs::path p = "/../foo/.///bar/../";
            std::cout << "p => " << p << '\n';

            std::cout << '\n';
            for (const auto& e : p)
                std::cout << "\te => " << e << '\n';

            auto fit = std::begin(p);
            std::cout << "*fit => " << *fit << '\n';
            std::cout << "*fit => " << *++fit << '\n';
            std::cout << "*fit => " << *++fit << '\n';
            std::cout << "*fit => " << *--fit << '\n';
            std::cout << "*fit => " << *++fit << '\n';
            std::cout << "*fit => " << *--fit << '\n';
            std::cout << "*fit => " << *--fit << '\n';

            std::cout << '\n';
            for (auto it = std::rbegin(p); it != std::rend(p); ++it)
                std::cout << "\te => " << *it << '\n';

            auto eit = --std::end(p);
            std::cout << "\n*eit => " << *eit << '\n';
            std::cout << "*--eit => " << *--eit << '\n';
            std::cout << "*--eit => " << *--eit << '\n';
            std::cout << "*++eit => " << *++eit << '\n';
            std::cout << "*--eit => " << *--eit << '\n';
            std::cout << "*++eit => " << *++eit << '\n';
            std::cout << "*++eit => " << *++eit << '\n';

            auto rit = std::rbegin(p);
            std::cout << "\n*rit => " << *rit << '\n';
            std::cout << "*++rit => " << *++rit << '\n';
            std::cout << "*++rit => " << *++rit << '\n';
            std::cout << "*--rit => " << *--rit << '\n';
            std::cout << "*++rit => " << *++rit << '\n';
            std::cout << "*--rit => " << *--rit << '\n';
            std::cout << "*--rit => " << *--rit << '\n';

            assert(fs::path{"foo/./bar/.."}.lexically_normal() == "foo/");
            assert(fs::path{"foo/.///bar/../"}.lexically_normal() == "foo/");
            assert(fs::path{"/../foo/.///bar/.."}.lexically_normal() == "/foo");
            assert(fs::path{"/../foo/.///bar/../"}.lexically_normal() == "/foo/");
            */
        }
        catch (const fs::filesystem_error& e)
        {
            std::cout << e.what() << '\n';
        }

        rcDisconnect(conn);
        std::cout << "disconnected.\n";
    }

    return 0;
}

