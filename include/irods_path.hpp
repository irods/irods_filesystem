#ifndef IRODS_PATH_HPP
#define IRODS_PATH_HPP

namespace irods {
namespace filesystem {

class path
{
public:
    constexpr char dot       = '.';
    constexpr char separator = '/';

    // Constructors and destructor

    path();
    path(const path& _p);
    path(path&& _p) noexcept;

    template <typename Source>
    path(const Source& _source);

    template <typename InputIterator>
    path(InputIterator _begin, InputIterator _end);

    ~path();

    // Assignments

    path& operator=(const path& _p);
    path& operator=(path&& _p) noexcept;

    template <typename Source>
    path& operator=(const Source& _source);

    template <typename Source>
    path& assign(const Source& _source);

    template <typename InputIterator>
    path& assign(InputIterator _begin, InputIterator _end);

    // Appends

    path& operator/=(const path& _p);

    template <typename Source>
    path& operator/=(const Source& _source);

    template <typename Source>
    path& append(const Source& _source);

    template <typename InputIterator>
    path& append(InputIterator _begin, InputIterator _end);

    // Concatenation

    path& operator+=(const path& _p);
    path& operator+=(const std::string& _p);
    path& operator+=(const char* _p);
    path& operator+=(char _p);

    template <typename Source>
    path& operator+=(const Source& _p);

    template <typename CharT>
    path& operator+=(CharT _p);

    template <typename InputIterator>
    path& concat(InputIterator _begin, InputIterator _end);

    // Modifiers

    void clear();
    auto remove_filename() -> path&;
    auto replace_extension(const path& _new_extension = {}) -> path&;
    void swap(path& _rhs);

    // Lexical operations

    path lexically_normal() const;
    path lexically_relative(const path& _base) const;

    // Format observers

    auto string() const -> const std::string&;

    // Compare

    auto compare(const path& _p) const noexcept -> int;
    auto compare(const std::string& _p) const -> int;
    auto compare(const char* _p) const -> int;

    // Decomposition

    path root_name() const;
    path root_collection() const;
    path root_path() const;
    path relative_path() const;
    path parent_path() const;
    path data_object_name() const;
    path stem() const;
    path extension() const;

    // Query

    bool empty() const;
    bool data_object_is_dot() const;
    bool data_object_is_dot_dot() const;
    bool has_root_name() const;
    bool has_root_collection() const;
    bool has_root_path() const;
    bool has_relative_path() const;
    bool has_parent_path() const;
    bool has_data_object_name() const;
    bool has_stem() const;
    bool has_extension() const;
    bool is_absolute() const;
    bool is_relative() const;

    // Iterators

    class iterator;
    using const_iterator = iterator;

    class reverse_iterator;
    using const_reverse_iterator = reverse_iterator;

    auto begin() const -> iterator;
    auto end() const -> iterator;

    auto rbegin() const -> reverse_iterator;
    auto rend() const -> reverse_iterator;

private:
    std::string pathname_;
};

#include "irods_path.tpp"

} // namespace filesystem
} // namespace irods

#endif // IRODS_PATH_HPP
