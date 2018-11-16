#ifndef IRODS_FILESYSTEM_ERROR_HPP
#define IRODS_FILESYSTEM_ERROR_HPP

#include <irods/filesystem/path.hpp>

#include <system_error>
#include <stdexcept>
#include <string>

namespace irods::filesystem
{
    class path;

    class filesystem_error
        : public std::runtime_error
        //: public std::system_error
    {
    public:
        filesystem_error() = default;

        filesystem_error(const std::string& _msg)
            : std::runtime_error{_msg}
            , p1_{}
            , p2_{}
        {
        }

        filesystem_error(const std::string& _msg, const path& _p1, const path& _p2)
            : std::runtime_error{_msg}
            , p1_{_p1}
            , p2_{_p2}
        {
        }

        filesystem_error(const filesystem_error& _other) = default;
        auto operator=(const filesystem_error& _other) -> filesystem_error& = default;

        ~filesystem_error() = default;

        auto path1() const -> const path&   { return p1_; }
        auto path2() const -> const path&   { return p2_; }

    private:
        path p1_;
        path p2_;
    };
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_ERROR_HPP
