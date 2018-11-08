#ifndef IRODS_FILESYSTEM_DETAIL_HPP
#define IRODS_FILESYSTEM_DETAIL_HPP

#include <irods/filesystem/path.hpp>

namespace irods::filesystem::detail
{
    auto is_separator(path::value_type _c) noexcept -> bool
    {
        return path::separator == _c;
    }
} // irods::filesystem::detail

#endif // IRODS_FILESYSTEM_DETAIL_HPP
