#include <irods/filesystem/path.hpp>

#include <irods/filesystem/detail.hpp>

#include <iostream>
#include <iomanip>
#include <string>

#include <boost/algorithm/string.hpp>

namespace irods::filesystem
{
    namespace
    {
        path join(path::iterator _first, path::iterator _last)
        {
            path p;

            for (; _first != _last; ++_first)
            {
                p /= *_first;
            }

            return p;
        }

        void replace_consecutive_separators(std::string& _p)
        {
            auto pos = _p.find_first_of('/');
            
            while (pos != std::string::npos)
            {
                if (auto end = _p.find_first_not_of('/', pos); end != std::string::npos)
                {
                    _p.replace(pos, end - pos, "/");
                    pos = _p.find_first_of('/', pos + 1);
                }
                else
                {
                    break;
                }
            }
        }

        void remove_dot_separators(std::string& _p)
        {
            auto pos = _p.find_first_of('.');
            
            while (pos != std::string::npos)
            {
                if (pos + 1 < _p.size())
                {
                    switch (_p[pos + 1])
                    {
                        case '/':
                            _p.erase(pos, 2);
                            pos = _p.find_first_of('.', pos);
                            break;
                            
                        case '.':
                            pos = _p.find_first_of('.', pos + 2);
                            break;
                            
                        default:
                            pos = _p.find_first_of('.', pos + 1);
                            break;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        void remove_name_followed_by_dot_dot_separator(std::string& _p)
        {
            auto pos = _p.find_first_not_of("/");
            
            while (pos != std::string::npos)
            {
                auto end = _p.find_first_of('/', pos);
                
                if (end == std::string::npos)
                    break;
                    
                if (end + 2 < _p.size() &&
                    _p[end + 1] == '.' &&
                    _p[end + 2] == '.')
                {
                    if (end + 3 < _p.size() && _p[end + 3] == '/')
                    {
                        end += 4;
                    }
                    else
                    {
                        end += 3;
                    }
                    
                    _p.erase(pos, end - pos);
                    pos = _p.find_first_not_of("/", pos);
                }
                else
                {
                    pos = _p.find_first_not_of("/", end);
                }
            }
        }

        void remove_root_dot_dot_separators(std::string& _p)
        {
            if (auto pos = _p.find_first_of('/'); pos != 0)
                return;
                
            using size_type = std::string::size_type;
            
            for (size_type i = 1; i < _p.size(); )
            {
                if (_p[i] == '.' &&
                    i + 2 < _p.size() &&
                    _p[i + 1] == '.' &&
                    _p[i + 2] == '/')
                {
                    auto end = i + 3;
                    while (end < _p.size() && _p[end] == '/')
                        ++end;
                    _p.erase(i, end - i);
                    i = end;
                }
                else
                {
                    ++i;
                }
            }
        }
    } // anonymous namespace

    //
    // Path
    //

    auto path::operator/=(const path& _p) -> path&
    {
        if (!_p.empty())
        {
            append_seperator_if_needed(_p);
            value_ += _p.value_;
        }

        return *this;
    }

    auto path::replace_extension(const path& _new_extension) -> path&
    {
        return *this;
    }

    auto path::lexically_normal() const -> path
    {
        if (empty()) {
            return *this;
        }

        auto normalized_value = value_;

        replace_consecutive_separators(normalized_value);
        remove_dot_separators(normalized_value);
        remove_name_followed_by_dot_dot_separator(normalized_value);
        remove_root_dot_dot_separators(normalized_value);

        return {normalized_value};

        /*
        for (const auto& e : *this)
        {
            if (e.object_name_is_dot())
            {
                p += '/';
                continue;
            }

            if (e.object_name_is_dot_dot())
            {
                p = join(p.begin(), --p.end());
            }
            else
            {
                p /= e;
            }
        }

        return p;
        */
    }

    auto path::compare(const path& _p) const noexcept -> int
    {
        return lexicographical_compare(begin(), end(), _p.begin(), _p.end());
    }

    auto path::root_collection() const -> path
    {
        return !empty() && is_absolute() ? *begin() : path{};
    }

    auto path::relative_path() const -> path
    {
        return (!empty() || has_root_collection())
            ? join(++begin(), end())
            : path{};
    }

    auto path::parent_path() const -> path
    {
        return (empty() || begin() == --end())
            ? path{}
            : join(begin(), --end());
    }

    auto path::object_name() const -> path
    {
        return empty() ? path{} : *--end();
    }

    auto path::stem() const -> path
    {
        if (empty() || object_name_is_dot() || object_name_is_dot_dot())
        {
            return {};
        }

        const auto n = object_name();

        if (auto pos = n.value_.find_last_of(dot); string_type::npos != pos)
        {
            return n.value_.substr(0, pos);
        }

        return {};
    }

    auto path::extension() const -> path
    {
        if (empty() || object_name_is_dot() || object_name_is_dot_dot())
        {
            return {};
        }

        const auto n = object_name();

        if (auto pos = n.value_.find_last_of(dot); string_type::npos != pos)
        {
            return n.value_.substr(pos);
        }

        return {};
    }

    auto path::begin() const -> iterator
    {
        return iterator{*this};
    }

    auto path::end() const -> iterator
    {
        iterator it;
        it.path_ptr_ = this;
        it.pos_ = value_.size();
        return it;
    }

    auto path::rbegin() const -> reverse_iterator
    {
        return reverse_iterator{end()};
    }

    auto path::rend() const -> reverse_iterator
    {
        return reverse_iterator{begin()};
    }

    auto path::append_seperator_if_needed(const path& _p) -> void
    {
        if (value_.empty() ||
            separator == value_.back() ||
            separator == _p.value_.front())
        {
            return;
        }

        value_ += separator;
    }

    //
    // Iterator
    //
    
    path::iterator::iterator(const path& _p)
        : path_ptr_{&_p}
        , element_{}
        , pos_{}
    {
        if (path_ptr_->empty())
        {
            return;
        }

        // Does the path contain a leading forward slash "/"?
        if (path_ptr_->is_absolute())
        {
            element_.value_ = path::separator;
        }
        else
        {
            const auto& full_path = path_ptr_->value_;
            const auto end = full_path.find_first_of(path::separator);

            element_.value_ = (path::string_type::npos != end)
                ? full_path.substr(0, end)
                : full_path;
        }
    }

    auto path::iterator::operator++() -> iterator&
    {
        const auto& fp = path_ptr_->value_; // Full path
        auto& e = element_.value_;          // Path element

        // Skip the element currently pointed to.
        pos_ += e.size();

        // If we're at the end of the path, then we're done.
        if (fp.size() == pos_)
        {
            e.clear();
            return *this;
        }

        // If we're not at the end of the path, then we're most likely at a separator.
        // Skip consecutive separators.
        while (detail::is_separator(fp[pos_]))
        {
            ++pos_;
        }

        if (fp.size() == pos_ && detail::is_separator(fp[fp.size() - 1]))
        {
            // Found a trailing separator.
            e = path::dot;
            pos_ = fp.size() - 1;
        }
        else
        {
            // Found a character that is not a separator.
            if (const auto end = fp.find_first_of(separator, pos_);
                path::string_type::npos != end)
            {
                // Found a separator.
                e = fp.substr(pos_, end - pos_);
            }
            else
            {
                // No trailing separator found.
                e = fp.substr(pos_, fp.size() - pos_);
            }
        }
        /*
        if (const auto start = fp.find_first_not_of(separator, pos_);
            path::string_type::npos != start)
        {
            // Found a character that is not a separator.
            pos_ = start;

            if (const auto end = fp.find_first_of(separator, pos_);
                path::string_type::npos != end)
            {
                // Found a separator.
                e = fp.substr(start, end - start);
            }
            else
            {
                // No trailing separator found.
                e = fp.substr(start, fp.size() - start);
            }
        }
        else if (detail::is_separator(fp[fp.size() - 1]))
        {
            // Found a trailing separator.
            e = path::dot;
            pos_ = fp.size() - 1;
        }
        */

        return *this;
    }

    auto path::iterator::operator--() -> iterator&
    {
        const auto& fp = path_ptr_->value_; // Full path
        auto& e = element_.value_;          // Path element

        // Handle trailing separator at the end of the path.
        // If the iterator represents the end iterator and the character preceding it
        // is a separator, then set the current element must be set to the dot path.
        if (fp.size() == pos_ &&
            fp.size() > 1 &&
            detail::is_separator(fp[pos_ - 1]))
        {
            --pos_;
            e = dot;
            return *this;
        }

        if (detail::is_separator(fp[pos_ - 1]))
        {
            // We've reached the root separator of the path.
            if (0 == pos_ - 1)
            {
                --pos_;
                e = separator;
                return *this;
            }

            // Point at the separator just after the preceding element.
            // Separators represent the end of a path element.
            while (pos_ > 0 && detail::is_separator(fp[pos_ - 1]))
            {
                --pos_;
            }
        }

        const auto end = pos_;

        // Find the start of the path element.
        while (pos_ > 0 && !detail::is_separator(fp[pos_ - 1]))
        {
            --pos_;
        }

        e = fp.substr(pos_, end - pos_);

        return *this;
    }

    auto lexicographical_compare(path::iterator _first1, path::iterator _last1,
                                 path::iterator _first2, path::iterator _last2) -> bool
    {
        for (; (_first1 != _last1) && (_first2 != _last2); ++_first1, ++_first2)
        {
            if (_first1->string() < _first2->string()) return true;
            if (_first2->string() < _first1->string()) return false;
        }

        return (_first1 == _last1) && (_first2 != _last2);
    }

    auto operator<<(std::ostream& _os, const path& _p) -> std::ostream&
    {
        return _os << std::quoted(_p.string());
    }

    auto operator>>(std::istream& _is, path& _p) -> std::istream&
    {
        path::string_type t;
        _is >> std::quoted(t);
        _p = t;
        return _is;
    }
} // namespace irods::filesystem

