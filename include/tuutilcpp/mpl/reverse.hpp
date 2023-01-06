///----------------------------------
/// @file mpl/reverse.hpp 
/// @brief �^�p�����[�^�p�b�N�̕��т𔽓]
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т𔽓]������
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> struct reverse;
    template <template <class...> class List, class Head, class... Parameters>
    struct reverse<List<Head, Parameters...>> : public foldl<quote<push_front>, List<Head>, List<Parameters...>> {}; 

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т𔽓]������
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> using reverse_t = reverse<TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
