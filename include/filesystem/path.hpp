#ifndef IRODS_FILESYSTEM_PATH_HPP
#define IRODS_FILESYSTEM_PATH_HPP

#include <string>
#include <utility>

#include <boost/algorithm/string.hpp>

#include "irods/filesystem/path_traits.hpp"

namespace irods::filesystem
{
    class path
    {
    public:
        class iterator;
        class reverse_iterator;

        using value_type                = char;
        using string_type               = std::basic_string<value_type>;
        using const_iterator            = iterator;
        using const_reverse_iterator    = reverse_iterator;

        inline static const value_type dot[]     = ".";
        inline static const value_type dot_dot[] = "..";
        inline static const value_type separator = '/';

        // Constructors and destructor

        path() = default;
        path(const path& _p) = default;
        path(path&& _p) noexcept = default;

        path(const value_type* _source)
            : value_{_source}
        {
        }

        path(string_type&& _source) noexcept
            : value_{std::move(_source)}
        {
        }

        // Delegating constructor
        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable_v<Source>>>
        path(const Source& _source)
            : path{std::begin(_source), std::end(_source)}
        {
        }

        template <typename InputIterator,
                  typename ValueType = typename std::iterator_traits<InputIterator>::value_type,
                  typename = std::enable_if_t<std::is_same_v<ValueType, value_type>>>
        path(InputIterator _begin, InputIterator _end)
            : value_{_begin, _end}
        {
        }

        ~path() = default;

        // Assignments

        auto operator=(const path& _p) -> path& = default;
        auto operator=(path&& _p) noexcept -> path& = default;

        auto operator=(string_type&& _source) -> path&
        {
            value_ = std::move(_source);
            return *this;
        }

        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable_v<Source>>>
        auto operator=(const Source& _source) -> path&
        {
            string_type new_path{std::begin(_source), std::end(_source)};
            value_ = std::move(new_path);
            return *this;
        }

        auto assign(string_type&& _source) -> path&
        {
            return *this = std::move(_source);
        }

        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable_v<Source>>>
        auto assign(const Source& _source) -> path&
        {
            return *this = _source;
        }

        template <typename InputIterator,
                  typename ValueType = typename std::iterator_traits<InputIterator>::value_type,
                  typename = std::enable_if_t<std::is_same_v<ValueType, value_type>>>
        auto assign(InputIterator _begin, InputIterator _end) -> path&
        {
            return *this = string_type{_begin, _end};
        }

        // Appends

        auto operator/=(const path& _p) -> path&
        {
            if (!_p.empty())
            {
                append_seperator_if_needed(_p);
                value_ += _p.value_;
            }

            return *this;
        }

        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable_v<Source>>>
        auto operator/=(const Source& _source) -> path&
        {
            return *this /= path{_source};
        }

        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable_v<Source>>>
        auto append(const Source& _source) -> path&
        {
            return *this /= path{_source};
        }

        template <typename InputIterator,
                  typename ValueType = typename std::iterator_traits<InputIterator>::value_type,
                  typename = std::enable_if_t<std::is_same_v<ValueType, value_type>>>
        auto append(InputIterator _begin, InputIterator _end) -> path&
        {
            return *this /= path{_begin, _end};
        }

        // Concatenation

        auto operator+=(const path& _p) -> path&        { value_ += _p.value_; return *this; }
        auto operator+=(const string_type& _p) -> path& { value_ += _p; return *this; }
        auto operator+=(std::string_view _p) -> path&   { value_ += _p; return *this; }
        auto operator+=(const value_type* _p) -> path&  { value_ += _p; return *this; }
        auto operator+=(value_type _p) -> path&         { value_ += _p; return *this; }

        template <typename Source>
        auto operator+=(const Source& _p) -> path&
        {
            return *this += path{_p};
        }

        // template <typename CharT>
        // auto operator+=(CharT _p) -> path&;

        template <typename InputIterator,
                  typename ValueType = typename std::iterator_traits<InputIterator>::value_type,
                  typename = std::enable_if_t<std::is_same_v<ValueType, value_type>>>
        auto concat(InputIterator _begin, InputIterator _end) -> path&
        {
            return *this += path{_begin, _end};
        }

        // Modifiers

        auto clear() -> void
        {
            value_.clear();
        }

        auto remove_data_object_name() -> path&
        {
            return *this;
            //return *this = parent_path();
        }

        auto replace_extension(const path& _new_extension = {}) -> path&
        {
            return *this;
        }

        auto swap(path& _rhs) -> void
        {
            std::swap(value_, _rhs.value_);
        }

        // Lexical operations

        auto lexically_normal() const -> path;
        auto lexically_relative(const path& _base) const -> path;
        auto lexically_proximate(const path& _base) const -> path;

        // Format observers

        operator const value_type*() const noexcept { return value_.c_str(); }
        operator const string_type() const          { return value_; }
        operator string_type() const                { return value_; }

        auto string() const -> const string_type&   { return value_; }

        // Compare

        auto compare(const path& _p) const noexcept -> int;
        auto compare(const string_type& _p) const -> int;
        auto compare(const value_type* _p) const -> int;

        // Decomposition

        auto root_name() const -> path;
        auto root_collection() const -> path;
        auto root_path() const -> path;
        auto relative_path() const -> path;
        auto parent_path() const -> path;
        auto data_object_name() const -> path;
        auto stem() const -> path;
        auto extension() const -> path;

        // Query

        auto empty() const -> bool                       { return value_.empty(); }
        auto data_object_name_is_dot() const -> bool     { return dot == data_object_name(); }
        auto data_object_name_is_dot_dot() const -> bool { return dot_dot == data_object_name(); }
        auto has_root_name() const -> bool               { return !root_name().empty(); }
        auto has_root_collection() const -> bool         { return !root_collection().empty(); }
        auto has_root_path() const -> bool               { return !root_path().empty(); }
        auto has_relative_path() const -> bool           { return !relative_path().empty(); }
        auto has_parent_path() const -> bool             { return !parent_path().empty(); }
        auto has_data_object_name() const -> bool        { return !data_object_name().empty(); }
        auto has_stem() const -> bool                    { return !stem().empty(); }
        auto has_extension() const -> bool               { return !extension().empty(); }
        auto is_absolute() const -> bool                 { return separator == value_.front(); }
        auto is_relative() const -> bool                 { return !is_absolute(); }

        // Iterators

        auto begin() const -> iterator;
        auto end() const -> iterator;

        auto rbegin() const -> reverse_iterator;
        auto rend() const -> reverse_iterator;

    private:
        void append_seperator_if_needed(const path& _p)
        {
            if (value_.empty() ||
                separator == value_.back() ||
                separator == _p.value_.front())
            {
                return;
            }

            value_ += separator;
        }

        string_type value_;
    };

    class path::iterator
    {
    public:
        using value_type        = path;
        using pointer           = value_type*;
        using reference         = value_type&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator();

        iterator(const iterator& _other);
        auto operator=(const iterator& _other) -> iterator&;

        iterator(iterator&& _other);
        auto operator=(iterator&& _other) -> iterator&;

        ~iterator();

        auto operator==(const iterator& _other) -> bool;
        auto operator!=(const iterator& _other) -> bool;

        auto operator*() -> reference;
        auto operator->() -> pointer;

        auto operator++() -> iterator&;
        auto operator++(int) -> iterator;
        auto operator--() -> iterator&;
        auto operator--(int) -> iterator;

        auto swap(iterator& _other);

    private:
        path& path_;
        boost::split_iterator<
    };

    class path::reverse_iterator
    {
    public:

    private:
    };

//#include "irods_path.tpp"

} // namespace irods::filesystem

#endif // IRODS_FILESYSTEM_PATH_HPP
