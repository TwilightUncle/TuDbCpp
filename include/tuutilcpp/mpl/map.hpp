///----------------------------------
/// @file mpl/fold.hpp 
/// @brief ���^�֐��ɂ��^�p�����[�^�p�b�N�̈ꊇ�K�p
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief ��������
    */
    template <MetaCallable F, template <class...> class List, class... Parameters>
    struct map_impl : public std::type_identity<List<apply_t<F, Parameters>...>> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�ɑ΂��郁�^�֐��̈ꊇ�K�p
     * @tparam F �K�p���郁�^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class TypeList> struct map;
    template <MetaCallable F, template <class...> class List, class... Parameters>
    struct map<F, List<Parameters...>> : public map_impl<F, List, Parameters...> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�ɑ΂��郁�^�֐��̈ꊇ�K�p
     * @tparam F �K�p���郁�^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class TypeList> using map_t = map<F, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP
