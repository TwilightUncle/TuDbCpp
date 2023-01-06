///----------------------------------
/// @file mpl/find_if.hpp 
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
    // �����܂ŏ����𖞂����^���Ȃ������ꍇ
    template <MetaCallable Pred, template <class...> class List, class Head, class... Parameters>
    struct extract_if_impl<Pred, List<Head, Parameters...>> : public std::conditional_t<
        apply_v<Pred, Head>,
        extract_if_impl<Pred, List<Parameters...>, List<Head>>,
        extract_if_impl<Pred, List<Parameters...>>
    > {};
    template <MetaCallable Pred, template <class...> class List, class Head>
    struct extract_if_impl<Pred, List<Head>> : public std::conditional<
        apply_v<Pred, Head>,
        List<Head>,
        ignore_type
    > {};
    // ���Ɋ�������𖞂����^���������Ă���
    template <MetaCallable Pred, template <class...> class List, class Head, class... Parameters, class... Parameters2>
    struct extract_if_impl<Pred, List<Head, Parameters...>, List<Parameters2...>> : public std::conditional_t<
        apply_v<Pred, Head>,
        extract_if_impl<Pred, List<Parameters...>, List<Parameters2..., Head>>,
        extract_if_impl<Pred, List<Parameters...>, List<Parameters2...>>
    > {};
    template <MetaCallable Pred, template <class...> class List, class Head, class... Parameters2>
    struct extract_if_impl<Pred, List<Head>, List<Parameters2...>> : public std::conditional<
        apply_v<Pred, Head>,
        List<Parameters2..., Head>,
        List<Parameters2...>
    > {};

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���ŏ��̌^���擾
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> struct extract_if;
    template <MetaCallable Pred, template <class...> class List, class... Parameters>
    struct extract_if<Pred, List<Parameters...>> : public extract_if_impl<Pred, List<Parameters...>> {};

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���ŏ��̌^���擾
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> using extract_if_t = extract_if<Pred, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
