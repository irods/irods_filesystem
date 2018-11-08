#ifndef IRODS_FILESYSTEM_FILESYSTEM_HPP
#define IRODS_FILESYSTEM_FILESYSTEM_HPP

#include <string>
#include <istream>
#include <ostream>
#include <ctime> // TODO Prefer chrono instead!
#include <cstdint>

//#include <irods/filesystem/path.hpp>

namespace irods::filesystem
{
    class path;
    class filesystem_error;
    class collection_entry;
    class collection_iterator;
    class recursive_collection_iterator;
    class object_status;

    auto swap(path& _lhs, path& _rhs) -> void;

    auto hash_value(const path& _p) -> std::size_t;

    // Enable C++11 range-based for statements.

    auto begin(const collection_iterator& _iter) -> const collection_iterator&;
    auto end(const collection_iterator&) -> collection_iterator;

    auto begin(const recursive_collection_iterator& _iter) -> const recursive_collection_iterator&;
    auto end(const recursive_collection_iterator&) -> recursive_collection_iterator;

    enum class object_type
    {
        status_error,
        object_not_found,
        data_object,
        collection_object,
        special_object,
        type_unknown
    };

    enum class perms
    {
        no_perms,
        owner_read,
        owner_write,
        owner_exe,
        owner_all,
        group_read,
        group_write,
        group_exe,
        group_all,
        others_read,
        others_write,
        others_exe,
        others_all,
        all_all,
        set_uid_on_exe,
        set_gid_on_exe,
        sticky_bit,
        perms_mask,
        perms_not_known,
        add_perms,
        remove_perms
    };

    struct space_info // Returned by space function.
    {
        std::uintmax_t capacity;
        std::uintmax_t free;
        std::uintmax_t available; // Free space available to non-privileged process.
    };

    enum class copy_option
    {
        none,
        fail_if_exists = none,
        overwrite_if_exists
    };

    // Operational functions

    //auto absolute(const path& _p, const path& base = current_path()) -> path;

    //auto canonical(const path& _p, const path& base = current_path()) -> path;

    auto copy(const path& _from, const path& _to) -> path;

    auto copy_collection(const path& _from, const path& _to) -> path;

    auto copy_data_object(const path& _from, const path& _to, copy_option _option = copy_option::overwrite_if_exists) -> path;

    auto create_collections(const path& _p) -> bool;
    auto create_collection(const path& _p) -> bool;

    //auto current_path() -> path;
    //auto current_path(const path& _p) -> void;

    auto exists(object_status _s) noexcept -> bool;
    auto exists(const path& _p) -> bool;

    auto equivalent(const path& _p1, const path& _p2) -> bool;

    auto data_object_size(const path& _p) -> std::uintmax_t;

    auto initial_path() -> const path&;

    auto is_collection(object_status _s) noexcept -> bool;
    auto is_collection(const path& _p) -> bool;

    auto is_empty(const path& _p) -> bool;

    auto is_other(object_status _s) noexcept -> bool;
    auto is_other(const path& _p) -> bool;

    auto is_data_object(object_status _s) noexcept -> bool;
    auto is_data_object(const path& _p) -> bool;

    auto last_write_time(const path& _p) -> std::time_t;
    auto last_write_time(const path& _p, const std::time_t _new_time) -> void;

    //auto relative(const path& _p, const path& _base = current_path()) -> path;

    auto remove(const path& _p) -> bool;
    auto remove_all(const path& _p) -> std::uintmax_t;

    auto rename(const path& _from, const path& _to) -> void;

    auto resize_data_object(const path& _p, std::uintmax_t _size) -> void;

    auto space(const path& _p) -> space_info;

    auto status(const path& _p) -> object_status;

    auto status_known(object_status _s) noexcept -> bool;

    auto system_complete(const path& _p) -> path;
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_FILESYSTEM_HPP
