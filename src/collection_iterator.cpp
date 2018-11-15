#include <irods/filesystem/collection_iterator.hpp>

#include <irods/filesystem/detail.hpp>
#include <irods/filesystem/filesystem_error.hpp>

#include <irods/openCollection.h>
#include <irods/readCollection.h>
#include <irods/closeCollection.h>

#include <functional>
#include <iostream>
#include <string>

namespace irods::filesystem
{
    namespace
    {
        template <typename T, typename U>
        auto set_if_not_null(T& _dst, U* _src)
        {
            if (_src) {
                _dst = _src;
            }
        }
    } // anonymous namespace

    collection_iterator::collection_iterator()
        : comm_{}
        , path_{}
        , handle_{-1}
        , offset_{}
        , entry_{}
    {
    }
    
    collection_iterator::collection_iterator(comm* _comm,
                                             const path& _p,
                                             collection_options _opts)
        : comm_{_comm}
        , path_{_p}
        , handle_{-1}
        , offset_{}
        , entry_{}
    {
        detail::throw_if_path_length_exceeds_limit(_p);

        collInp_t input{};
        std::strncpy(input.collName, _p.c_str(), _p.string().size());
        //input.flags = RECUR_QUERY_FG | VERY_LONG_METADATA_FG;

        handle_ = rcOpenCollection(comm_, &input);

        if (handle_ < 0) {
            throw filesystem_error{"could not open collection for reading"};
        }

        // Read the first entry.
        ++(*this);
    }

    collection_iterator::collection_iterator(const collection_iterator& _other)
        : comm_{_other.comm_}
        , path_{_other.path_}
        , handle_{}
        , offset_{}
        , entry_{}
        //, entry_{_other.entry_}
    {
        collInp_t input{};
        std::strncpy(input.collName, path_.c_str(), path_.string().size());
        //input.flags = RECUR_QUERY_FG | VERY_LONG_METADATA_FG;

        handle_ = rcOpenCollection(comm_, &input);

        if (handle_ < 0) {
            throw filesystem_error{"could not open collection for reading"};
        }

        // Move the iterator forward so that it points to the same location
        // as "_other".
        for (long i = 0; i < _other.offset_; ++i) {
            ++(*this);
        }
    }

    auto collection_iterator::operator=(const collection_iterator& _other) -> collection_iterator&
    {
        comm_ = _other.comm_;
        path_ = _other.path_;
        offset_ = 0;

        collInp_t input{};
        std::strncpy(input.collName, path_.c_str(), path_.string().size());
        //input.flags = RECUR_QUERY_FG | VERY_LONG_METADATA_FG;

        handle_ = rcOpenCollection(comm_, &input);

        if (handle_ < 0) {
            throw filesystem_error{"could not open collection for reading"};
        }

        // Move the iterator forward so that it points to the same location
        // as "_other".
        for (long i = 0; i < _other.offset_; ++i) {
            ++(*this);
        }

        return *this;
    }

    collection_iterator::~collection_iterator()
    {
        rcCloseCollection(comm_, handle_);
    }

    auto collection_iterator::operator++() -> collection_iterator&
    {
        collEnt_t* e{};

        if (const auto ec = rcReadCollection(comm_, handle_, &e); ec < 0) {
            if (ec == CAT_NO_ROWS_FOUND) {
                handle_ = -1;
                return *this;
            }
            
            throw filesystem_error{"could not read collection entry [ec => " + std::to_string(ec) + ']'};
        }

        ++offset_;

        /*
        irods::at_scope_exit<std::function<void()> at_scope_exit{[e] {
            if (entry) {
                freeCollEnt(e);
            }
        }};
        */

        entry_.repl_number_ = e->replNum;
        entry_.repl_status_ = e->replStatus;
        entry_.data_mode_ = e->dataMode;
        entry_.data_size_ = e->dataSize;

        if (e->dataId)      { entry_.data_id_ = e->dataId; }
        if (e->createTime)  { entry_.ctime_ = std::stoll(e->createTime); }
        if (e->modifyTime)  { entry_.mtime_ = std::stoll(e->modifyTime); }
        if (e->chksum)      { entry_.checksum_ = e->chksum; }
        if (e->resource)    { entry_.resc_ = e->resource; }
        if (e->resc_hier)   { entry_.resc_hier_ = e->resc_hier; }
        if (e->phyPath)     { entry_.phy_path_ = e->phyPath; }
        if (e->ownerName)   { entry_.owner_ = e->ownerName; }
        if (e->dataType)    { entry_.data_type_ = e->dataType; }

        switch (e->objType) {
            case COLL_OBJ_T:
                entry_.status_.type(object_type::collection);
                entry_.path_ = e->collName;
                break;

            case DATA_OBJ_T:
                entry_.status_.type(object_type::data_object);
                entry_.path_ = e->dataName;
                break;

            default:
                entry_.status_.type(object_type::none);
                break;
        }

        freeCollEnt(e);

        return *this;
    }

    auto collection_iterator::operator++(int) -> collection_iterator&
    {
        return ++(*this);
    }
} // namespace irods::filesystem

