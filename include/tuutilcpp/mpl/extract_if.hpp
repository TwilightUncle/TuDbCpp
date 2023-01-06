///----------------------------------
/// @file mpl/extract_if.hpp 
/// @brief ���^�֐��ɂ��^�p�����[�^�p�b�N�̒��o
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief ��������
    */
    template <MetaCallable Pred, class... TypeList> struct extract_if_impl;
    template <MetaCallable Pred, template <class...> class List, class Head, class... Parameters>
    struct extract_if_impl<Pred, List<Head, Parameters...>> : public std::conditional_t<
        apply_v<Pred, Head>,
        foldl<bind<quote<push_back_if>, Pred>, List<Head>, List<Parameters...>>,
        extract_if_impl<Pred, List<Parameters...>>
    > {};
    template <MetaCallable Pred, template <class...> class List, class Head>
    struct extract_if_impl<Pred, List<Head>>
        : public std::conditional<apply_v<Pred, Head>, List<Head>, ignore_type>
    {};

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���^�𒊏o
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> struct extract_if;
    template <MetaCallable Pred, template <class...> class List, class... Parameters>
    struct extract_if<Pred, List<Parameters...>> : public extract_if_impl<Pred, List<Parameters...>> {};

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���^�𒊏o
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> using extract_if_t = extract_if<Pred, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
