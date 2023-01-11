///----------------------------------
/// @file mpl/concepts.hpp
/// @brief �ėp�̐����`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_CONCEPTS_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_CONCEPTS_HPP

namespace tuutil::mpl
{
    /**
     * @brief �񋓌^�ł��邱�Ƃ���������R���Z�v�g(�ق�Ƃ�scoped_enum�𔻒肵������c++23����)
    */
    template <class T> concept Enumeration = std::is_enum_v<T>;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_CONCEPTS_HPP
