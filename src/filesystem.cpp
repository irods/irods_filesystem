#include <irods/filesystem/filesystem.hpp>
#include <irods/filesystem/path.hpp>

namespace irods::filesystem
{
    auto operator==(const path& _lhs, const path& _rhs) -> bool
    {
        return std::equal(std::begin(_lhs), std::end(_lhs), std::begin(_rhs), std::end(_rhs));
    }

    auto operator!=(const path& _lhs, const path& _rhs) -> bool
    {
        return !(_lhs == _rhs);
    }

    auto operator< (const path& _lhs, const path& _rhs) -> bool
    {
        return false;
    }

    auto operator<=(const path& _lhs, const path& _rhs) -> bool
    {
        return false;
    }

    auto operator> (const path& _lhs, const path& _rhs) -> bool
    {
        return false;
    }

    auto operator>=(const path& _lhs, const path& _rhs) -> bool
    {
        return false;
    }
} // namespace irods::filesystem

