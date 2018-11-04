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
        template <typename T>
        struct is_pathable { static constexpr bool value = false; };

        template <> struct is_pathable<char*>             { static constexpr bool value = true; };
        template <> struct is_pathable<const char*>       { static constexpr bool value = true; };
        template <> struct is_pathable<std::string>       { static constexpr bool value = true; };
        template <> struct is_pathable<std::vector<char>> { static constexpr bool value = true; };
        template <> struct is_pathable<std::list<char>>   { static constexpr bool value = true; };
        template <> struct is_pathable<collection_entry>  { static constexpr bool value = true; };

        template <typename T>
        constexpr bool is_pathable_v = is_pathable<T>::value;

    } // namespace path_traits

} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_PATH_TRAITS_HPP
