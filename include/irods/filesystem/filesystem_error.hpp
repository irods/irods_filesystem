#ifndef IRODS_FILESYSTEM_ERROR_HPP
#define IRODS_FILESYSTEM_ERROR_HPP

#include <system_error>
#include <string>

namespace irods::filesystem
{
    class path;

    class filesystem_error : public std::system_error
    {
    public:
        filesystem_error();
        filesystem_error(const filesystem_error& _other);
        filesystem_error(const std::string& _msg);
        filesystem_error(const std::string& _msg, const path& _p1, const path& p2);

        auto operator=(const filesystem_error& _other) -> filesystem_error&;

        ~filesystem_error();

        auto path1() const -> const path&;
        auto path2() const -> const path&;

        auto what() const -> const char*;

    private:
        std::string msg_;
        path* p1_;
        path* p2_;
    };
} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_ERROR_HPP
