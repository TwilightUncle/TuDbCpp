///----------------------------------
/// @file tuutilcpp/value_type_pair.hpp
/// @brief �e���v���[�g��������n�̃w���p�Q
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_VALUE_TYPE_PAIR_HPP
#define TUUTILCPP_INCLUDE_GUARD_VALUE_TYPE_PAIR_HPP

namespace tudb
{
    /**
     * @class
     * @brief �l�ƌ^�̑g
    */
    template <auto V, class T>
    struct value_type_pair : public std::type_identity<T> {
        static constexpr auto key = V;
    };

    /**
     * @fn
     * @brief �l�ƌ^�̑g�ł��邩����
    */
    template <class T> struct is_value_type_pair : public std::false_type {};
    template <auto V, class T>
    struct is_value_type_pair<value_type_pair<V, T>> : public std::true_type {};

    /**
     * �w�肵���l�ƁA�l�ƌ^�̑g�̃L�[��������������
    */
    template <auto V, class T> struct equal_key : public std::false_type {};
    template <auto V, auto Key, class T>
    struct equal_key<V, value_type_pair<Key, T>> : public equal_values<V, Key> {};

    template <class T>
    concept AssociativeTypeList = requires {
        requires has_type_parameters_v<T>;

        // �e���v���[�g�������l�ƌ^�̑g�ł��邱��(�S�Ẵp�����[�^���J������`�ł��邩�����X�g�Ƃ��Ď擾���A�_���ςŌ��ʂ��m�F)
        requires copy_types_t<
            map_types_t<is_value_type_pair, pass_types<T>>,
            std::conjunction
        >::value;
    };

    /**
     * @fn
     * @brief �^�̘A�z�z�񂩂�L�[����v����v�f���擾����
    */
    template <auto SearchKey, AssociativeTypeList List>
    struct get : public find_if_by_value<SearchKey, equal_key, pass_types<List>>::type {};
    template <auto SearchKey, AssociativeTypeList List> using get_t = get<SearchKey, List>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_VALUE_TYPE_PAIR_HPP
