#ifndef IRODS_FILESYSTEM_HPP
#define IRODS_FILESYSTEM_HPP

#include <string>
#include <istream>
#include <ostream>
#include <ctime> // TODO Prefer chrono instead!
#include <cstdint>

namespace irods {
namespace filesystem {

class path;

bool lexicographical_compare(path::iterator _first1,
                             path::iterator _last1,
                             path::iterator _first2,
                             path::iterator _last2);

void swap(path&, _lhs, path& _rhs);

auto hash_value(const path& _p) -> std::size_t;

bool operator==(const path& _lhs, const path& _rhs);
bool operator!=(const path& _lhs, const path& _rhs);
bool operator< (const path& _lhs, const path& _rhs);
bool operator<=(const path& _lhs, const path& _rhs);
bool operator> (const path& _lhs, const path& _rhs);
bool operator>=(const path& _lhs, const path& _rhs);

bool operator/ (const path& _lhs, const path& _rhs);

std::ostream& operator<<(std::ostream& _os, const path& _p);
std::istream& operator>>(std::ostream& _is, const path& _p);

class filesystem_error;
class collection_entry;

class collection_iterator;

// Enable C++11 range-based for statements.
auto begin(const collection_iterator& _iter) -> const collection_iterator&;
auto end(const collection_iterator&) -> collection_iterator;

class recursive_collection_iterator;

// Enable C++11 range-based for statements.
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

class object_status;

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

//
// Operational Functions.
//

path absolute(const path& _p, const path& base = current_path());

path canonical(const path& _p, const path& base = current_path());

path copy(const path& _from, const path& _to);

path copy_collection(const path& _from, const path& _to);

path copy_data_object(const path& _from, const path& _to, copy_option _option = copy_option::overwrite_if_exists);

bool create_collections(const path& _p);
bool create_collection(const path& _p);

path current_path();
void current_path(const path& _p);

bool exists(object_status _s) noexcept;
bool exists(const path& _p);

bool equivalent(const path& _p1, const path& _p2);

auto data_object_size(const path& _p) -> std::uintmax_t;

auto initial_path() -> const path&;

bool is_collection(object_status _s) noexcept;
bool is_collection(const path& _p);

bool is_empty(const path& _p);

bool is_other(object_status _s) noexcept;
bool is_other(const path& _p);

bool is_data_object(object_status _s) noexcept;
bool is_data_object(const path& _p);

auto last_write_time(const path& _p) -> std::time_t;
void last_write_time(const path& _p, const std::time_t _new_time);

path relative(const path& _p, const path& _base = current_path());

bool remove(const path& _p);
auto remove_all(const path& _p) -> std::uintmax_t;

void rename(const path& _from, const path& _to);

void resize_data_object(const path& _p, std::uintmax_t _size);

auto space(const path& _p) -> space_info;

auto status(const path& _p) -> object_status;

bool status_known(object_status _s) noexcept;

path system_complete(const path& _p);

} // namespace filesystem
} // namespace irods

#endif // IRODS_FILESYSTEM_HPP
