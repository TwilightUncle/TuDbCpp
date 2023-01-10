///----------------------------------
/// @file mpl/value_constant.hpp 
/// @brief ��^�e���v���[�g�p�����[�^���^�e���v���[�g�p�����[�^�Ɏ����グ��
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_VALUE_CONSTANT_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_VALUE_CONSTANT_HPP

namespace tuutil::mpl
{
    /**
     * @class
     * @brief �����N���X�̃e���v���[�g�B�l���^�֎����グ�Ȃ���Ais_same�̌��ʂ͌^�ƒl���Ƃ��Ɉ�v�������̂ݐ^�ƂȂ�
     * @tparam V �R���p�C�����]���\�ȔC�ӌ^�̒l
    */
    template <auto V>
    struct value_constant : public std::type_identity<decltype(V)>
    {
        static constexpr auto value = V;
    };

    /**
     * @fn
     * @brief �e���v���[�g�����Ŏw�肵���l�����̂܂ܕԂ�
     * @tparam V �R���p�C�����]���\�ȔC�ӌ^�̒l
    */
    template <auto V> constexpr auto value_constant_v = value_constant<V>::value;

    /**
     * @fn
     * @brief decltype�ƈꏏ
     * @tparam V �R���p�C�����]���\�ȔC�ӌ^�̒l
    */
    template <auto V> using value_constant_t = value_constant<V>::type;

    /**
     * @fn
     * @brief value_constant���ǂ�������
    */
    template <class T> struct is_value_constant : public std::false_type {};
    template <auto V> struct is_value_constant<value_constant<V>> : public std::true_type {};

    /**
     * @fn
     * @brief value_constant���ǂ�������
    */
    template <class T> constexpr auto is_value_constant_v = is_value_constant<T>::value;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N���w��̃e���v���[�g�N���X��ێ�����
    */
    template <template <auto...> class List> struct liftv {};

    /**
     * @fn
     * @brief liftedv�^������
    */
    template <class T> struct is_liftedv : public std::false_type {};
    template <template <auto...> class List> struct is_liftedv<liftv<List>> : public std::true_type {};

    /**
     * @fn
     * @brief liftedv�^������
    */
    template <class T> constexpr auto is_liftedv_v = is_liftedv<T>::value;

    /**
     * @fn
     * @brief lift������^�e���v���[�g�ɔ�^�e���v���[�g�p�����[�^��K�p����
    */
    template <class LiftedList, auto... Parameters> struct applyv;
    template <template <auto...> class List, auto... Parameters>
    struct applyv<liftv<List>, Parameters...> : public std::type_identity<List<Parameters...>> {};

    /**
     * @fn
     * @brief lift������^�e���v���[�g�ɔ�^�e���v���[�g�p�����[�^��K�p����
    */
    template <class LiftedList, auto... Parameters> using applyv_t = applyv<LiftedList, Parameters...>::type;

    /**
     * @fn
     * @brief �l���X�g�̊e�v�f��value_constant�Ō^�Ɏ����グ�����X�g��Ԃ�
    */
    template <class ValueList> struct pack_type;
    template <template <auto...> class List, auto... Parameters>
    struct pack_type<List<Parameters...>>
        : public std::type_identity<type_list<liftv<List>, value_constant<Parameters>...>>
    {};

    /**
     * @fn
     * @brief �l���X�g�̊e�v�f��value_constant�Ō^�Ɏ����グ�����X�g��Ԃ�
    */
    template <class ValueList> using pack_type_t = pack_type<ValueList>::type;

    /**
     * @fn
     * @brief pack_type���ꂽ�l�����Ƃɖ߂�
    */
    template <class TypeList> struct unpack_type;
    template <template <class...> class List, class Head, class... Parameters>
    requires (is_liftedv_v<Head> && (is_value_constant_v<Parameters> && ...))
    struct unpack_type<List<Head, Parameters...>> : public applyv<Head, Parameters::value...> {};

    /**
     * @fn
     * @brief pack_type���ꂽ�l�����Ƃɖ߂�
    */
    template <class TypeList> using unpack_type_t = unpack_type<TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_VALUE_CONSTANT_HPP
