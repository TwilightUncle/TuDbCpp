///----------------------------------
/// @file mpl/argment.hpp 
/// @brief ���^�֐������֘A�̑���
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief ��������^�֐��̈����̏��Ԃ����ւ��������Ŏ��s
     * @tparam F ���^�֐�
     * @tparam Arg2 ���^�֐��̓�ڂɎw�肵��������
     * @tparam Arg1 ���^�֐��̈�ڂɎw�肵��������
    */
    template <MetaCallable F, class Arg2, class Arg1>
    struct flip : public apply<F, Arg1, Arg2> {};

    /**
     * @fn
     * @brief ��������^�֐��̈����̏��Ԃ����ւ��������Ŏ��s
     * @tparam F ���^�֐�
     * @tparam Arg2 ���^�֐��̓�ڂɎw�肵��������
     * @tparam Arg1 ���^�֐��̈�ڂɎw�肵��������
    */
    template <MetaCallable F, class Arg2, class Arg1> using flip_t = flip<F, Arg2, Arg1>::type;

    template <MetaCallable F, class Arg2, class Arg1> constexpr auto flip_v = flip<F, Arg2, Arg1>::value;

    /**
     * @fn
     * @brief ���^�֐����X�g��Arg��H�킹�A���ʂ𒀎����s����
     * @tparam Arg �����l
     * @tparam MetaFuncList ���^�֐����X�g
    */
    template <class Arg, class MetaFuncList> struct relay;
    template <class Arg, template <class...> class List, MetaCallable... Funcs>
    struct relay<Arg, List<Funcs...>> : public foldr<quote<apply>, Arg, reverse_t<List<Funcs...>>> {};

    /**
     * @fn
     * @brief ���^�֐����X�g��Arg��H�킹�A���ʂ𒀎����s����
     * @tparam Arg �����l
     * @tparam MetaFuncList ���^�֐����X�g
    */
    template <class Arg, class MetaFuncList> using relay_t = relay<Arg, MetaFuncList>::type;

    /**
     * @fn
     * @brief ���X�g�Ƃ��ēn���ꂽ�^���X�g�������Ƃ��Ċ֐��ɓK�p����
     * @tparam F ���^�֐�
     * @tparam ArgList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class ArgList> struct apply_list {};
    template <MetaCallable F, template <class...> class List, class... Args>
    struct apply_list<F, List<Args...>> : public apply<F, Args...> {};

    /**
     * @fn
     * @brief ���X�g�Ƃ��ēn���ꂽ�^���X�g�������Ƃ��Ċ֐��ɓK�p����
     * @tparam F ���^�֐�
     * @tparam ArgList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class ArgList> using apply_list_t = apply_list<F, ArgList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP
