#ifndef IRODS_FILESYSTEM_OBJECT_STATUS_HPP
#define IRODS_FILESYSTEM_OBJECT_STATUS_HPP

#include <irods/filesystem/filesystem.hpp>
#include <irods/filesystem/permissions.hpp>

namespace irods::experimental::filesystem
{
    enum class object_type
    {
        none,
        not_found,
        data_object,
        collection,
        special_collection,
        unknown
        //status_error,
        //object_not_found,
        //special_object,
        //type_unknown
    };

    class object_status
    {
    public:
        object_status() noexcept = default;

        //explicit object_status(object_type _ot, perms _prms = perms::unknown) noexcept
        explicit object_status(object_type _ot, perms _prms = perms::null) noexcept
            : type_{_ot}
            , perms_{_prms}
        {
        }

        // Compiler generated
        object_status(const object_status& _other) noexcept = default;
        auto operator=(const object_status& _other) noexcept -> object_status& = default;

        ~object_status() noexcept = default;

        // Observers
        auto type() const noexcept -> object_type   { return type_; }
        auto permissions() const noexcept -> perms  { return perms_; }

        // Modifiers
        auto type(object_type _ot) noexcept -> void     { type_ = _ot; }
        auto permissions(perms _prms) noexcept -> void  { perms_ = _prms; }

    private:
        object_type type_;
        perms perms_;
    };
} // namespace irods::experimental::filesystem

#endif // IRODS_FILESYSTEM_OBJECT_STATUS_HPP
