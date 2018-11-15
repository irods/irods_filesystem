#ifndef IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP
#define IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP

#include <irods/filesystem/filesystem.hpp>
#include <irods/filesystem/path.hpp>
#include <irods/filesystem/object_status.hpp>

#include <ctime>

namespace irods::filesystem
{
    class collection_entry
    {
    public:
        // Constructors and destructor
        collection_entry() = default;
        explicit collection_entry(const path& _p, object_status _os = {});

        // Copy-constructor and assignment
        collection_entry(const collection_entry& _other) = default;
        auto operator=(const collection_entry& _other) -> collection_entry& = default;

        ~collection_entry() = default;

        // Modifiers
        //auto assign(const path& _p, object_status _os = {}) -> void;
        //auto replace_object_name(const path& _p, object_status _os = {}) -> void;
        //auto refresh() -> void;

        // Observers
        operator const path&() const noexcept                   { return path_; }
        auto path() const noexcept -> const path&               { return path_; }
        auto exists() const noexcept -> bool                    { return filesystem::exists(status_); }
        auto is_data_object() const noexcept -> bool            { return filesystem::is_data_object(status_); }
        auto is_collection() const noexcept -> bool             { return filesystem::is_collection(status_); }
        auto is_other() const noexcept -> bool                  { return filesystem::is_other(status_); }
        auto data_object_size() -> std::uintmax_t;
        auto last_write_time() const noexcept -> std::time_t    { return mtime_; }
        auto status() const noexcept -> object_status           { return status_; }

        // Comparisons
        auto operator==(const collection_entry& _rhs) -> bool   { return path_ == _rhs.path_; }
        auto operator!=(const collection_entry& _rhs) -> bool   { return path_ != _rhs.path_; }
        auto operator< (const collection_entry& _rhs) -> bool   { return path_ <  _rhs.path_; }
        auto operator<=(const collection_entry& _rhs) -> bool   { return path_ <= _rhs.path_; }
        auto operator> (const collection_entry& _rhs) -> bool   { return path_ >  _rhs.path_; }
        auto operator>=(const collection_entry& _rhs) -> bool   { return path_ >= _rhs.path_; }

    private:
        class path path_;
        mutable object_status status_;
        mutable std::time_t mtime_;
    };
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP
