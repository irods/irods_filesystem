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

    auto stat(conn* _conn, const irods::filesystem::path& _p) -> stat_info
    {
        dataObjInp_t input{};
        std::strncpy(input.objPath, _p.c_str(), _p.string().size());

        rodsObjStat_t* stat_info_ptr{};
        stat_info si{};

        if (const auto ec = rcObjStat(_conn, &input, &stat_info_ptr); ec >= 0 && stat_info_ptr) {
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

    auto is_collection_empty(conn* _conn, const irods::filesystem::path& _p) -> bool
    {
        std::string sql = "select count(COLL_NAME) where COLL_PARENT_NAME = '";
        sql += _p;
        sql += "'";

        for (const auto& row : irods::query{_conn, sql}) {
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

    auto remove_impl(conn* _conn, const irods::filesystem::path& _p, remove_impl_options _opts) -> bool
    {
        irods::filesystem::detail::throw_if_path_length_exceeds_limit(_p);

        if (exists(_conn, _p)) {
            if (const auto s = status(_conn, _p); is_data_object(s)) {
                dataObjInp_t input{};
                std::strncpy(input.objPath, _p.c_str(), _p.string().size());

                if (_opts.no_trash) {
                    addKeyVal(&input.condInput, FORCE_FLAG_KW, "");
                }

                return rcDataObjUnlink(_conn, &input) == 0;
            }
            else if (is_collection(s)) {
                if (!_opts.recursive && !is_collection_empty(_conn, _p)) {
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
                return rcRmColl(_conn, &input, verbose) >= 0;
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

    //auto absolute(conn* _conn, const path& _p, const path& base = current_path()) -> path;

    //auto canonical(conn* _conn, const path& _p, const path& base = current_path()) -> path;
    //auto weakly_canonical(conn* _conn, const path& _p, const path& base = current_path()) -> path;

    //auto relative(conn* _conn, const path& _p, const path& _base = current_path()) -> path;
    //auto proximate(conn* _conn, const path& _p, const path& _base = current_path()) -> path;

    auto copy(conn* _conn, const path& _from, const path& _to, copy_options _options) -> void
    {
        const auto from_status = status(_conn, _from);

        if (!exists(from_status)) {
            throw filesystem_error{"object does not exist [path => " + _from + ']'};
        }

        if (equivalent(_from, _to)) {
            throw filesystem_error{"the source and destination are the same"};
        }

        if (status_known(from_status) && is_other(from_status)) {
            throw filesystem_error{"object is not a collection or data object [path => " + _from + ']'};
        }

        const auto to_status = status(_conn, _to);
        
        if (status_known(to_status) && is_other(to_status)) {
            throw filesystem_error{"object is not a collection or data object [path => " + _to + ']'};
        }

        if (is_collection(from_status) && is_data_object(to_status)) {
            throw filesystem_error{"cannot copy a collection into a data object"};
        }

        if (is_data_object(from_status)) {
            if (copy_options::collections_only == _options) {
                return false;
            }

            if (is_collection(to_status)) {
                return copy_data_object(_conn, _from, _to / _from.object_name(), _options);
            }

            return copy_data_object(_conn, _from, _to, _options);
        }

        if (is_collection(from_status)) {
            if (copy_options::recursive == _options || copy_options::none == _options) {
                if (!exists(to_status)) {
                    //if (!create_collection(_conn, _to, _from)) {
                    if (!create_collection(_conn, _to)) {
                        throw filesystem_error{"error creating collection [path => " + _to + ']'};
                    }
                }

                for (const auto& e : collection_iterator{_conn, _from}) {
                    const auto opts = _options | copy_options::in_recursive_copy;
                    if (!copy(_conn, e.path(), _to / e.path().object_name(), opts)) {
                        throw filesystem_error{"error copying content"};
                    }
                }

                return true;
            }
        }

        return false;
    }

    auto copy_data_object(conn* _conn, const path& _from, const path& _to, copy_options _options) -> bool
    {
        return false;
    }

    /*
    auto copy_collection(conn* _conn, const path& _from, const path& _to, copy_options _options) -> bool
    {
        return false;
    }
    */

    auto create_collection(conn* _conn, const path& _p) -> bool // Implies perms::all
    {
        if (exists(_conn, _p)) {
            return false;
        }

        char buf[1024]{};
        std::strncpy(buf, _p.c_str(), _p.string().size());

        return mkColl(_conn, buf) == 0;
    }

    auto create_collection(conn* _conn, const path& _p, const path& _existing_p) -> bool
    {
        return false;
    }

    auto create_collections(conn* _conn, const path& _p) -> bool
    {
        if (exists(_conn, _p)) {
            return false;
        }

        bool created_one = false;
        path tree;

        for (const auto& e : _p) {
            tree /= e;
            if (create_collection(_conn, tree)) {
                created_one = true;
            }
        }

        return created_one;
    }

    //auto current_path(conn* _conn) -> path;
    //auto current_path(conn* _conn, const path& _p) -> void;

    auto exists(object_status _s) noexcept -> bool
    {
        return status_known(_s) && _s.type() != object_type::not_found;
    }

    auto exists(conn* _conn, const path& _p) -> bool
    {
        return exists(status(_conn, _p));
    }

    auto equivalent(const path& _p1, const path& _p2) -> bool
    {
        return _p1.lexically_normal() == _p2.lexically_normal();
    }

    auto data_object_size(conn* _conn, const path& _p) -> std::uintmax_t
    {
        if (is_data_object(_conn, _p)) {
            if (auto info = stat(_conn, _p); info.error >= 0) {
                return static_cast<std::uintmax_t>(info.size);
            }
        }

        return 0;
    }

    auto is_collection(object_status _s) noexcept -> bool
    {
        return _s.type() == object_type::collection;
    }

    auto is_collection(conn* _conn, const path& _p) -> bool
    {
        return is_collection(status(_conn, _p));
    }

    auto is_empty(conn* _conn, const path& _p) -> bool
    {
        if (const auto s = status(_conn, _p); is_data_object(s)) {
            return data_object_size(_conn, _p) == 0;
        }
        else if (is_collection(s)) {
            return is_collection_empty(_conn, _p);
        }

        throw filesystem_error{"unknown object type"};
    }

    auto is_other(object_status _s) noexcept -> bool
    {
        return _s.type() == object_type::unknown;
    }

    auto is_other(conn* _conn, const path& _p) -> bool
    {
        return is_data_object(status(_conn, _p));
    }

    auto is_data_object(object_status _s) noexcept -> bool
    {
        return _s.type() == object_type::data_object;
    }

    auto is_data_object(conn* _conn, const path& _p) -> bool
    {
        return is_data_object(status(_conn, _p));
    }

    auto last_write_time(conn* _conn, const path& _p) -> std::time_t
    {
        return {};
    }

    auto last_write_time(conn* _conn, const path& _p, const std::time_t _new_time) -> void
    {

    }

    auto remove(conn* _conn, const path& _p, remove_options _opts) -> bool
    {
        const auto no_trash = (remove_options::no_trash == _opts);
        constexpr auto recursive = false;

        return remove_impl(_conn, _p, {no_trash, recursive});
    }

    auto remove_all(conn* _conn, const path& _p, remove_options _opts) -> std::uintmax_t
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
            for (const auto& row : irods::query{_conn, sql}) {
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

        if (remove_impl(_conn, _p, {no_trash, recursive})) {
            return count;
        }

        return 0;
    }

    auto permissions(conn* _conn, const path& _p, perms _prms, perm_options _opts) -> void
    {

    }

    auto rename(conn* _conn, const path& _old_p, const path& _new_p) -> void
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
        if (auto s = status(_conn, _old_p); is_data_object(s)) {
            // Case 2: "_new_p" is an existing non-collection object.
            if (exists(_conn, _new_p)) {
                if (!is_data_object(_conn, _new_p)) {
                    throw filesystem_error{R"_("_new_p" must be a data object)_"};
                }


            }
            // Case 3: "_new_p" is a non-existing data object in an existing collection.
            else {

            }
        }
        else if (is_collection(s)) {
            // Case 2: "_new_p" is an existing collection.
            if (exists(_conn, _new_p)) {
                if (!is_collection(_conn, _new_p)) {
                    throw filesystem_error{R"_("_new_p" must be a collection)_"};
                }


            }
            // Case 3: "_new_p" is a non-existing collection w/ the following requirements:
            //  1. Does not end with a collection separator.
            //  2. The parent collection must exist.
            else if (!detail::is_separator(_new_p.string().back()) && exists(_conn, _new_p.parent_path())) {

            }
        }
        */

        dataObjCopyInp_t input{};
        std::strncpy(input.srcDataObjInp.objPath, _old_p.c_str(), _old_p.string().size());
        std::strncpy(input.destDataObjInp.objPath, _new_p.c_str(), _new_p.string().size());

        if (const auto ec = rcDataObjRename(_conn, &input); ec < 0) {
            throw filesystem_error{"rename error [ec = " + std::to_string(ec) + "]"};
        }
    }

    auto move(conn* _conn, const path& _old_p, const path& _new_p) -> void
    {
        rename(_conn, _old_p, _new_p);
    }

    auto resize_data_object(conn* _conn, const path& _p, std::uintmax_t _size) -> void
    {

    }

    //auto space(conn* _conn, const path& _p) -> space_info;

    auto status(conn* _conn, const path& _p) -> object_status
    {
        auto s = stat(_conn, _p);
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

    //auto system_complete(conn* _conn, const path& _p) -> path;
} // namespace irods::filesystem

