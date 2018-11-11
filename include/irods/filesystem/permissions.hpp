#ifndef IRODS_FILESYSTEM_PERMISSIONS_HPP
#define IRODS_FILESYSTEM_PERMISSIONS_HPP

namespace irods::filesystem
{
    enum class perms
    {
        none,
        owner_read,
        owner_write,
        owner_exec,
        owner_all,
        group_read,
        group_write,
        group_exec,
        group_all,
        others_read,
        others_write,
        others_exec,
        others_all,
        all,
        set_uid,
        set_gid,
        sticky_bit,
        mask,
        unknown
    };
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_PERMISSIONS_HPP
