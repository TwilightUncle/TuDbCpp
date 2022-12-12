///----------------------------------
/// @file type.hpp
/// @brief DB�t�B�[���h�p�̌^��`
///----------------------------------
#ifndef TUDBCPP_INCLUDE_GUARD_TYPE_HPP
#define TUDBCPP_INCLUDE_GUARD_TYPE_HPP

#include <string>

namespace tudb
{
    template <std::size_t N> struct varchar : public std::string{};
    using smallint = std::int16_t;
    using integer = std::int32_t;
    using bigint = std::int64_t;

    // char(N)�͑g�ݍ��݂��̂܂܂̂��߁A�����ł̒�`�͍s��Ȃ�

    /// varchar(max)�̏ꍇ�Ɏg�p
    /// (e.g. varchar<varchar_max>
    constexpr std::size_t varchar_max = std::string{}.max_size();

    /**
     * @fn
     * @brief �����Ώۂ̌^���varchar�y�сAchar�n�z��̃T�C�Y���擾
     * @param T �����Ώۂ̌^
    */
    template <class T> struct get_maxlength_t;
    template <std::size_t N> struct get_maxlength_t<varchar<N>> { static constexpr auto size = N; };
    template <std::size_t N, typename T> struct get_maxlength_t<T[N]> { static constexpr auto size = N; };

    /**
     * @fn
     * @brief �����Ώۂ̒l���varchar�y�сAchar�v�̔z��̃T�C�Y���擾
     * @param v �����Ώۂ̒l
    */
    template <class T> constexpr std::size_t get_maxlength(const T& v) { return get_maxlength_t<T>::size; } 
}

#endif // TUDBCPP_INCLUDE_GUARD_TYPE_HPP
