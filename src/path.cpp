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
        auto join(path::iterator _first, path::iterator _last) -> path
        {
            path p;

            for (; _first != _last; ++_first) {
                p /= *_first;
            }

            return p;
        }
    } // anonymous namespace

    //
    // Path
    //

    auto path::operator/=(const path& _p) -> path&
    {
        if (!_p.empty()) {
            append_seperator_if_needed(_p);
            value_ += _p.value_;
        }

        return *this;
    }

    auto path::remove_object_name() -> path&
    {
        if (const auto pos = value_.find_last_of(separator); string_type::npos != pos) {
            value_.erase(pos + 1);
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

        path p;

        for (const auto& e : *this)
        {
            if (e.object_name_is_dot()) {
                continue;
            }

            if (e.object_name_is_dot_dot()) {
                p.remove_object_name();
                continue;
            }

            p /= e;
        }

        return p;
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
        if (has_root_collection()) {
            return join(++begin(), end());
        }

        return empty() ? path{} : *this;
    }

    auto path::parent_path() const -> path
    {
        return (empty() || begin() == --end())
            ? path{}
            : join(begin(), --end());
    }

    auto path::object_name() const -> path
    {
        return relative_path().empty() ? path{} : *--end();
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

        // If we're just before the "end" iterator and the currrent value of
        // the element is empty, then we know the path ended with a separator and
        // therefore, the iterator should now be set to the "end" iterator.
        if (fp.size() - 1 == pos_ && e.empty()) {
            ++pos_;
            return *this;
        }

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
            e.clear();
            pos_ = fp.size() - 1;
        }
        // Found a character that is not a separator.
        else if (const auto end = fp.find_first_of(separator, pos_);
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

        return *this;
    }

    auto path::iterator::operator--() -> iterator&
    {
        const auto& fp = path_ptr_->value_; // Full path
        auto& e = element_.value_;          // Path element

        // Handle trailing separator at the end of the path.
        // If the iterator represents the end iterator and the character preceding it
        // is a separator, then set the current element to the dot path.
        if (fp.size() == pos_ &&
            fp.size() > 1 &&
            detail::is_separator(fp[pos_ - 1]))
        {
            --pos_;
            e.clear();
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
        for (; (_first1 != _last1) && (_first2 != _last2); ++_first1, ++_first2) {
            if (_first1->string() < _first2->string()) {
                return true;
            }

            if (_first2->string() < _first1->string()) {
                return false;
            }
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

