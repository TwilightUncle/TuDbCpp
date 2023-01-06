///----------------------------------
/// @file mpl/find_if.hpp 
/// @brief ���^�֐��ɂ��^�p�����[�^�p�b�N�̌���
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_FIND_IF_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_FIND_IF_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief ��������
    */
    template <MetaCallable Pred, class... Parameters> struct find_if_impl;
    template <MetaCallable Pred, class Head, class... Parameters>
    struct find_if_impl<Pred, Head, Parameters...> : public std::conditional_t<
        apply_v<Pred, Head>,
        std::type_identity<Head>,
        find_if_impl<Pred, Parameters...>
    > {};
    template <MetaCallable Pred>
    struct find_if_impl<Pred> : public std::type_identity<ignore_type> {};

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���ŏ��̌^���擾
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> struct find_if;
    template <MetaCallable Pred, template <class...> class List, class... Parameters>
    struct find_if<Pred, List<Parameters...>> : public find_if_impl<Pred, Parameters...> {};

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���ŏ��̌^���擾
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> using find_if_t = find_if<Pred, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_FIND_IF_HPP
