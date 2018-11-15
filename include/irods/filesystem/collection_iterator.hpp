#ifndef IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP
#define IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP

#include <irods/filesystem/filesystem.hpp>
#include <irods/filesystem/collection_entry.hpp>
#include <irods/filesystem/path.hpp>
//#include <irods/irods_query.hpp>

#include <iterator>
#include <memory>

namespace irods::filesystem
{
    enum class collection_options : std::uint8_t
    {
        none,
        skip_permission_denied
    };

    class collection_iterator
    {
    public:
        // clang-format off
        using value_type        = collection_entry;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::input_iterator_tag;
        // clang-format on

        collection_iterator();
        collection_iterator(comm& _comm,
                            const path& _p,
                            collection_options _opts = collection_options::none);

        collection_iterator(const collection_iterator& _other) = delete;
        auto operator=(const collection_iterator& _other) -> collection_iterator& = delete;

        ~collection_iterator();

        auto operator++() -> collection_iterator&;
        auto operator++(int) -> collection_iterator;

        auto operator*() const -> reference { return entry_; }
        auto operator->() const -> pointer  { return &entry_; }

    private:
        //std::shared_ptr<irods::query> q_ptr_;
        comm& comm_;
        const path path_;
        int handle_;
        value_type entry_;
    };

    // Enables support for range-based for-loops.
    auto begin(collection_iterator _iter) -> collection_iterator;
    auto end(const collection_iterator& _iter) -> collection_iterator;
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP
