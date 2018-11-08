// Constructors and destructor

// Delegating constructor
template <typename Source,
          typename = std::enable_if_t<path_traits::is_pathable_v<Source>>>
path::path(const Source& _source)
    : path{std::begin(_source), std::end(_source)}
{
}

// The iterator's value_type must be of type "char".
template <typename InputIterator,
          typename = std::enable_if_t<std::is_same_v<InputIterator::value_type, char>>>
path::path(InputIterator _begin, InputIterator _end)
    : path_{_begin, _end}
{
}

// Assignments

template <typename Source,
          typename = std::enable_if_t<path_traits::is_pathable_v<Source>>>
auto path::operator=(const Source& _source) -> path&
{
    std::string temp{std::begin(_source), std::end(_source)};
    path_ = std::move(temp);
    return *this;
}

