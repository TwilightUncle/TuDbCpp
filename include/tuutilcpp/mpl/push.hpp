///----------------------------------
/// @file mpl/push_pop.hpp 
/// @brief ���^�֐��ɂ��^�p�����[�^�p�b�N�̈ꊇ�K�p
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_PUSH_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_PUSH_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̍Ō���Ɍ^��ǉ�����
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam T �ǉ�����^
    */
    template <class TypeList, class T> struct push_back;
    template <template <class...> class List, class... Parameters, class T>
    struct push_back<List<Parameters...>, T> : public std::type_identity<List<Parameters..., T>> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̍Ō���Ɍ^��ǉ�����
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam T �ǉ�����^
    */
    template <class TypeList, class T> using push_back_t = push_back<TypeList, T>::type;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̐擪�Ɍ^��ǉ�����
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam T �ǉ�����^
    */
    template <class TypeList, class T> struct push_front;
    template <template <class...> class List, class... Parameters, class T>
    struct push_front<List<Parameters...>, T> : public std::type_identity<List<T, Parameters...>> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̐擪�Ɍ^��ǉ�����
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam T �ǉ�����^
    */
    template <class TypeList, class T> using push_front_t = push_front<TypeList, T>::type;

    /**
     * @fn
     * @brief �ǉ����悤�Ƃ���^�������ɍ��v����ꍇ�̂݃p�����[�^�p�b�N�̍Ō���Ɍ^��ǉ�����
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam T �ǉ�����^
    */
    template <MetaCallable Pred, class TypeList, class T> struct push_back_if;
    template <MetaCallable Pred, class T, template <class...> class List, class... Parameters>
    struct push_back_if<Pred, List<Parameters...>, T> : public std::conditional<
        apply_v<Pred, T>,
        List<Parameters..., T>,
        List<Parameters...>
    > {};

    /**
     * @fn
     * @brief �ǉ����悤�Ƃ���^�������ɍ��v����ꍇ�̂݃p�����[�^�p�b�N�̍Ō���Ɍ^��ǉ�����
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam T �ǉ�����^
    */
    template <MetaCallable Pred, class TypeList, class T> using push_back_if_t = push_back_if<Pred, TypeList, T>::type;

    /**
     * @fn
     * @brief �ǉ����悤�Ƃ���^�������ɍ��v����ꍇ�̂݃p�����[�^�p�b�N�̐擪�Ɍ^��ǉ�����
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam T �ǉ�����^
    */
    template <MetaCallable Pred, class TypeList, class T> struct push_front_if;
    template <MetaCallable Pred, class T, template <class...> class List, class... Parameters>
    struct push_front_if<Pred, List<Parameters...>, T> : public std::conditional<
        apply_v<Pred, T>,
        List<T, Parameters...>,
        List<Parameters...>
    > {};

    /**
     * @fn
     * @brief �ǉ����悤�Ƃ���^�������ɍ��v����ꍇ�̂݃p�����[�^�p�b�N�̐擪�Ɍ^��ǉ�����
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam T �ǉ�����^
    */
    template <MetaCallable Pred, class TypeList, class T> using push_front_if_t = push_front_if<Pred, TypeList, T>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_PUSH_HPP
