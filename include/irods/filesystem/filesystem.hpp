#ifndef IRODS_FILESYSTEM_FILESYSTEM_HPP
#define IRODS_FILESYSTEM_FILESYSTEM_HPP

#include <irods/filesystem/object_status.hpp>
#include <irods/filesystem/permissions.hpp>

#include <string>
#include <istream>
#include <ostream>
#include <chrono>
#include <ctime> // TODO Prefer chrono instead!
#include <cstdint>

#include <irods/rcConnect.h>

#ifdef RODS_SERVER
using comm = rsComm_t;
#else
using comm = rcComm_t;
#endif // RODS_SERVER

namespace irods::filesystem
{
    class path;
    class filesystem_error;
    class collection_entry;
    class collection_iterator;
    class recursive_collection_iterator;
    //class object_status;

    // Enable C++11 range-based for statements.

    auto begin(const recursive_collection_iterator& _iter) -> const recursive_collection_iterator&;
    auto end(const recursive_collection_iterator&) -> recursive_collection_iterator;

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
        collections_only
        //fail_if_exists = none
    };

    enum class remove_options : std::uint8_t
    {
        none, // Moves objects to trash.
        no_trash
    };

    // Operational functions

    //auto absolute(comm* _comm, const path& _p, const path& base = current_path()) -> path;

    //auto canonical(comm* _comm, const path& _p, const path& base = current_path()) -> path;
    //auto weakly_canonical(comm* _comm, const path& _p, const path& base = current_path()) -> path;

    //auto relative(comm* _comm, const path& _p, const path& _base = current_path()) -> path;
    //auto proximate(comm* _comm, const path& _p, const path& _base = current_path()) -> path;

    auto copy(comm* _comm, const path& _from, const path& _to, copy_options _option = copy_options::none) -> void;
    auto copy_collection(comm* _comm, const path& _from, const path& _to, copy_options _option = copy_options::none) -> bool;
    auto copy_data_object(comm* _comm, const path& _from, const path& _to, copy_options _option = copy_options::none) -> bool;

    auto create_collection(comm* _comm, const path& _p) -> bool; // Implies perms::all
    auto create_collection(comm* _comm, const path& _p, const path& _existing_p) -> bool;
    auto create_collections(comm* _comm, const path& _p) -> bool;

    //auto current_path(comm* _comm) -> path;
    //auto current_path(comm* _comm, const path& _p) -> void;

    auto exists(object_status _s) noexcept -> bool;
    auto exists(comm* _comm, const path& _p) -> bool;

    auto equivalent(const path& _p1, const path& _p2) -> bool;

    auto data_object_size(comm* _comm, const path& _p) -> std::uintmax_t;

    auto is_collection(object_status _s) noexcept -> bool;
    auto is_collection(comm* _comm, const path& _p) -> bool;

    auto is_empty(comm* _comm, const path& _p) -> bool;

    auto is_other(object_status _s) noexcept -> bool;
    auto is_other(comm* _comm, const path& _p) -> bool;

    auto is_data_object(object_status _s) noexcept -> bool;
    auto is_data_object(comm* _comm, const path& _p) -> bool;

    auto last_write_time(comm* _comm, const path& _p) -> std::time_t;
    auto last_write_time(comm* _comm, const path& _p, const std::time_t _new_time) -> void;

    auto remove(comm* _comm, const path& _p, remove_options _opts = remove_options::none) -> bool;
    auto remove_all(comm* _comm, const path& _p, remove_options _opts = remove_options::none) -> std::uintmax_t;

    //auto permissions(comm* _comm, const path& _p, perms _prms, perm_options opts = perm_options::replace) -> void;

    auto rename(comm* _comm, const path& _from, const path& _to) -> void;
    auto move(comm* _comm, const path& _from, const path& _to) -> void;

    //auto resize_data_object(comm* _comm, const path& _p, std::uintmax_t _size) -> void;

    //auto space(comm* _comm, const path& _p) -> space_info;

    auto status(comm* _comm, const path& _p) -> object_status;

    auto status_known(object_status _s) noexcept -> bool;

    //auto system_complete(comm* _comm, const path& _p) -> path;
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_FILESYSTEM_HPP
