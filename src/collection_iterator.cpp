#include <irods/filesystem/collection_iterator.hpp>

#include <irods/filesystem/detail.hpp>
#include <irods/filesystem/filesystem_error.hpp>

#include <irods/openCollection.h>
#include <irods/readCollection.h>
#include <irods/closeCollection.h>
#include <irods/at_scope_exit.hpp>

#include <functional>

namespace irods::filesystem
{
    namespace
    {
        constexpr char query[] = "";
    } // anonymous namespace

    collection_iterator::collection_iterator() = default;

    collection_iterator::collection_iterator(comm& _comm,
                                             const path& _p,
                                             collection_options _opts = collection_options::none)
        : comm_{_comm}
        , path_{_p}
        , handle_{}
        , entry_{}
    {
        detail::throw_if_path_length_exceeds_limit(_p);

        collInp_t input{};
        std::strncpy(input.collName, _p.c_str(), _p.string().size());

        handle_ = rcOpenCollection(comm_, &input);

        if (handle_ < 0) {
            throw filesystem_error{"could not open collection for reading"};
        }

        // Read the first entry.
        ++(*this);
    }

    collection_iterator::~collection_iterator()
    {
        if (handle_ >= 0) {
            rcCloseCollection(comm_, handle_);
        }
    }

    auto collection_iterator::operator++() -> collection_iterator&
    {
        collEnt_t* entry{};

        if (const auto ec = rcReadCollection(comm_, handle_, &entry); ec < 0) {
            throw filesystem_error{"could not read collection entry"};
        }

        irods::at_scope_exit<std::function<void()> at_scope_exit{[entry] {
            if (entry) {
                freeCollEnt(entry);
            }
        }};

        std::cout << "path: " << path_ << '\n';
        std::cout << "\tobject type: " << entry->objType << '\n';
        std::cout << "\treplNum: " << entry->relpNum << '\n';
        std::cout << "\treplStatus: " << entry->replStatus << '\n';
        std::cout << "\tdataMode: " << entry->dataMode << '\n';
        std::cout << "\tdataSize: " << entry->dataSize << '\n';
        std::cout << "\tcollName: " << entry->collName << '\n';
        std::cout << "\tdataName: " << entry->dataName << '\n';
        std::cout << "\tdataId: " << entry->dataId << '\n';
        std::cout << "\tcreateTime: " << entry->createTime << '\n';
        std::cout << "\tmodifyTime: " << entry->modifyTime << '\n';
        std::cout << "\tchksum: " << entry->chksum << '\n';
        std::cout << "\tresource: " << entry->resource << '\n';
        std::cout << "\tresc_hier: " << entry->resc_hier << '\n';
        std::cout << "\tphyPath: " << entry->phyPath << '\n';
        std::cout << "\townerName: " << entry->ownerName << '\n';
        std::cout << "\tdataType: " << entry->dataType << '\n';
        std::cout << '\n';

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

    auto collection_iterator::operator++(int) -> collection_iterator
    {
        return ++(*this);
    }
} // namespace irods::filesystem

