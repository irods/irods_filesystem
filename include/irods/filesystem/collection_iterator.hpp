#ifndef IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP
#define IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP

#include <iterator>

#include <irods/filesystem/path.hpp>
#include <irods/filesystem/collection_entry.hpp>

namespace irods::filesystem
{
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

        collection_iterator() noexcept; // Creates the "end" iterator
        explicit collection_iterator(const path& _p);

        collection_iterator(const collection_iterator& _other);
        auto operator=(const collection_iterator& _other) -> collection_iterator&;

        ~collection_iterator();

        auto operator++() -> collection_iterator&;
        auto operator++(int) -> collection_iterator;

        bool operator==(const collection_iterator& _rhs) const;
        bool operator!=(const collection_iterator& _rhs) const;

        auto operator*() const -> reference;

    private:
    };
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP
