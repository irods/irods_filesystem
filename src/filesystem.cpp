#include <irods/filesystem/filesystem.hpp>

#include <irods/filesystem/path.hpp>
#include <irods/filesystem/filesystem_error.hpp>
#include <irods/filesystem/detail.hpp>

#include <irods/rodsClient.h>
#include <irods/objStat.h>
#include <irods/openCollection.h>
#include <irods/closeCollection.h>
#include <irods/readCollection.h>
#include <irods/collection.hpp>
#include <irods/miscUtil.h>
#include <irods/rcMisc.h>
#include <irods/rmColl.h>
#include <irods/irods_log.hpp>
#include <irods/irods_error.hpp>
#include <irods/irods_exception.hpp>
#include <irods/irods_query.hpp>

#include <iostream>
#include <string>
#include <iterator>
#include <exception>

namespace
{
    struct stat_info
    {
        int error;
        long long size;
        int type;
        int mode;
        long id;
        char owner_name[128];
        char owner_zone[128];
        long long ctime;
        long long mtime;
    };

    auto stat(comm* _comm, const irods::filesystem::path& _p) -> stat_info
    {
        dataObjInp_t input{};
        std::strncpy(input.objPath, _p.c_str(), _p.string().size());

        rodsObjStat_t* stat_info_ptr{};
        stat_info si{};

        if (const auto ec = rcObjStat(_comm, &input, &stat_info_ptr); ec >= 0 && stat_info_ptr) {
            si.error = ec;
            si.size = stat_info_ptr->objSize;
            si.type = stat_info_ptr->objType;
            si.mode = static_cast<int>(stat_info_ptr->dataMode);
            //si.id = stat_info_ptr->dataId;
            std::strncpy(si.owner_name, stat_info_ptr->ownerName, strlen(stat_info_ptr->ownerName));
            std::strncpy(si.owner_zone, stat_info_ptr->ownerZone, strlen(stat_info_ptr->ownerZone));
            si.ctime = std::stoll(stat_info_ptr->createTime);
            si.mtime = std::stoll(stat_info_ptr->modifyTime);
            freeRodsObjStat(stat_info_ptr);
        }

        return si;
    }

    auto is_collection_empty(comm* _comm, const irods::filesystem::path& _p) -> bool
    {
        std::string sql = "select count(COLL_NAME) where COLL_PARENT_NAME = '";
        sql += _p;
        sql += "'";

        for (const auto& row : irods::query{_comm, sql}) {
            if (row.empty()) {
                throw irods::filesystem::filesystem_error{"empty resultset returned"};
            }

            if (std::stoi(row[0]) > 0) {
                return false;
            }
        }

        return true;
    }

    struct remove_impl_options
    {
        bool no_trash    = false;
        bool recursive   = false;
    };

    auto remove_impl(comm* _comm, const irods::filesystem::path& _p, remove_impl_options _opts) -> bool
    {
        irods::filesystem::detail::throw_if_path_length_exceeds_limit(_p);

        if (exists(_comm, _p)) {
            if (const auto s = status(_comm, _p); is_data_object(s)) {
                dataObjInp_t input{};
                std::strncpy(input.objPath, _p.c_str(), _p.string().size());

                if (_opts.no_trash) {
                    addKeyVal(&input.condInput, FORCE_FLAG_KW, "");
                }

                return rcDataObjUnlink(_comm, &input) == 0;
            }
            else if (is_collection(s)) {
                if (!_opts.recursive && !is_collection_empty(_comm, _p)) {
                    throw irods::filesystem::filesystem_error{"cannot remove non-empty collection"};
                }

                collInp_t input{};
                std::strncpy(input.collName, _p.c_str(), _p.string().size());

                if (_opts.no_trash) {
                    addKeyVal(&input.condInput, FORCE_FLAG_KW, "");
                }

                if (_opts.recursive) {
                    addKeyVal(&input.condInput, RECURSIVE_OPR__KW, "");
                }

                constexpr int verbose = 0;
                return rcRmColl(_comm, &input, verbose) >= 0;
            }
            else if (is_other(s)) {
                throw irods::filesystem::filesystem_error{"unknown object type"};
            }
        }

        return false;
    }
} // anonymous namespace

namespace irods::filesystem
{
    // Operational functions

    //auto absolute(comm* _comm, const path& _p, const path& base = current_path()) -> path;

    //auto canonical(comm* _comm, const path& _p, const path& base = current_path()) -> path;
    //auto weakly_canonical(comm* _comm, const path& _p, const path& base = current_path()) -> path;

    //auto relative(comm* _comm, const path& _p, const path& _base = current_path()) -> path;
    //auto proximate(comm* _comm, const path& _p, const path& _base = current_path()) -> path;

    auto copy(comm* _comm, const path& _from, const path& _to, copy_options _option) -> void
    {

    }

    auto copy_collection(comm* _comm, const path& _from, const path& _to, copy_options _option) -> bool
    {
        return false;
    }

    auto copy_data_object(comm* _comm, const path& _from, const path& _to, copy_options _option) -> bool
    {
        return false;
    }

    auto create_collection(comm* _comm, const path& _p) -> bool // Implies perms::all
    {
        if (exists(_comm, _p)) {
            return false;
        }

        char buf[1024]{};
        std::strncpy(buf, _p.c_str(), _p.string().size());
        return mkColl(_comm, buf) == 0;

        /*
        if (const auto ec = mkColl(_comm, _p.lexically_normal().c_str()); ec != 0)
        {
            throw filesystem_error{};
        }

        return true;
        */
    }

    auto create_collection(comm* _comm, const path& _p, const path& _existing_p) -> bool
    {
        return false;
    }

    auto create_collections(comm* _comm, const path& _p) -> bool
    {
        if (exists(_comm, _p)) {
            return false;
        }

        bool created_one = false;
        path tree;

        for (const auto& e : _p) {
            tree /= e;
            if (create_collection(_comm, tree)) {
                created_one = true;
            }
        }

        return created_one;
    }

    //auto current_path(comm* _comm) -> path;
    //auto current_path(comm* _comm, const path& _p) -> void;

    auto exists(object_status _s) noexcept -> bool
    {
        return status_known(_s) && _s.type() != object_type::not_found;
    }

    auto exists(comm* _comm, const path& _p) -> bool
    {
        return exists(status(_comm, _p));
    }

    auto equivalent(const path& _p1, const path& _p2) -> bool
    {
        return _p1.lexically_normal() == _p2.lexically_normal();
    }

    auto data_object_size(comm* _comm, const path& _p) -> std::uintmax_t
    {
        if (is_data_object(_comm, _p)) {
            if (auto info = stat(_comm, _p); info.error >= 0) {
                return static_cast<std::uintmax_t>(info.size);
            }
        }

        return 0;
    }

    auto is_collection(object_status _s) noexcept -> bool
    {
        return _s.type() == object_type::collection;
    }

    auto is_collection(comm* _comm, const path& _p) -> bool
    {
        return is_collection(status(_comm, _p));
    }

    auto is_empty(comm* _comm, const path& _p) -> bool
    {
        if (const auto s = status(_comm, _p); is_data_object(s)) {
            return data_object_size(_comm, _p) == 0;
        }
        else if (is_collection(s)) {
            return is_collection_empty(_comm, _p);
        }

        throw filesystem_error{"unknown object type"};
    }

    auto is_other(object_status _s) noexcept -> bool
    {
        return _s.type() == object_type::unknown;
    }

    auto is_other(comm* _comm, const path& _p) -> bool
    {
        return is_data_object(status(_comm, _p));
    }

    auto is_data_object(object_status _s) noexcept -> bool
    {
        return _s.type() == object_type::data_object;
    }

    auto is_data_object(comm* _comm, const path& _p) -> bool
    {
        return is_data_object(status(_comm, _p));
    }

    auto last_write_time(comm* _comm, const path& _p) -> std::time_t
    {
        return {};
    }

    auto last_write_time(comm* _comm, const path& _p, const std::time_t _new_time) -> void
    {

    }

    auto remove(comm* _comm, const path& _p, remove_options _opts) -> bool
    {
        const auto no_trash = (remove_options::no_trash == _opts);
        constexpr auto recursive = false;

        return remove_impl(_comm, _p, {no_trash, recursive});
    }

    auto remove_all(comm* _comm, const path& _p, remove_options _opts) -> std::uintmax_t
    {
        const auto normal_p = _p.lexically_normal();

        std::string data_obj_sql = "select count(DATA_ID) where COLL_NAME like '";
        data_obj_sql += normal_p;
        data_obj_sql += "%'";

        std::string colls_sql = "select count(COLL_ID) where COLL_NAME like '";
        colls_sql += normal_p;
        colls_sql += "%'";

        std::uintmax_t count = 0;

        for (const auto& sql : {data_obj_sql, colls_sql}) {
            for (const auto& row : irods::query{_comm, sql}) {
                if (row.empty()) {
                    continue;
                }

                count += std::stoull(row[0]);
            }
        }

        if (0 == count) {
            return 0;
        }

        const auto no_trash = (remove_options::no_trash == _opts);
        constexpr auto recursive = true;

        if (remove_impl(_comm, _p, {no_trash, recursive})) {
            return count;
        }

        return 0;
    }

    auto permissions(comm* _comm, const path& _p, perms _prms, perm_options _opts) -> void
    {

    }

    auto rename(comm* _comm, const path& _old_p, const path& _new_p) -> void
    {
        if (_old_p.empty()) {
            throw filesystem_error{"source path cannot be empty"};
        }

        if (_new_p.empty()) {
            throw filesystem_error{"destination path cannot be empty"};
        }

        detail::throw_if_path_length_exceeds_limit(_old_p);
        detail::throw_if_path_length_exceeds_limit(_new_p);

        // Case 1: "_new_p" is the same object as "_old_p".
        if (equivalent(_old_p, _new_p)) {
            return;
        }

        /*
        if (auto s = status(_comm, _old_p); is_data_object(s)) {
            // Case 2: "_new_p" is an existing non-collection object.
            if (exists(_comm, _new_p)) {
                if (!is_data_object(_comm, _new_p)) {
                    throw filesystem_error{R"_("_new_p" must be a data object)_"};
                }


            }
            // Case 3: "_new_p" is a non-existing data object in an existing collection.
            else {

            }
        }
        else if (is_collection(s)) {
            // Case 2: "_new_p" is an existing collection.
            if (exists(_comm, _new_p)) {
                if (!is_collection(_comm, _new_p)) {
                    throw filesystem_error{R"_("_new_p" must be a collection)_"};
                }


            }
            // Case 3: "_new_p" is a non-existing collection w/ the following requirements:
            //  1. Does not end with a collection separator.
            //  2. The parent collection must exist.
            else if (!detail::is_separator(_new_p.string().back()) && exists(_comm, _new_p.parent_path())) {

            }
        }
        */

        /*
        if (_old_p.empty()) {
            throw filesystem_error{"source path cannot be empty"};
        }

        if (_new_p.empty()) {
            throw filesystem_error{"destination path cannot be empty"};
        }

        detail::throw_if_path_length_exceeds_limit(_old_p);
        detail::throw_if_path_length_exceeds_limit(_new_p);

        if (_new_p.object_name_is_dot() || _new_p.object_name_is_dot_dot()) {
            throw filesystem_error{R"_(destination path cannot be "." or "..")_"};
        }

        if (is_collection(_comm, _old_p) &&
            lexicographical_compare(std::begin(_old_p), std::end(_old_p),
                                    std::begin(_new_p), std::end(_new_p)))
        {
            throw filesystem_error{"_old_p cannot be an ancestor of _new_p"};
        }

        if (!exists(_comm, _old_p)) {
            throw filesystem_error{"source path does not exist"};
        }

        if (equivalent(_old_p, _new_p)) {
            return;
        }
        */

        dataObjCopyInp_t input{};
        std::strncpy(input.srcDataObjInp.objPath, _old_p.c_str(), _old_p.string().size());
        std::strncpy(input.destDataObjInp.objPath, _new_p.c_str(), _new_p.string().size());

        if (const auto ec = rcDataObjRename(_comm, &input); ec < 0) {
            throw filesystem_error{"rename error [ec = " + std::to_string(ec) + "]"};
        }
    }

    auto move(comm* _comm, const path& _old_p, const path& _new_p) -> void
    {
        rename(_comm, _old_p, _new_p);
    }

    auto resize_data_object(comm* _comm, const path& _p, std::uintmax_t _size) -> void
    {

    }

    //auto space(comm* _comm, const path& _p) -> space_info;

    auto status(comm* _comm, const path& _p) -> object_status
    {
        auto s = stat(_comm, _p);
        object_status status;

        switch (s.type) {
            case DATA_OBJ_T:
                status.type(object_type::data_object);
                break;

            case COLL_OBJ_T:
                status.type(object_type::collection);
                break;

            // This case indicates that iRODS does not contain a data object or
            // collection with the provided path "_p".
            case UNKNOWN_OBJ_T:
                status.type(object_type::not_found);
                break;

            default:
                status.type(object_type::none);
                break;
        }
        
        return status;
    }

    auto status_known(object_status _s) noexcept -> bool
    {
        return _s.type() != object_type::none;
    }

    //auto system_complete(comm* _comm, const path& _p) -> path;
} // namespace irods::filesystem

