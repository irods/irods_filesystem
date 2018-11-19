#ifndef IRODS_FILESYSTEM_FILESYSTEM_HPP
#define IRODS_FILESYSTEM_FILESYSTEM_HPP

#include <irods/filesystem/config.hpp>
#include <irods/filesystem/object_status.hpp>
#include <irods/filesystem/permissions.hpp>
#include <irods/filesystem/copy_options.hpp>

#include <irods/rcConnect.h>

#include <string>
#include <istream>
#include <ostream>
#include <chrono>
#include <cstdint>

namespace irods::filesystem
{
    class path;

    using object_time_type = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;

    /*
    struct space_info // Returned by space function.
    {
        std::uintmax_t capacity;
        std::uintmax_t free;
        std::uintmax_t available; // Free space available to non-privileged process.
    };
    */

    enum class remove_options : std::uint8_t
    {
        none, // Moves objects to trash.
        no_trash
    };

    // Operational functions

    //auto absolute(rxrxConn* _conn, const path& _p, const path& base = current_path()) -> path;

    //auto canonical(rxConn* _conn, const path& _p, const path& base = current_path()) -> path;
    //auto weakly_canonical(rxConn* _conn, const path& _p, const path& base = current_path()) -> path;

    //auto relative(rxConn* _conn, const path& _p, const path& _base = current_path()) -> path;
    //auto proximate(rxConn* _conn, const path& _p, const path& _base = current_path()) -> path;

    auto copy(rxConn* _conn, const path& _from, const path& _to, copy_options _options = copy_options::none) -> void;
    auto copy_data_object(rxConn* _conn, const path& _from, const path& _to, copy_options _options = copy_options::none) -> bool;
    //auto copy_collection(rxConn* _conn, const path& _from, const path& _to, copy_options _options = copy_options::none) -> bool;

    auto create_collection(rxConn* _conn, const path& _p) -> bool; // Implies perms::all
    //auto create_collection(rxConn* _conn, const path& _p, const path& _existing_p) -> bool;
    auto create_collections(rxConn* _conn, const path& _p) -> bool;

    //auto current_path(rxConn* _conn) -> path;
    //auto current_path(rxConn* _conn, const path& _p) -> void;

    auto exists(object_status _s) noexcept -> bool;
    auto exists(rxConn* _conn, const path& _p) -> bool;

    auto equivalent(const path& _p1, const path& _p2) -> bool;

    auto data_object_size(rxConn* _conn, const path& _p) -> std::uintmax_t;

    auto is_collection(object_status _s) noexcept -> bool;
    auto is_collection(rxConn* _conn, const path& _p) -> bool;

    auto is_empty(rxConn* _conn, const path& _p) -> bool;

    auto is_other(object_status _s) noexcept -> bool;
    auto is_other(rxConn* _conn, const path& _p) -> bool;

    auto is_data_object(object_status _s) noexcept -> bool;
    auto is_data_object(rxConn* _conn, const path& _p) -> bool;

    auto last_write_time(rxConn* _conn, const path& _p) -> object_time_type;
    auto last_write_time(rxConn* _conn, const path& _p, object_time_type _new_time) -> void;

    auto remove(rxConn* _conn, const path& _p, remove_options _opts = remove_options::none) -> bool;
    auto remove_all(rxConn* _conn, const path& _p, remove_options _opts = remove_options::none) -> std::uintmax_t;

    //auto permissions(rxConn* _conn, const path& _p, perms _prms, perm_options opts = perm_options::replace) -> void;
    auto permissions(rxConn* _conn, const path& _p, perms _prms) -> void;

    auto rename(rxConn* _conn, const path& _from, const path& _to) -> void;
    auto move(rxConn* _conn, const path& _from, const path& _to) -> void;

    //auto resize_data_object(rxConn* _conn, const path& _p, std::uintmax_t _size) -> void;

    //auto space(rxConn* _conn, const path& _p) -> space_info;

    auto status(rxConn* _conn, const path& _p) -> object_status;

    auto status_known(object_status _s) noexcept -> bool;

    //auto system_complete(rxConn* _conn, const path& _p) -> path;
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_FILESYSTEM_HPP
