#include "irods/filesystem/path.hpp"

namespace irods::filesystem
{
    // Constructors and destructor

    path::path() = default;
    path::path(const path&) = default;
    path::path(path&&) = default;
    path::~path() = default;

    // Assignments

    auto operator=(const path&) -> path& = default;
    auto operator=(path&&) noexcept -> path& = default;

    auto operator=(std::string&& _source) -> path&
    {
        value_ = std::move(_source);
        return *this;
    }

    // Appends

    auto operator/=(const path& _p) -> path&
    {
        return *this;
    }

    auto operator/=(path&& _p) -> path&
    {
        return *this;
    }

    // Concatenation

    auto operator+=(const path& _p) -> path&
    {
        return *this;
    }

    auto operator+=(const std::string& _p) -> path&
    {
        return *this;
    }

    auto operator+=(const char* _p) -> path&
    {
        return *this;
    }

    auto operator+=(char _p) -> path&
    {
        return *this;
    }

} // namespace irods::filesystem

