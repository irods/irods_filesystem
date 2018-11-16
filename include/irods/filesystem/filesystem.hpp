#ifndef IRODS_FILESYSTEM_FILESYSTEM_HPP
#define IRODS_FILESYSTEM_FILESYSTEM_HPP

#include <irods/filesystem/config.hpp>
#include <irods/filesystem/object_status.hpp>
#include <irods/filesystem/permissions.hpp>

#include <string>
#include <istream>
#include <ostream>
#include <chrono>
#include <ctime> // TODO Prefer chrono instead!
#include <cstdint>

namespace irods::filesystem
{
    class path;

    /*
    struct space_info // Returned by space function.
    {
        std::uintmax_t capacity;
        std::uintmax_t free;
        std::uintmax_t available; // Free space available to non-privileged process.
    };
    */

    enum class copy_options
    {
        none,
        skip_existing,
        overwrite_if_exists,
        update_existing,
        recursive,
        collections_only,
        in_recursive_copy
    };

    enum class remove_options : std::uint8_t
    {
        none, // Moves objects to trash.
        no_trash
    };

    // Operational functions

    //auto absolute(conn* _conn, const path& _p, const path& base = current_path()) -> path;

    //auto canonical(conn* _conn, const path& _p, const path& base = current_path()) -> path;
    //auto weakly_canonical(conn* _conn, const path& _p, const path& base = current_path()) -> path;

    //auto relative(conn* _conn, const path& _p, const path& _base = current_path()) -> path;
    //auto proximate(conn* _conn, const path& _p, const path& _base = current_path()) -> path;

    auto copy(conn* _conn, const path& _from, const path& _to, copy_options _options = copy_options::none) -> void;
    auto copy_data_object(conn* _conn, const path& _from, const path& _to, copy_options _options = copy_options::none) -> bool;
    //auto copy_collection(conn* _conn, const path& _from, const path& _to, copy_options _options = copy_options::none) -> bool;

    auto create_collection(conn* _conn, const path& _p) -> bool; // Implies perms::all
    auto create_collection(conn* _conn, const path& _p, const path& _existing_p) -> bool;
    auto create_collections(conn* _conn, const path& _p) -> bool;

    //auto current_path(conn* _conn) -> path;
    //auto current_path(conn* _conn, const path& _p) -> void;

    auto exists(object_status _s) noexcept -> bool;
    auto exists(conn* _conn, const path& _p) -> bool;

    auto equivalent(const path& _p1, const path& _p2) -> bool;

    auto data_object_size(conn* _conn, const path& _p) -> std::uintmax_t;

    auto is_collection(object_status _s) noexcept -> bool;
    auto is_collection(conn* _conn, const path& _p) -> bool;

    auto is_empty(conn* _conn, const path& _p) -> bool;

    auto is_other(object_status _s) noexcept -> bool;
    auto is_other(conn* _conn, const path& _p) -> bool;

    auto is_data_object(object_status _s) noexcept -> bool;
    auto is_data_object(conn* _conn, const path& _p) -> bool;

    auto last_write_time(conn* _conn, const path& _p) -> std::time_t;
    auto last_write_time(conn* _conn, const path& _p, const std::time_t _new_time) -> void;

    auto remove(conn* _conn, const path& _p, remove_options _opts = remove_options::none) -> bool;
    auto remove_all(conn* _conn, const path& _p, remove_options _opts = remove_options::none) -> std::uintmax_t;

    //auto permissions(conn* _conn, const path& _p, perms _prms, perm_options opts = perm_options::replace) -> void;

    auto rename(conn* _conn, const path& _from, const path& _to) -> void;
    auto move(conn* _conn, const path& _from, const path& _to) -> void;

    //auto resize_data_object(conn* _conn, const path& _p, std::uintmax_t _size) -> void;

    //auto space(conn* _conn, const path& _p) -> space_info;

    auto status(conn* _conn, const path& _p) -> object_status;

    auto status_known(object_status _s) noexcept -> bool;

    //auto system_complete(conn* _conn, const path& _p) -> path;
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_FILESYSTEM_HPP
