#ifndef IRODS_FILESYSTEM_OBJECT_STATUS_HPP
#define IRODS_FILESYSTEM_OBJECT_STATUS_HPP

#include <irods/filesystem/filesystem.hpp>

namespace irods {
namespace filesystem {

class object_status
{
public:
    object_status() noexcept;
    explicit object_status(object_type _ot, perms _prms = perms_not_known) noexcept;

    // Compiler generated
    object_status(const object_status& _other) noexcept;
    auto operator=(const object_status& _other) noexcept -> object_status&;

    ~object_status() noexcept;

    // Observers
    auto type() const noexcept -> object_type;
    auto permissions() const noexcept -> perms;

    // Modifiers
    void type(object_type _ot) noexcept;
    void permissions(perms _prms) noexcept;

private:
};

} // namespace filesystem
} // namespace irods

#endif // IRODS_FILESYSTEM_OBJECT_STATUS_HPP
