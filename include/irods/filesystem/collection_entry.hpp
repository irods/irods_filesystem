#ifndef IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP
#define IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP

#include <irods/filesystem/filesystem.hpp>
#include <irods/filesystem/path.hpp>
#include <irods/filesystem/object_status.hpp>

namespace irods::filesystem
{
    class collection_entry
    {
    public:
        // Constructors and destructor
        collection_entry();
        explicit collection_entry(const path& _p, object_status _os = {});

        // Copy-constructor and assignment
        collection_entry(const collection_entry& _other);
        auto operator=(const collection_entry& _other) -> collection_entry&;

        ~collection_entry();

        // Modifiers

        auto assign(const path& _p, object_status _os = {}) -> void;
        auto replace_data_object_name(const path& _p, object_status _os = {}) -> void;

        // Observers

        auto path() const -> const path&;
        auto status() const -> object_status;

        auto operator==(const collection_entry& _rhs) -> bool;
        auto operator!=(const collection_entry& _rhs) -> bool;
        auto operator< (const collection_entry& _rhs) -> bool;
        auto operator<=(const collection_entry& _rhs) -> bool;
        auto operator> (const collection_entry& _rhs) -> bool;
        auto operator>=(const collection_entry& _rhs) -> bool;

    private:
        path path_;
        mutable object_status status_;
    };
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP
