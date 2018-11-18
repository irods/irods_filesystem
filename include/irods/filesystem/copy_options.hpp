#ifndef IRODS_FILESYSTEM_COPY_OPTIONS_HPP
#define IRODS_FILESYSTEM_COPY_OPTIONS_HPP

#include <cstdint>
#include <type_traits>

namespace irods::filesystem
{
    enum class copy_options : std::uint8_t
    {
        none                    = 0,
        skip_existing           = 1 << 1,
        overwrite_if_exists     = 1 << 2,
        update_existing         = 1 << 3,
        recursive               = 1 << 4,
        collections_only        = 1 << 5,
        in_recursive_copy       = 1 << 6
    };

    inline auto operator|=(copy_options& _lhs, copy_options _rhs) noexcept -> copy_options&
    {
        using T = std::underlying_type_t<copy_options>;
        return _lhs = static_cast<copy_options>(static_cast<T>(_lhs) | static_cast<T>(_rhs));
    }

    inline auto operator|(copy_options _lhs, copy_options _rhs) noexcept -> copy_options
    {
        return _lhs |= _rhs;
    }

    inline auto operator&=(copy_options& _lhs, copy_options _rhs) noexcept -> copy_options&
    {
        using T = std::underlying_type_t<copy_options>;
        return _lhs = static_cast<copy_options>(static_cast<T>(_lhs) & static_cast<T>(_rhs));
    }

    inline auto operator&(copy_options _lhs, copy_options _rhs) noexcept -> copy_options
    {
        return _lhs &= _rhs;
    }
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_COPY_OPTIONS_HPP
