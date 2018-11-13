#ifndef IRODS_FILESYSTEM_PATH_TRAITS_HPP
#define IRODS_FILESYSTEM_PATH_TRAITS_HPP

#include <string>
#include <vector>
#include <list>

namespace irods::filesystem
{
    class collection_entry;
        
    namespace path_traits
    {
        template <typename>
        constexpr bool is_pathable = false;

        // clang-format off
        template <> constexpr bool is_pathable<char*>             = true;
        template <> constexpr bool is_pathable<const char*>       = true;
        template <> constexpr bool is_pathable<std::string>       = true;
        template <> constexpr bool is_pathable<std::vector<char>> = true;
        template <> constexpr bool is_pathable<std::list<char>>   = true;
        template <> constexpr bool is_pathable<collection_entry>  = true;
        // clang-format on

#if 0
        template <typename T>
        struct is_pathable { static constexpr bool value = false; };

        // clang-format off
        template <> struct is_pathable<char*>             { static constexpr bool value = true; };
        template <> struct is_pathable<const char*>       { static constexpr bool value = true; };
        template <> struct is_pathable<std::string>       { static constexpr bool value = true; };
        template <> struct is_pathable<std::vector<char>> { static constexpr bool value = true; };
        template <> struct is_pathable<std::list<char>>   { static constexpr bool value = true; };
        template <> struct is_pathable<collection_entry>  { static constexpr bool value = true; };
        // clang-format on

        template <typename T>
        constexpr bool is_pathable_v = is_pathable<T>::value;
#endif
    } // namespace path_traits
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_PATH_TRAITS_HPP
