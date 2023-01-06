///----------------------------------
/// @file mpl/fold.hpp 
/// @brief ���^�֐��ɂ��^�p�����[�^�p�b�N�̏�ݍ���
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_FOLD_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_FOLD_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief ������E�ւ̏�ݍ��ݓ�������
    */
    template <MetaCallable F, class... Parameters> struct foldl_impl;
    template <MetaCallable F, class Init, class Head, class... Parameters>
    struct foldl_impl<F, Init, Head, Parameters...> : public foldl_impl<F, apply_t<F, Init, Head>, Parameters...> {};
    template <MetaCallable F, class Init, class Head>
    struct foldl_impl<F, Init, Head> : public apply<F, Init, Head> {};

    /**
     * @fn
     * @brief ������E�ւ̏�ݍ���
     * @tparam F ��ݍ��݂Ŏ��s���郁�^�֐�
     * @tparam Init �������q
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class Init, class TypeList> struct foldl;
    template <MetaCallable F, class Init, template <class...> class List, class... Parameters>
    struct foldl<F, Init, List<Parameters...>> : public foldl_impl<F, Init, Parameters...> {};

    /**
     * @fn
     * @brief ������E�ւ̏�ݍ��݂��s���A���ʂ̌^���擾����
     * @tparam F ��ݍ��݂Ŏ��s���郁�^�֐�
     * @tparam Init �������q
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class Init, class TypeList> using foldl_t = foldl<F, Init, TypeList>::type;

    /**
     * @fn
     * @brief �E���獶�ւ̏�ݍ��ݓ�������
    */
    template <MetaCallable F, class... Parameters> struct foldr_impl;
    template <MetaCallable F, class Head, class... Parameters>
    struct foldr_impl<F, Head, Parameters...> : public apply<F, Head, typename foldr_impl<F, Parameters...>::type> {};
    template <MetaCallable F, class Head, class Next>
    struct foldr_impl<F, Head, Next> : public apply<F, Head, Next> {};

    /**
     * @fn
     * @brief �E���獶�ւ̏�ݍ���
     * @tparam F ��ݍ��݂Ŏ��s���郁�^�֐�
     * @tparam Init �������q
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class Init, class TypeList> struct foldr;
    template <MetaCallable F, class Init, template <class...> class List, class... Parameters>
    struct foldr<F, Init, List<Parameters...>> : public foldr_impl<F, Parameters..., Init> {};

    /**
     * @fn
     * @brief �E���獶�ւ̏�ݍ���
     * @tparam F ��ݍ��݂Ŏ��s���郁�^�֐�
     * @tparam Init �������q
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class Init, class TypeList> using foldr_t = foldr<F, Init, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_FOLD_HPP