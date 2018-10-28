#ifndef IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP
#define IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP

#include <iterator>

#include <irods/filesystem/collection_entry.hpp>

namespace irods {
namespace filesystem {

class recursive_collection_iterator
{
public:
    // clang-format off
    using value_type        = collection_entry;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::input_iterator_tag;
    // clang-format on

    recursive_collection_iterator() noexcept; // Creates the "end" iterator
    explicit recursive_collection_iterator(const path& _p);

    recursive_collection_iterator(const recursive_collection_iterator& _other);
    auto operator=(const recursive_collection_iterator& _other) -> recursive_collection_iterator&;

    ~recursive_collection_iterator();

    // Observers

    auto level() const noexcept -> int;
    bool no_push_pending() const noexcept;

    auto operator++() -> recursive_collection_iterator&;
    auto operator++(int) -> recursive_collection_iterator;

    bool operator==(const recursive_collection_iterator& _rhs) const;
    bool operator!=(const recursive_collection_iterator& _rhs) const;

    auto operator*() const -> reference;

private:
    // Actual data members will probably be stored in a shared object,
    // or some similar mechanism, to achieve the required input iterator
    // copy semantics.

    int level_;
    bool no_push_;
};

} // namespace filesystem
} // namespace irods

#endif // IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP
