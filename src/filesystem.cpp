#include <irods/filesystem/filesystem.hpp>

#include <irods/filesystem/path.hpp>
#include <irods/filesystem/collection_iterator.hpp>
#include <irods/filesystem/filesystem_error.hpp>
#include <irods/filesystem/detail.hpp>

#if defined(RODS_SERVER) || defined(RODS_CLERVER)
    //#include <irods/rodsClient.h>
    #include <irods/rods.h>
    #include <irods/apiHeaderAll.h>
    #include <irods/rsObjStat.hpp>
    #include <irods/rsDataObjCopy.hpp>
    #include <irods/rsDataObjRename.hpp>
    #include <irods/rsDataObjUnlink.hpp>
    #include <irods/rsOpenCollection.hpp>
    #include <irods/rsCloseCollection.hpp>
    #include <irods/rsReadCollection.hpp>
    #include <irods/rsModAccessControl.hpp>
    #include <irods/rsCollCreate.hpp>
    //#include <irods/collection.hpp>
    //#include <irods/miscUtil.h>
    #include <irods/rsModColl.hpp>
    #include <irods/rsRmColl.hpp>
#else
    #include <irods/rodsClient.h>
    #include <irods/objStat.h>
    #include <irods/dataObjCopy.h>
    #include <irods/dataObjRename.h>
    #include <irods/dataObjUnlink.h>
    #include <irods/openCollection.h>
    #include <irods/closeCollection.h>
    #include <irods/readCollection.h>
    #include <irods/modAccessControl.h>
    #include <irods/collCreate.h>
    //#include <irods/collection.hpp>
    //#include <irods/miscUtil.h>
    #include <irods/modColl.h>
    #include <irods/rmColl.h>
#endif // RODS_SERVER or RODS_CLERVER

// TODO miscUtil.h and collCreate.h have mkColl implementations.
// Which one should be used?
// TODO Is collection.hpp needed? It also has collection functions.

#include <irods/rcMisc.h>
#include <irods/irods_log.hpp>
#include <irods/irods_error.hpp>
#include <irods/irods_exception.hpp>
#include <irods/irods_query.hpp>
#include <irods/irods_at_scope_exit.hpp>

#include <iostream>
#include <string>
#include <iterator>
#include <exception>
#include <sstream>
#include <iomanip>

namespace irods::experimental::filesystem
{
    namespace
    {
#if defined(RODS_SERVER) || defined(RODS_CLERVER)
        int rsDataObjCopy(rsComm_t* _conn, dataObjCopyInp_t* _dataObjCopyInp)
        {
            //_dataObjCopyInp->srcDataObjInp.oprType = COPY_SRC;
            //_dataObjCopyInp->destDataObjInp.oprType = COPY_DEST;

            transferStat_t* ts_ptr{};

            const auto ec = rsDataObjCopy(_conn, _dataObjCopyInp, &ts_ptr);

            if (ts_ptr) {
                std::free(ts_ptr);
            }

            return ec;
        }

        const auto rsRmColl = [](rsComm_t* _conn, collInp_t* _rmCollInp, int) -> int
        {
            collOprStat_t* stat{};
            return ::rsRmColl(_conn, _rmCollInp, &stat);
        };
#endif // RODS_SERVER or RODS_CLERVER

        struct stat_info
        {
            int error;
            long long size;
            int type;
            int mode;
            long id;
            char owner_name[NAME_LEN];
            char owner_zone[NAME_LEN];
            long long ctime;
            long long mtime;
            perms prms;
        };

        auto stat(rxConn* _conn, const path& _p) -> stat_info
        {
            dataObjInp_t input{};
            std::strncpy(input.objPath, _p.c_str(), _p.string().size());

            rodsObjStat_t* stat_info_ptr{};
            stat_info si{};

            if (const auto ec = rxObjStat(_conn, &input, &stat_info_ptr); ec >= 0 && stat_info_ptr) {
                irods::at_scope_exit<std::function<void()>> {[stat_info_ptr] {
                    freeRodsObjStat(stat_info_ptr);
                }};

                si.error = ec;
                si.size = stat_info_ptr->objSize;
                si.type = stat_info_ptr->objType;
                si.mode = static_cast<int>(stat_info_ptr->dataMode);
                si.id = std::stol(stat_info_ptr->dataId);
                std::strncpy(si.owner_name, stat_info_ptr->ownerName, strlen(stat_info_ptr->ownerName));
                std::strncpy(si.owner_zone, stat_info_ptr->ownerZone, strlen(stat_info_ptr->ownerZone));
                si.ctime = std::stoll(stat_info_ptr->createTime);
                si.mtime = std::stoll(stat_info_ptr->modifyTime);

                std::string sql;
                bool set_perms = false;

                if (DATA_OBJ_T == si.type) {
                    set_perms = true;
                    sql = "select DATA_ACCESS_NAME where COLL_NAME = '";
                    sql += _p.parent_path();
                    sql += "' and DATA_NAME = '";
                    sql += _p.object_name();
                    sql += "'";
                }
                else if (COLL_OBJ_T == si.type) {
                    set_perms = true;
                    sql = "select COLL_ACCESS_NAME where COLL_NAME = '";
                    sql += _p;
                    sql += "'";
                }

                if (set_perms) {
                    for (const auto& row : irods::query{_conn, sql}) {
                        if (!row.empty()) {
                            if      (row[0] == "null")      { si.prms = perms::null; }
                            else if (row[0] == "read")      { si.prms = perms::read; }
                            else if (row[0] == "write")     { si.prms = perms::write; }
                            else if (row[0] == "own")       { si.prms = perms::own; }
                            else if (row[0] == "inherit")   { si.prms = perms::inherit; }
                            else if (row[0] == "noinherit") { si.prms = perms::noinherit; }
                            else                            { si.prms = perms::null; }
                        }
                    }
                }
            }

            return si;
        }

        auto is_collection_empty(rxConn* _conn, const path& _p) -> bool
        {
            std::string sql = "select count(COLL_NAME) where COLL_PARENT_NAME = '";
            sql += _p;
            sql += "'";

            for (const auto& row : irods::query{_conn, sql}) {
                if (row.empty()) {
                    throw filesystem_error{"empty resultset returned"};
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

        auto remove_impl(rxConn* _conn, const path& _p, remove_impl_options _opts) -> bool
        {
            detail::throw_if_path_length_exceeds_limit(_p);

            if (exists(_conn, _p)) {
                if (const auto s = status(_conn, _p); is_data_object(s)) {
                    dataObjInp_t input{};
                    std::strncpy(input.objPath, _p.c_str(), _p.string().size());

                    if (_opts.no_trash) {
                        addKeyVal(&input.condInput, FORCE_FLAG_KW, "");
                    }

                    return rxDataObjUnlink(_conn, &input) == 0;
                }
                else if (is_collection(s)) {
                    if (!_opts.recursive && !is_collection_empty(_conn, _p)) {
                        throw filesystem_error{"cannot remove non-empty collection"};
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
                    return rxRmColl(_conn, &input, verbose) >= 0;
                }
                else if (is_other(s)) {
                    throw filesystem_error{"unknown object type"};
                }
            }

            return false;
        }

        auto has_prefix(const path& _p, const path& _prefix) -> bool
        {
            using std::begin;
            using std::end;
            return std::search(begin(_p), end(_p), begin(_prefix), end(_prefix)) != end(_p);
        }
    } // anonymous namespace

    // Operational functions

    //auto absolute(rxConn* _conn, const path& _p, const path& base = current_path()) -> path;

    //auto canonical(rxConn* _conn, const path& _p, const path& base = current_path()) -> path;
    //auto weakly_canonical(rxConn* _conn, const path& _p, const path& base = current_path()) -> path;

    //auto relative(rxConn* _conn, const path& _p, const path& _base = current_path()) -> path;
    //auto proximate(rxConn* _conn, const path& _p, const path& _base = current_path()) -> path;

    auto copy(rxConn* _conn, const path& _from, const path& _to, copy_options _options) -> void
    {
        const auto from_status = status(_conn, _from);

        if (!exists(from_status)) {
            throw filesystem_error{"object does not exist [path => " + _from.string() + ']'};
        }

        if (equivalent(_from, _to)) {
            throw filesystem_error{"the source and destination are the same"};
        }

        if (status_known(from_status) && is_other(from_status)) {
            throw filesystem_error{"object is not a collection or data object [path => " + _from.string() + ']'};
        }

        const auto to_status = status(_conn, _to);
        
        if (status_known(to_status) && is_other(to_status)) {
            throw filesystem_error{"object is not a collection or data object [path => " + _to.string() + ']'};
        }

        if (is_collection(from_status) && is_data_object(to_status)) {
            throw filesystem_error{"cannot copy a collection into a data object"};
        }

        if (is_data_object(from_status)) {
            if (copy_options::collections_only == (copy_options::collections_only & _options)) {
                return;
            }

            if (is_collection(to_status)) {
                copy_data_object(_conn, _from, _to / _from.object_name(), _options);
                return;
            }

            copy_data_object(_conn, _from, _to, _options);

            return;
        }

        if (is_collection(from_status)) {
            if (copy_options::recursive == (copy_options::recursive & _options) ||
                copy_options::none == (copy_options::none & _options))
            {
                if (!exists(to_status)) {
                    //if (!create_collection(_conn, _to, _from)) {
                    if (!create_collection(_conn, _to)) {
                        throw filesystem_error{"error creating collection [path => " + _to.string() + ']'};
                    }
                }

                for (const auto& e : collection_iterator{_conn, _from}) {
                    const auto opts = _options | copy_options::in_recursive_copy;
                    copy(_conn, e.path(), _to / e.path().object_name(), opts);
                }
            }
        }
    }

    auto copy_data_object(rxConn* _conn, const path& _from, const path& _to, copy_options _options) -> bool
    {
        detail::throw_if_path_length_exceeds_limit(_from);
        detail::throw_if_path_length_exceeds_limit(_to);

        // If "_options" is not a power of 2, then report an error.
        // "_options" cannot have multiple options set.
        if (const auto v = static_cast<std::underlying_type_t<copy_options>>(_options);
            !(v && !(v & (v - 1))))
        {
            throw filesystem_error{"too many copy options set"}; 
        }

        if (!is_data_object(_conn, _from)) {
            throw filesystem_error{"source path does not exist or is not a data object"}; 
        }

        if (auto s = status(_conn, _to); exists(s)) {
            if (equivalent(_from, _to)) {
                throw filesystem_error{"source and destination paths are the same"}; 
            }

            if (!is_data_object(s)) {
                throw filesystem_error{"destination path is not a data object"}; 
            }

            if (copy_options::none == _options) {
                throw filesystem_error{"no copy options set"}; 
            }

            if (copy_options::skip_existing == _options) {
                return false;
            }

            if (copy_options::overwrite_existing == _options) {
                // NOP
            }

            if (copy_options::update_existing == _options) {
                if (last_write_time(_conn, _from) <= last_write_time(_conn, _to)) {
                    return false;
                }
            }
        }

        dataObjCopyInp_t input{};
        std::strncpy(input.srcDataObjInp.objPath, _from.c_str(), _from.string().size());
        std::strncpy(input.destDataObjInp.objPath, _to.c_str(), _to.string().size());
        addKeyVal(&input.destDataObjInp.condInput, DEST_RESC_NAME_KW, "");

        if (const auto ec = rxDataObjCopy(_conn, &input); ec < 0) {
            throw filesystem_error{"error copying data object [error code => " + std::to_string(ec) + ']'};
        }

        return true;
    }

    /*
    auto copy_collection(rxConn* _conn, const path& _from, const path& _to, copy_options _options) -> bool
    {
        return false;
    }
    */

    auto create_collection(rxConn* _conn, const path& _p) -> bool // Implies perms::all
    {
        detail::throw_if_path_length_exceeds_limit(_p);

        if (!exists(_conn, _p.parent_path())) {
            throw filesystem_error{"the parent path must exist [path => " + _p.string() + ']'};
        }

        if (exists(_conn, _p)) {
            return false;
        }

        collInp_t input{};
        std::strncpy(input.collName, _p.c_str(), _p.string().size());

        if (const auto ec = rxCollCreate(_conn, &input); ec != 0) {
            throw filesystem_error{"error creating collection [error code => " + std::to_string(ec) + ']'};
        }

        return true;
    }

    /*
    auto create_collection(rxConn* _conn, const path& _p, const path& _existing_p) -> bool
    {
        return false;
    }
    */

    auto create_collections(rxConn* _conn, const path& _p) -> bool
    {
        detail::throw_if_path_length_exceeds_limit(_p);

        if (exists(_conn, _p)) {
            return false;
        }

        collInp_t input{};
        std::strncpy(input.collName, _p.c_str(), _p.string().size());
        addKeyVal(&input.condInput, RECURSIVE_OPR__KW, "");

        return rxCollCreate(_conn, &input) == 0;
    }

    //auto current_path(rxConn* _conn) -> path;
    //auto current_path(rxConn* _conn, const path& _p) -> void;

    auto exists(object_status _s) noexcept -> bool
    {
        return status_known(_s) && _s.type() != object_type::not_found;
    }

    auto exists(rxConn* _conn, const path& _p) -> bool
    {
        return exists(status(_conn, _p));
    }

    auto equivalent(const path& _p1, const path& _p2) -> bool
    {
        return _p1.lexically_normal() == _p2.lexically_normal();
    }

    auto data_object_size(rxConn* _conn, const path& _p) -> std::uintmax_t
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

    auto is_collection(rxConn* _conn, const path& _p) -> bool
    {
        return is_collection(status(_conn, _p));
    }

    auto is_empty(rxConn* _conn, const path& _p) -> bool
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

    auto is_other(rxConn* _conn, const path& _p) -> bool
    {
        return is_other(status(_conn, _p));
    }

    auto is_data_object(object_status _s) noexcept -> bool
    {
        return _s.type() == object_type::data_object;
    }

    auto is_data_object(rxConn* _conn, const path& _p) -> bool
    {
        return is_data_object(status(_conn, _p));
    }

    auto last_write_time(rxConn* _conn, const path& _p) -> object_time_type
    {
        const auto info = stat(_conn, _p);

        if (info.error < 0) {
            throw filesystem_error{"could not retrieve mtime [error code => " +
                                   std::to_string(info.error) + ']'};
        }

        return object_time_type{std::chrono::seconds{info.mtime}};
    }

    auto last_write_time(rxConn* _conn, const path& _p, object_time_type _new_time) -> void
    {
        detail::throw_if_path_length_exceeds_limit(_p);

        const auto seconds = _new_time.time_since_epoch();

        collInp_t input{};
        std::strncpy(input.collName, _p.c_str(), _p.string().size());
        std::stringstream new_time;
        new_time << std::setfill('0') << std::setw(11) << std::to_string(seconds.count());
        addKeyVal(&input.condInput, COLLECTION_MTIME_KW, new_time.str().c_str());

        if (const auto ec = rxModColl(_conn, &input); ec != 0) {
            throw filesystem_error{"mtime update failure [error code => " + std::to_string(ec) + ']'};
        }
    }

    auto remove(rxConn* _conn, const path& _p, remove_options _opts) -> bool
    {
        const auto no_trash = (remove_options::no_trash == _opts);
        constexpr auto recursive = false;
        return remove_impl(_conn, _p, {no_trash, recursive});
    }

    auto remove_all(rxConn* _conn, const path& _p, remove_options _opts) -> std::uintmax_t
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

    auto permissions(rxConn* _conn, const path& _p, perms _prms) -> void
    {
        detail::throw_if_path_length_exceeds_limit(_p);

        modAccessControlInp_t input{};

        char username[NAME_LEN]{};
        rstrcpy(username, _conn->clientUser.userName, NAME_LEN);
        input.userName = username;

        char zone[NAME_LEN]{};
        rstrcpy(zone, _conn->clientUser.rodsZone, NAME_LEN);
        input.zone = zone;

        char path[MAX_NAME_LEN]{};
        std::strncpy(path, _p.c_str(), _p.string().size());
        input.path = path;

        char access[10]{};

        switch (_prms) {
            case perms::null:
                std::strncpy(access, "null", 4);
                break;

            case perms::read:
                std::strncpy(access, "read", 4);
                break;

            case perms::write:
                std::strncpy(access, "write", 5);
                break;

            case perms::own:
                std::strncpy(access, "own", 3);
                break;

            case perms::inherit:
                std::strncpy(access, "inherit", 7);
                break;

            case perms::noinherit:
                std::strncpy(access, "noinherit", 9);
                break;
        }

        input.accessLevel = access;

        if (const auto ec = rxModAccessControl(_conn, &input); ec != 0) {
            throw filesystem_error{"error modifying permissions [error code => " + std::to_string(ec) + ']'};
        }
    }

    auto rename(rxConn* _conn, const path& _old_p, const path& _new_p) -> void
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

        if (has_prefix(_new_p, _old_p)) {
            throw filesystem_error{R"_("_old_p" cannot be an ancestor of "_new_p")_"};
        }

        if (_new_p.object_name_is_dot() || _new_p.object_name_is_dot_dot()) {
            throw filesystem_error{R"_("_new_p" cannot end with "." or "..")_"};
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
            else if (!exists(_conn, _new_p.parent_path())) {
                throw filesystem_error{R"_(the parent path of "_new_p" must exist)_"};
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
            else if (_new_p.string().back() == path::separator ||
                     !is_collection(_conn, _new_p.parent_path()))
            {
                throw filesystem_error{R"_(the parent path of "_new_p" must exist and cannot end with a separator)_"};
            }
        }
        else if (is_other(s)) {
            throw filesystem_error{R"_("_new_p" must be a collection or data object)_"};
        }
        */

        dataObjCopyInp_t input{};
        std::strncpy(input.srcDataObjInp.objPath, _old_p.c_str(), _old_p.string().size());
        std::strncpy(input.destDataObjInp.objPath, _new_p.c_str(), _new_p.string().size());

        if (const auto ec = rxDataObjRename(_conn, &input); ec < 0) {
            throw filesystem_error{"rename error [ec = " + std::to_string(ec) + "]"};
        }
    }

    auto move(rxConn* _conn, const path& _old_p, const path& _new_p) -> void
    {
        rename(_conn, _old_p, _new_p);
    }

    auto resize_data_object(rxConn* _conn, const path& _p, std::uintmax_t _size) -> void
    {

    }

    //auto space(rxConn* _conn, const path& _p) -> space_info;

    auto status(rxConn* _conn, const path& _p) -> object_status
    {
        auto s = stat(_conn, _p);
        object_status status;

        status.permissions(s.prms);

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

    //auto system_complete(rxConn* _conn, const path& _p) -> path;
} // namespace irods::experimental::filesystem

