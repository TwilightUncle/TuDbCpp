///----------------------------------
/// @file mpl/sort.hpp 
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

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��������Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> struct rotatel;
    template <template <class...> class List, class Head, class... Parameters>
    struct rotatel<List<Head, Parameters...>> : public std::type_identity<List<Parameters..., Head>> {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��������Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> using rotatel_t = rotatel<TypeList>::type;

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��E�����Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> struct rotater;
    template <template <class...> class List, class... Parameters>
    struct rotater<List<Parameters...>> : public reverse<rotatel_t<reverse_t<List<Parameters...>>>> {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��E�����Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> using rotater_t = rotater<TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
