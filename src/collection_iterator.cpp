#include <irods/filesystem/collection_iterator.hpp>

#include <irods/filesystem/detail.hpp>
#include <irods/filesystem/filesystem_error.hpp>

#include <irods/openCollection.h>
#include <irods/readCollection.h>
#include <irods/closeCollection.h>

#include <functional>
#include <iostream>

namespace irods::filesystem
{
    collection_iterator::collection_iterator(comm* _comm,
                                             const path& _p,
                                             collection_options _opts)
        : comm_{_comm}
        , path_{_p}
        , handle_{}
        , entry_{}
    {
        detail::throw_if_path_length_exceeds_limit(_p);

        collInp_t input{};
        std::strncpy(input.collName, _p.c_str(), _p.string().size());
        input.flags = RECUR_QUERY_FG | VERY_LONG_METADATA_FG;

        handle_ = std::make_shared<int>(rcOpenCollection(comm_, &input));

        if (*handle_ < 0) {
            throw filesystem_error{"could not open collection for reading"};
        }

        // Read the first entry.
        ++(*this);
    }

    collection_iterator::~collection_iterator()
    {
        if (handle_.use_count() == 1 && *handle_ >= 0) {
            rcCloseCollection(comm_, *handle_);
        }
    }

    auto collection_iterator::operator++() -> collection_iterator&
    {
        collEnt_t* e{};

        if (const auto ec = rcReadCollection(comm_, *handle_, &e); ec < 0) {
            throw filesystem_error{"could not read collection entry [ec => " + std::to_string(ec) + ']'};
        }

        /*
        irods::at_scope_exit<std::function<void()> at_scope_exit{[e] {
            if (entry) {
                freeCollEnt(e);
            }
        }};
        */

        switch (e->objType) {
            case COLL_OBJ_T:
                break;

            case DATA_OBJ_T:
                break;

            default:
                break;
        }

        std::cout << "\tobject type: " << e->objType << '\n';
        std::cout << "\treplNum: " << e->replNum << '\n';
        std::cout << "\treplStatus: " << e->replStatus << '\n';
        std::cout << "\tdataMode: " << e->dataMode << '\n';
        std::cout << "\tdataSize: " << e->dataSize << '\n';
        if (e->collName) std::cout << "\tcollName: " << e->collName << '\n';
        if (e->dataName) std::cout << "\tdataName: " << e->dataName << '\n';
        if (e->dataId) std::cout << "\tdataId: " << e->dataId << '\n';
        if (e->createTime) std::cout << "\tcreateTime: " << e->createTime << '\n';
        if (e->modifyTime) std::cout << "\tmodifyTime: " << e->modifyTime << '\n';
        if (e->chksum) std::cout << "\tchksum: " << e->chksum << '\n';
        if (e->resource) std::cout << "\tresource: " << e->resource << '\n';
        if (e->resc_hier) std::cout << "\tresc_hier: " << e->resc_hier << '\n';
        if (e->phyPath) std::cout << "\tphyPath: " << e->phyPath << '\n';
        if (e->ownerName) std::cout << "\townerName: " << e->ownerName << '\n';
        if (e->dataType) std::cout << "\tdataType: " << e->dataType << '\n';
        std::cout << '\n';

        freeCollEnt(e);

        /*
        entry_.object_type();
        entry_.replica_number();
        entry_.replica_status();
        entry_.data_mode();
        entry_.data_size();
        entry_.collection_name(); // Merge w/ next line into "name()" function.
        entry_.data_name();
        entry_.id();
        entry_.ctime();
        entry_.mtime();
        entry_.checksum();
        entry_.resource();
        entry_.resource_hierarchy();
        entry_.physical_path();
        entry_.owner_name();
        entry_.data_type();
        entry_.special_collection();
        */

        return *this;
    }

    auto collection_iterator::operator++(int) -> collection_iterator&
    {
        return ++(*this);
    }
} // namespace irods::filesystem

