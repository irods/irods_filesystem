#include <irods/filesystem/collection_iterator.hpp>

#include <irods/filesystem/detail.hpp>
#include <irods/filesystem/filesystem_error.hpp>

#ifdef RODS_SERVER
    #include <irods/rsOpenCollection.hpp>
    #include <irods/rsReadCollection.hpp>
    #include <irods/rsCloseCollection.hpp>
#else
    #include <irods/openCollection.h>
    #include <irods/readCollection.h>
    #include <irods/closeCollection.h>
#endif // RODS_SERVER

#include <functional>
#include <iostream>
#include <string>
#include <cassert>

namespace irods::filesystem
{
    namespace
    {
        const auto rsReadCollection = [](rsComm_t* _conn, int _handle, collEnt_t** _collEnt) -> int
        {
            return ::rsReadCollection(_conn, &_handle, _collEnt);
        };

        const auto rsCloseCollection = [](rsComm_t* _conn, int _handle) -> int
        {
            return ::rsCloseCollection(_conn, &_handle);
        };
    } // anonymous namespace

    collection_iterator::collection_iterator(rxConn* _conn,
                                             const path& _p,
                                             collection_options _opts)
        : ctx_{}
    {
        detail::throw_if_path_length_exceeds_limit(_p);
    
        ctx_ = std::make_shared<context>();
        ctx_->conn = _conn;
        ctx_->path = _p;
        assert(ctx_->handle == 0);
        assert(ctx_->offset == 0);

        collInp_t input{};
        std::strncpy(input.collName, _p.c_str(), _p.string().size());

        ctx_->handle = rxOpenCollection(_conn, &input);

        if (ctx_->handle < 0) {
            throw filesystem_error{"could not open collection for reading [handle => " +
                                   std::to_string(ctx_->handle) + ']'};
        }

        // Point to the first entry.
        ++(*this);
    }

    collection_iterator::~collection_iterator()
    {
        if (ctx_.use_count() == 1) {
            rxCloseCollection(ctx_->conn, ctx_->handle);
        }
    }

    auto collection_iterator::operator++() -> collection_iterator&
    {
        collEnt_t* e{};

        if (const auto ec = rxReadCollection(ctx_->conn, ctx_->handle, &e); ec < 0) {
            if (ec == CAT_NO_ROWS_FOUND) {
                ctx_ = nullptr;
                return *this;
            }
            
            throw filesystem_error{"could not read collection entry [ec => " +
                                   std::to_string(ec) + ']'};
        }

        ++ctx_->offset;

        /*
        irods::at_scope_exit<std::function<void()> at_scope_exit{[e] {
            if (entry) {
                freeCollEnt(e);
            }
        }};
        */

        auto& entry = ctx_->entry;

        entry.repl_number_ = e->replNum;
        entry.repl_status_ = e->replStatus;
        entry.data_mode_ = e->dataMode;
        entry.data_size_ = e->dataSize;

        if (e->dataId)      { entry.data_id_ = e->dataId; }
        if (e->createTime)  { entry.ctime_ = std::stoll(e->createTime); }
        if (e->modifyTime)  { entry.mtime_ = std::stoll(e->modifyTime); }
        if (e->chksum)      { entry.checksum_ = e->chksum; }
        if (e->resource)    { entry.resc_ = e->resource; }
        if (e->resc_hier)   { entry.resc_hier_ = e->resc_hier; }
        if (e->phyPath)     { entry.phy_path_ = e->phyPath; }
        if (e->ownerName)   { entry.owner_ = e->ownerName; }
        if (e->dataType)    { entry.data_type_ = e->dataType; }

        switch (e->objType) {
            case COLL_OBJ_T:
                entry.status_.type(object_type::collection);
                entry.path_ = e->collName;
                break;

            case DATA_OBJ_T:
                entry.status_.type(object_type::data_object);
                entry.path_ = ctx_->path / e->dataName;
                break;

            default:
                entry.status_.type(object_type::none);
                break;
        }

        freeCollEnt(e);

        return *this;
    }
} // namespace irods::filesystem

