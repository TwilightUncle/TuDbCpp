///----------------------------------
/// @file column.hpp
/// @brief DB���`
///----------------------------------
#pragma once
#include <constraint.hpp>
#include <type.hpp>

namespace tudb
{
    /**
     * @fn
     * @brief ���`�p���^�֐�(�����ɒ�`���e�Q�Ɨp�N���X)
    */
    template <
        enumeration auto ColID,
        StringLiteralSpecificable auto Name,
        class FieldType,
        ColumnConstraintDefinable... Constraints
    >
    struct define_column
    {
        static constexpr auto name = Name;
        static constexpr auto id = ColID;
        using id_type = decltype(ColID);
        using field_type = FieldType;
        using constraint_list = type_list<to_table_constraint_t<Constraints, ColID>...>;
    };

    /**
     * @fn
     * @brief column_definition�^���ǂ����𔻒肷�郁�^�֐�
    */
    template <class T> struct is_column_definition : public std::false_type {};
    template <enumeration auto ColID, StringLiteralSpecificable auto Name, typename FieldType, ColumnConstraintDefinable... Constraints>
    struct is_column_definition<define_column<ColID, Name, FieldType, Constraints...>> : public std::true_type {};

    /**
     * @brief define_column�ł��邱�Ƃ��������鐧��
    */
    template <class T>
    concept ColumnDefinable = is_column_definition<T>::value;

    template <ColumnDefinable T>
    struct get_column_id : public std::integral_constant<decltype(T::id), T::id> {};

    template <ColumnDefinable T>
    struct get_column_name { static constexpr auto value = T::name; };

    /**
     * �J������`���X�g�ł��邱��
    */
    template <class T>
    concept ColumnListDefinitionable = requires {
        // T�̓p�����[�^�p�b�N�������Ă���
        requires has_type_parameters_v<T>;

        // ������`�̃J���������݂��Ă͂����Ȃ�
        requires copy_types_t<T, is_unique>::value;

        // �e���v���[�g�������S�ăJ������`�ł��邱��(�S�Ẵp�����[�^���J������`�ł��邩�����X�g�Ƃ��Ď擾���A�_���ςŌ��ʂ��m�F)
        requires copy_types_t<
            map_types_t<is_column_definition, pass_types<T>>,
            std::conjunction
        >::value;
    };
}
