#include <irods/filesystem/path.hpp>
#include <irods/filesystem/detail.hpp>

namespace irods::filesystem
{
    //
    // Path
    //

    auto path::root_collection() const -> path
    {
        return {};
    }

    auto path::relative_path() const -> path
    {
        return {};
    }

    auto path::parent_path() const -> path
    {
        if (empty() || begin() == --end())
        {
            return {};
        }

        path pp;

        for (auto it = begin(); it != --end(); ++it)
        {
            pp /= *it;
        }

        return pp;
    }

    auto path::object_name() const -> path
    {
        return empty() ? path{} : *--end();
    }

    auto path::stem() const -> path
    {
        return {};
    }

    auto path::extension() const -> path
    {
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
            e = path::dot;
            return *this;
        }

        if (detail::is_separator(fp[pos_ - 1]))
        {
            // We've reached the root separator of the path.
            if (pos_ - 1 == 0)
            {
                --pos_;
                e = separator;
                return *this;
            }

            // Point at the separator.
            // Separators represent the end of a path element.
            --pos_;
        }

        const auto end = pos_;

        while (pos_ > 0 && !detail::is_separator(fp[pos_ - 1])) {
            --pos_;
        }

        e = fp.substr(pos_, end - pos_);

        return *this;
    }
} // namespace irods::filesystem

