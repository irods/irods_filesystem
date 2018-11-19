#ifndef IRODS_FILESYSTEM_PERMISSIONS_HPP
#define IRODS_FILESYSTEM_PERMISSIONS_HPP

namespace irods::filesystem
{
    enum class perms
    {
        null,
        read,
        write,
        own,
        inherit,
        noinherit
    };

    /*
    enum class perms
    {
        null                    = 1000,
        execute                 = 1010,
        read_annotation         = 1020,
        read_system_metadata    = 1030,
        read_metadata           = 1040,
        read_object             = 1050,
        write_annotation        = 1060,
        create_metadata         = 1070,
        modify_metadata         = 1080,
        delete_metadata         = 1090,
        administer_object       = 1100,
        create_object           = 1110,
        modify_object           = 1120,
        delete_object           = 1130,
        create_token            = 1140,
        delete_token            = 1150,
        curate                  = 1160,
        own                     = 1200,

        inherit                 = 9000,
        no_inherit              = 9010
    };
    */

    /*
    enum class perm_options
    {
        replace,
        add,
        remove
    };

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
    */
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_PERMISSIONS_HPP
