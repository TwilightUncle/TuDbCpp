#include <gtest/gtest.h>
#include <utility.hpp>

TEST(tudbcpptest, GetFirstTypeTest)
{
    // ���X�g�̂����ŏ��̌^��Ԃ�
    constexpr auto case1 = std::is_same_v<tudb::get_first_type_t<int, void, double>, int>;
    constexpr auto case2 = std::is_same_v<tudb::get_first_type_t<void, double, int>, int>;
    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
}

// �e�X�g�p���^�֐���`(�󂯎������̈����̓��^�̃T�C�Y���傫���ق���Ԃ�)
template <class T1, class T2> struct get_max_size_type : std::conditional<(sizeof(T1) > sizeof(T2)), T1, T2> {};
template <class T1, class T2> struct get_min_size_type : std::conditional<(sizeof(T1) <= sizeof(T2)), T1, T2> {};

TEST(tudbcpptest, FoldByTypeTest)
{
    // ����ݍ���
    using l_max = tudb::foldl_by_type_t<get_max_size_type, long, long long, short, unsigned short, unsigned long long, unsigned long>;
    using l_min = tudb::foldl_by_type_t<get_min_size_type, long, long long, short, unsigned short, unsigned long long, unsigned long>;
    // �E��ݍ���
    using r_max = tudb::foldr_by_type_t<get_max_size_type, long, long long, short, unsigned short, unsigned long long, unsigned long>;
    using r_min = tudb::foldr_by_type_t<get_min_size_type, long, long long, short, unsigned short, unsigned long long, unsigned long>;

    constexpr auto case1 = std::is_same_v<l_max, unsigned long long>;
    constexpr auto case2 = std::is_same_v<l_min, short>;
    constexpr auto case3 = std::is_same_v<r_max, long long>;
    constexpr auto case4 = std::is_same_v<r_min, unsigned short>;

    ASSERT_TRUE(case1);
    ASSERT_TRUE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
}

// �ǂ̌^��n���Ă�int��Ԃ�
template <class T> struct to_int : std::type_identity<int> {};

TEST(tudbcpptest, MapTypesTest)
{
    // ���^�֐����S�Ă̌^�ɓK�p����Ă��邱��
    using maped = tudb::map_types_t<to_int, void, double, int, char>;
    constexpr auto case1 = std::is_same_v<maped, tudb::type_list<int, int, int, int>>;
    ASSERT_TRUE(case1);
}

template <int V, class T> struct test_same_value : std::bool_constant<V == T::value> {};

TEST(tudbcpptest, FindIfTypeTest)
{
    // ���^�֐����^��Ԃ����̂��܂܂�Ă����ꍇ�A�ŏ��ɐ^��Ԃ��^��ԋp���A�Ȃ����void��ԋp
    using find_type_int = tudb::find_if_by_type_t<int, std::is_same, double, long long, char, int>;
    using not_find_type1 = tudb::find_if_by_type_t<short, std::is_same, double, long long, char, int>;
    using find_type_1 = tudb::find_if_by_value_t<1, test_same_value, std::integral_constant<int, 0>, std::integral_constant<int, 1>, std::integral_constant<int, 2>>;
    using not_find_type2 = tudb::find_if_by_value_t<5, test_same_value, std::integral_constant<int, 0>, std::integral_constant<int, 1>, std::integral_constant<int, 2>>;

    constexpr auto case1 = std::is_same_v<find_type_int, int>;
    constexpr auto case2 = std::is_same_v<not_find_type1, void>;
    constexpr auto case3 = std::is_same_v<find_type_1, std::integral_constant<int, 1>>;
    constexpr auto case4 = std::is_same_v<not_find_type2, void>;

    ASSERT_TRUE(case1);
    ASSERT_TRUE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
}

TEST(tudbcpptest, EqualValuesTest)
{
    // �^�ƒl���Ƃ��ɓ������ꍇ�̂ݐ^�ƂȂ邱�Ƃ��m�F
    constexpr auto case1 = tudb::equal_values_v<(int)5, (int)5>;
    constexpr auto case2 = tudb::equal_values_v<(int)5, (int)4>;
    constexpr auto case3 = tudb::equal_values_v<(char)5, (long)5>;
    constexpr auto case4 = tudb::equal_values_v<(char)5, (long)4>;

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_FALSE(case3);
    ASSERT_FALSE(case4);
}

TEST(tudbcpptest, IncludeTest)
{
    // �����Ώۂ̃��X�g�Ɍ^�����������̂����݂���ꍇ�^
    constexpr auto case1 = tudb::include_type_v<int, double, void, int, long long>;
    constexpr auto case2 = tudb::include_type_v<int, double, void, long, long long>;
    // �����Ώۂ̃��X�g�Ɍ^�ƒl���Ƃ��ɓ��������̂����݂���ꍇ�^
    constexpr auto case3 = tudb::include_value_v<(int)1, (char)1, (long)2, (int)1, (long long)0>;
    constexpr auto case4 = tudb::include_value_v<(int)1, (char)1, (long)2, (unsigned int)1, (long long)0>;

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_TRUE(case3);
    ASSERT_FALSE(case4);
}

TEST(tudbcpptest, IsUniqueTest)
{
    // ��ł���v������̂��܂܂�Ă�����U
    constexpr auto case1 = tudb::is_unique_v<int, void, short, long>;
    constexpr auto case2 = tudb::is_unique_v<int, void, short, void>;
    constexpr auto case3 = tudb::is_unique_values_v<(int)1, (int)2, (short)1, (short)2>;
    constexpr auto case4 = tudb::is_unique_values_v<(int)1, (int)2, (short)1, (int)2>;

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_TRUE(case3);
    ASSERT_FALSE(case4);
}

TEST(tudbcpptest, IsSameTest)
{
    // ��ł��قȂ���̂��܂܂�Ă�����U
    constexpr auto case1 = tudb::is_same_types_v<int, int, int, int>;
    constexpr auto case2 = tudb::is_same_types_v<int, int, char, int>;
    // ��ł��^���قȂ���̂��܂܂�Ă�����U(�l�͈قȂ��Ă�OK)
    constexpr auto case3 = tudb::is_same_value_types_v<(int)1, (int)1, (int)1, (int)1>;
    constexpr auto case4 = tudb::is_same_value_types_v<(int)1, (int)1, (int)2, (int)1>;
    constexpr auto case5 = tudb::is_same_value_types_v<(int)1, (int)1, (char)1, (int)1>;

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
    ASSERT_FALSE(case5);
}

TEST(tudbcpptest, CountTest)
{
    // �����̐����J�E���g
    constexpr auto case1 = tudb::count_parameters_v<int>;
    constexpr auto case2 = tudb::count_parameters_v<int, void, double, long long>;
    constexpr auto case3 = tudb::count_value_parameters_v<1>;
    constexpr auto case4 = tudb::count_value_parameters_v<1, 'c', true, 4>;

    ASSERT_EQ(case1, 1);
    ASSERT_EQ(case2, 4);
    ASSERT_EQ(case3, 1);
    ASSERT_EQ(case4, 4);
}

TEST(tudbcpptest, GetDigitTest)
{
    constexpr auto case1 = tudb::get_digit<255>();
    constexpr auto case2 = tudb::get_digit<255, 2>();
    constexpr auto case3 = tudb::get_digit<255, 8>();
    constexpr auto case4 = tudb::get_digit<255, 16>();
    constexpr auto case5 = tudb::get_digit<-255>();
    constexpr auto case6 = tudb::get_digit<-255, 2>();
    constexpr auto case7 = tudb::get_digit<-255, 8>();
    constexpr auto case8 = tudb::get_digit<-255, 16>();
    constexpr auto case9 = tudb::get_digit<tudb::big_int<2>{~0ull, 80u}>();

    ASSERT_EQ(case1, 3);
    ASSERT_EQ(case2, 8);
    ASSERT_EQ(case3, 3);
    ASSERT_EQ(case4, 2);
    ASSERT_EQ(case5, 3);
    ASSERT_EQ(case6, 8);
    ASSERT_EQ(case7, 3);
    ASSERT_EQ(case8, 2);
    ASSERT_EQ(case9, 22);
}
