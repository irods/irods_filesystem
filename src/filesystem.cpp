#include <irods/filesystem/filesystem.hpp>

#include <irods/filesystem/path.hpp>
#include <irods/filesystem/filesystem_error.hpp>

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

    auto equivalent(comm* _comm, const path& _p1, const path& _p2) -> bool
    {
        return false;
    }

    auto data_object_size(comm* _comm, const path& _p) -> std::uintmax_t
    {
        if (auto info = stat(_comm, _p); info.error >= 0) {
            return static_cast<std::uintmax_t>(info.size);
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
            std::string sql = "select count(COLL_NAME) where COLL_PARENT_NAME = '";
            sql += _p;
            sql += "'";

            for (const auto& row : irods::query{_comm, sql}) {
                if (row.empty()) {
                    throw filesystem_error{"empty resultset returned"};
                }

                if (std::stoi(row[0]) > 0) {
                    return false;
                }
            }

            return true;
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

    auto remove(comm* _comm, const path& _p) -> bool
    {
        return false;
    }

    auto remove_all(comm* _comm, const path& _p) -> std::uintmax_t
    {
        return 0;
    }

    auto permissions(comm* _comm, const path& _p, perms _prms, perm_options _opts) -> void
    {

    }

    auto rename(comm* _comm, const path& _from, const path& _to) -> void
    {

    }

    auto move(comm* _comm, const path& _from, const path& _to) -> void
    {

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

