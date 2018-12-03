#ifndef IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP
#define IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP

#include <irods/filesystem/filesystem.hpp>
#include <irods/filesystem/path.hpp>
#include <irods/filesystem/object_status.hpp>

#include <chrono>

namespace irods::experimental::filesystem
{
    class collection_iterator;
    class recursive_collection_iterator;

    class collection_entry
    {
    public:
        // Observers

        // clang-format off
        operator const path&() const noexcept                           { return path_; }
        auto path() const noexcept -> const path&                       { return path_; }
        auto exists() const noexcept -> bool                            { return filesystem::exists(status_); }
        auto is_data_object() const noexcept -> bool                    { return filesystem::is_data_object(status_); }
        auto is_collection() const noexcept -> bool                     { return filesystem::is_collection(status_); }
        auto is_other() const noexcept -> bool                          { return filesystem::is_other(status_); }
        auto is_special_collection() const noexcept -> bool             { return false; }
        auto data_object_size() -> std::uintmax_t                       { return static_cast<std::uintmax_t>(data_size_); }
        auto created_at_time() const noexcept -> long long              { return ctime_; }
        auto last_write_time() const noexcept -> long long              { return mtime_; }
        auto status() const noexcept -> object_status                   { return status_; }
        auto replica_number() const noexcept -> int                     { return repl_number_; }
        auto replica_status() const noexcept -> int                     { return repl_status_; }
        auto data_mode() const noexcept -> unsigned                     { return data_mode_; }
        auto data_id() const noexcept -> const std::string&             { return data_id_; }
        auto checksum() const noexcept -> const std::string&            { return checksum_; }
        auto resource() const noexcept -> const std::string&            { return resc_; }
        auto resource_hierarchy() const noexcept -> const std::string&  { return resc_hier_; }
        auto physical_path() const noexcept -> const std::string&       { return phy_path_; }
        auto owner() const noexcept -> const std::string&               { return owner_; }
        auto data_type() const noexcept -> const std::string&           { return data_type_; }

        // Comparisons

        auto operator==(const collection_entry& _rhs) -> bool   { return path_ == _rhs.path_; }
        auto operator!=(const collection_entry& _rhs) -> bool   { return path_ != _rhs.path_; }
        auto operator< (const collection_entry& _rhs) -> bool   { return path_ <  _rhs.path_; }
        auto operator<=(const collection_entry& _rhs) -> bool   { return path_ <= _rhs.path_; }
        auto operator> (const collection_entry& _rhs) -> bool   { return path_ >  _rhs.path_; }
        auto operator>=(const collection_entry& _rhs) -> bool   { return path_ >= _rhs.path_; }
        // clang-format on

    private:
        friend class collection_iterator;
        friend class recursive_collection_iterator;

        mutable class path path_;
        mutable object_status status_;
        mutable int repl_number_;
        mutable int repl_status_;
        mutable unsigned data_mode_;
        mutable long data_size_;
        mutable std::string data_id_;
        mutable long long ctime_;
        mutable long long mtime_;
        mutable std::string checksum_;
        mutable std::string resc_;
        mutable std::string resc_hier_;
        mutable std::string phy_path_;
        mutable std::string owner_;
        mutable std::string data_type_;
        // mutable specColl_t spec_coll_;
    };
} // namespace irods::experimental::filesystem

#endif // IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP
