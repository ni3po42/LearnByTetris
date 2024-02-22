#ifndef tetris_concepts_hpp
#define tetris_concepts_hpp

#include <type_traits>
#include <concepts>
#include <cstdint>

//this ended up not being used, it was an attempt
//to allow duck typing for similar shaped data
template <typename T, typename D>
concept GameCell = std::is_convertible_v<D, uint64_t> && requires (T c) {
    {c.row} -> std::convertible_to<int>;
    {c.col} -> std::convertible_to<int>;
    {c.data} -> std::same_as<D>;
};

#endif