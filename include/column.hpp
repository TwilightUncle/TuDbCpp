///----------------------------------
/// @file column.hpp
/// @brief DB列定義
///----------------------------------
#pragma once
#include <constraint.hpp>
#include <type.hpp>

namespace tudb
{
    /**
     * @fn
     * @brief 列定義用メタ関数(同時に定義内容参照用クラス)
     * @tparam ColID スコープ付き列挙体要素を指定
     * @tparam Name cstrで包んだ文字列リテラルを渡す。文字列は1文字以上
     * @tparam FieldType 格納するデータ型
     * @tparam Constraints 列制約を任意数指定する(指定なしもOK)
    */
    template <
        enumeration auto ColID,
        cstr Name,
        class FieldType,
        ColumnConstraintDefinable... Constraints
    >
    requires (Name.size() > 0)
    struct define_column
    {
        static constexpr auto name = Name;
        static constexpr auto id = ColID;
        using id_type = decltype(ColID);
        using field_type = FieldType;
        using constraint_list = std::conditional_t<
            (sizeof...(Constraints) > 0),
            type_list<to_table_constraint_t<Constraints, ColID>...>,
            constraint_unspecified
        >;
    };

    /**
     * @fn
     * @brief column_definition型かどうかを判定するメタ関数
    */
    template <class T> struct is_column_definition : public std::false_type {};
    template <enumeration auto ColID, cstr Name, typename FieldType, ColumnConstraintDefinable... Constraints>
    struct is_column_definition<define_column<ColID, Name, FieldType, Constraints...>> : public std::true_type {};

    /**
     * @brief define_columnであることを強制する制約
    */
    template <class T>
    concept ColumnDefinable = is_column_definition<T>::value;

    template <ColumnDefinable T>
    struct get_column_id { static constexpr auto value = T::id; };

    template <ColumnDefinable T>
    struct get_column_name { static constexpr auto value = T::name; };

    /**
     * @fn
     * @brief define_columnで指定した制約を取り出す。ほかの任意の型を渡した場合は、constraint_unspecifiedを返す
    */
    template <class T> struct get_constraint_list : public std::type_identity<constraint_unspecified>{};
    template <class T>
    requires ColumnDefinable<T>
    struct get_constraint_list<T> : public std::type_identity<typename T::constraint_list> {};

    template <class T> using get_constraint_list_t = get_constraint_list<T>::type;

    /**
     * カラム定義リストであること
    */
    template <class T>
    concept ColumnListDefinitionable = requires {
        // Tはパラメータパックを持っている
        requires has_type_parameters_v<T>;

        // 同じ定義のカラムが存在してはいけない
        requires copy_types_t<T, is_unique>::value;

        // テンプレート引数が全てカラム定義であること(全てのパラメータがカラム定義であるかをリストとして取得し、論理積で結果を確認)
        requires copy_types_t<
            map_types_t<is_column_definition, pass_types<T>>,
            std::conjunction
        >::value;

        // nameがかぶってはいけない
        // idがかぶってはいけない
        // idの型は全て等しい必要がある
    };

    /**
     * @fn
     * @brief カラムの定義リストから、各カラムに指定された制約情報を抽出する(一つもなければtype_list<ignore_type>が返る)
    */
    template <ColumnListDefinitionable T>
    struct extract_constraints : public remove_if_by_type<
        constraint,
        std::is_same,
        pass_types<copy_types_t<
            map_types_t<get_constraint_list, pass_types<T>>,
            concat_type_list_t
        >>
    > {};

    template <ColumnListDefinitionable T> using extract_constraints_t = extract_constraints<T>::type;
}
