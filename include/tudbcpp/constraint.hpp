///----------------------------------
/// @file constraint.hpp
/// @brief �񐧖��`
///----------------------------------
#ifndef TUDBCPP_INCLUDE_GUARD_CONSTRAINT_HPP
#define TUDBCPP_INCLUDE_GUARD_CONSTRAINT_HPP

#include <tuutilcpp/utility.hpp>

namespace tudb
{
    // /**
    //  * @brief �J������`�̐���������萔
    // */
    // enum class column_constraints
    // {
    //     PK, FK, NOT_NULL, UNIQUE, CHECK
    // };

    struct constraint {};
    struct constraint_c {};

    // �e�[�u����`���̎w��p
    template <enumeration auto... ColIds>
    struct primary_key : public constraint {};
    // �J������`���̎w��p
    struct pk : public constraint_c {};

    template <class... ColIdLists>
    concept ForeignKeyArgsSpecifiable = requires {
        // �ΏۂƎQ�Ɛ�̓��
        requires sizeof...(ColIdLists) == 2;
        // ��^�p�����[�^�p�b�N�����R���e�i�ł���
        requires (has_value_parameters_v<ColIdLists> && ...);
        // �ΏۂƎQ�Ɛ�̃J�����̐��͓����łȂ���΂Ȃ�Ȃ�
        requires (count_parameters_v<pass_values<ColIdLists>> == ...);
        // ���ꂼ��AIdList�����l�̌^�͑S�ē����łȂ���΂Ȃ�Ȃ�
        requires (is_same_types_v<pass_values<ColIdLists>> && ...);
        // ���ꂼ��AIdList�����l�͏d���������Ă͂Ȃ�Ȃ�
        requires (is_unique_v<pass_values<ColIdLists>> && ...);
        // ��^�p�����[�^�͗񋓑̂łȂ���΂����Ȃ�(��L�ŁA�����^�ł���m�F���Ƃ��Ă��邽�߁A�擪�̂݌���΂悢)
        requires (enumeration<get_first_type_t<pass_values<ColIdLists>>> && ...);
    };

    // �e�[�u����`���̎w��p
    template <class ColIdList, class RefColIdList>
    requires ForeignKeyArgsSpecifiable<ColIdList, RefColIdList>
    struct foreign_key : public constraint {};
    // �J������`���̎w��p
    template <enumeration auto ColId> struct fk : public constraint_c {};

    template <class T>
    concept ColumnConstraintDefinable = std::is_base_of_v<constraint_c, T> && !std::is_same_v<constraint_c, T>;

    template <class T> struct is_constraint_definition
        : public std::bool_constant<std::is_base_of_v<constraint, T>> {};

    template <class T>
    concept ConstraintDefinable = is_constraint_definition<T>::value;

    template <class T>
    concept ConstraintListDefinable = requires {
        // T�̓p�����[�^�p�b�N�������Ă���
        requires has_type_parameters_v<T>;

        // ������`�����݂��Ă͂����Ȃ�
        requires copy_types_t<T, is_unique>::value;

        // �e���v���[�g�������S�Đ����`�ł��邱��(�S�Ẵp�����[�^�������`�ł��邩�����X�g�Ƃ��Ď擾���A�_���ςŌ��ʂ��m�F)
        requires copy_types_t<
            map_types_t<is_constraint_definition, pass_types<T>>,
            std::conjunction
        >::value;

        // ������primary_key�����݂��Ă͂����Ȃ�
    };

    /**
     * @fn
     * @brief �J������`�p����ʏ�̃e�[�u����`�p�ɐ����
    */
    template <ColumnConstraintDefinable Constraint, enumeration auto ColId> struct to_table_constraint;
    template <enumeration auto ColId>
    struct to_table_constraint<pk, ColId> : public std::type_identity<primary_key<ColId>> {};
    template <enumeration auto ColId, enumeration auto ColId2>
    struct to_table_constraint<fk<ColId2>, ColId>
        : public std::type_identity<foreign_key<value_list<ColId>, value_list<ColId2>>> {};
    
    template <ColumnConstraintDefinable Constraint, enumeration auto ColId>
    using to_table_constraint_t = to_table_constraint<Constraint, ColId>::type;

    /**
     * �e�[�u���Ƃ��Đ���𖢎w��ł��邱��
    */
    using constraint_unspecified = type_list<constraint>;
}

#endif // TUDBCPP_INCLUDE_GUARD_CONSTRAINT_HPP
