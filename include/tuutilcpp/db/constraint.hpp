///----------------------------------
/// @file constraint.hpp
/// @brief �񐧖��`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP

namespace tuutil::db
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
    template <mpl::Enumeration auto... ColIds>
    struct primary_key : public constraint {};
    // �J������`���̎w��p
    struct pk : public constraint_c {};

    template <class... ColIdLists>
    concept ForeignKeyArgsSpecifiable = requires {
        // �ΏۂƎQ�Ɛ�̓��
        requires sizeof...(ColIdLists) == 2;
        // ��^�p�����[�^�p�b�N�����R���e�i�ł���
        requires (mpl::has_value_parameters_v<ColIdLists> && ...);
        // �ΏۂƎQ�Ɛ�̃J�����̐��͓����łȂ���΂Ȃ�Ȃ�
        requires (mpl::count_v<ColIdLists> == ...);
        // ���ꂼ��AIdList�����l�̌^�͑S�ē����łȂ���΂Ȃ�Ȃ�
        requires (mpl::is_same_types_v<ColIdLists> && ...);
        // ���ꂼ��AIdList�����l�͏d���������Ă͂Ȃ�Ȃ�
        requires (mpl::is_unique_v<ColIdLists> && ...);
        // ��^�p�����[�^�͗񋓑̂łȂ���΂����Ȃ�(��L�ŁA�����^�ł���m�F���Ƃ��Ă��邽�߁A�擪�̂݌���΂悢)
        requires (mpl::Enumeration<mpl::get_front_t<ColIdLists>> && ...);
    };

    // �e�[�u����`���̎w��p
    template <class ColIdList, class RefColIdList>
    requires ForeignKeyArgsSpecifiable<ColIdList, RefColIdList>
    struct foreign_key : public constraint {};
    // �J������`���̎w��p
    template <mpl::Enumeration auto ColId> struct fk : public constraint_c {};

    template <class T>
    concept ColumnConstraintDefinable = std::is_base_of_v<constraint_c, T> && !std::is_same_v<constraint_c, T>;

    template <class T> struct is_constraint_definition
        : public std::bool_constant<std::is_base_of_v<constraint, T>> {};

    template <class T>
    concept ConstraintDefinable = is_constraint_definition<T>::value;

    template <class T>
    concept ConstraintListDefinable = requires {
        // T�̓p�����[�^�p�b�N�������Ă���
        requires mpl::has_type_parameters_v<T>;

        // ������`�����݂��Ă͂����Ȃ�
        requires mpl::is_unique_v<T>;

        // �e���v���[�g�������S�Đ����`�ł��邱��(�S�Ẵp�����[�^�������`�ł��邩�����X�g�Ƃ��Ď擾���A�_���ςŌ��ʂ��m�F)
        requires mpl::apply_list<
            mpl::quote<std::conjunction>,
            mpl::map_t<mpl::quote<is_constraint_definition>, T>
        >::value;

        // ������primary_key�����݂��Ă͂����Ȃ�
    };

    /**
     * @fn
     * @brief �J������`�p����ʏ�̃e�[�u����`�p�ɐ����
    */
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId> struct to_table_constraint;
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<pk, ColId> : public std::type_identity<primary_key<ColId>> {};
    template <mpl::Enumeration auto ColId, mpl::Enumeration auto ColId2>
    struct to_table_constraint<fk<ColId2>, ColId>
        : public std::type_identity<foreign_key<mpl::value_list<ColId>, mpl::value_list<ColId2>>> {};
    
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId>
    using to_table_constraint_t = to_table_constraint<Constraint, ColId>::type;

    /**
     * �e�[�u���Ƃ��Đ���𖢎w��ł��邱��
    */
    using constraint_unspecified = mpl::type_list<constraint>;
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
