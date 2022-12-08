#include <gtest/gtest.h>
#include <utility.hpp>


using namespace std::string_view_literals;
TEST(tudbcpptest, RegexTest)
{
    using test_type = tudb::regex<"abcdef">;
    constexpr auto empty_inst = tudb::empty_regex{};
    // // �ǂ̕��@���g�p���邩�w�肪�Ȃ������ꍇ�AECMAScript�̃r�b�g���܂܂�Ă��邩�m�F
    // constexpr auto inst1 = tudb::regex{"abcde", std::regex_constants::syntax_option_type::icase};

    // EXPECT_EQ(inst1.syntax, std::regex_constants::syntax_option_type::ECMAScript | std::regex_constants::syntax_option_type::icase);
    constexpr auto case1 = tudb::is_collect_regex_back_slash("a\\0\\\\a\\\\");
    constexpr auto case2 = tudb::is_collect_regex_back_slash("a\\0\\\\a\\");
    constexpr auto case3 = tudb::is_collect_regex_back_slash("\\a\\0\\\\a\\\\");
    constexpr auto case4 = tudb::is_collect_regex_back_slash("a\\0\\\\\\a\\\\");
    constexpr auto case5 = tudb::is_allowed_string("aaaab", true, "ab", "");
    constexpr auto case6 = tudb::is_allowed_string("\\0\\n\\\\", true, "", "0n\\");
    constexpr auto case7 = tudb::is_allowed_string("a\\0\\\\a\\\\", true, "a", "\\0");
    constexpr auto case8 = tudb::is_allowed_string("a\\0\\\\a\\\\", false, "bc", "ds");
    constexpr auto case9 = tudb::is_allowed_string("ab\\0\\\\a\\\\", true, "a", "\\0");
    constexpr auto case10 = tudb::is_allowed_string("a\\d\\0\\\\a\\\\", false, "bc", "ds");
    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_FALSE(case3);
    ASSERT_FALSE(case4);
    ASSERT_TRUE(case5);
    ASSERT_TRUE(case6);
    ASSERT_TRUE(case7);
    ASSERT_TRUE(case8);
    ASSERT_FALSE(case9);
    ASSERT_FALSE(case10);

    // ����
    constexpr auto case11 = tudb::regex_brancket_inner<"abc[defg{hijk}lm\\](opq)]rs", 3>::value;
    constexpr auto case12 = tudb::regex_brancket_inner<"abc{defg\\{hijk\\}lm](opq)}rs", 3>::value;
    constexpr auto case13 = tudb::regex_brancket_inner<"abc(defg{hijk\\}lm](opq\\))rs", 3>::value;
    constexpr auto case14 = tudb::regex_brancket_inner<"abc<defg{hijk\\}lm](opq\\)>rs", 3>::value;
    EXPECT_STREQ(case11.data(), "defg{hijk}lm\\](opq)");
    EXPECT_STREQ(case12.data(), "defg\\{hijk\\}lm](opq)");
    EXPECT_STREQ(case13.data(), "defg{hijk\\}lm](opq\\)");
    EXPECT_STREQ(case14.data(), "defg{hijk\\}lm](opq\\)");

    // �ُ�(�Ή���������ʂ����݂��Ȃ�) �R�����g�A�E�g���O���ƃR���p�C���G���[������
    // constexpr auto case15 = tudb::regex_brancket_inner<"abc[defg{hijk}lm\\](opq)\\]rs", 3>::value;
    // constexpr auto case16 = tudb::regex_brancket_inner<"abc{defg\\{hijk\\}lm](opq)\\}rs", 3>::value;
    // constexpr auto case17 = tudb::regex_brancket_inner<"abc(defg{hijk\\}lm](opq\\)\\)rs", 3>::value;
    // constexpr auto case18 = tudb::regex_brancket_inner<"abc<defg{hijk\\}lm](opq\\)\\>rs", 3>::value;

    // �����N���X,�o�b�N�X���b�V��,�����͈͎w��Ȃ�
    constexpr auto case19 = tudb::get_regex_char_range_matcher<"-abc.">()(".c-aba");
    constexpr auto case20 = tudb::get_regex_char_range_matcher<"abc.-">()(".c-aba");
    constexpr auto case21 = tudb::get_regex_char_range_matcher<"-abc.">()("b.c-d");
    constexpr auto case22 = tudb::get_regex_char_range_matcher<"-abc.">()("def");
    constexpr auto case23 = tudb::get_regex_char_range_matcher<"^-abc">()("cab");
    constexpr auto case24 = tudb::get_regex_char_range_matcher<"^-abc">()("bcd");
    constexpr auto case25 = tudb::get_regex_char_range_matcher<"^-abc">()("def^");
    EXPECT_TRUE(case19);
    EXPECT_TRUE(case20);
    EXPECT_FALSE(case21);
    EXPECT_FALSE(case22);
    EXPECT_FALSE(case23);
    EXPECT_FALSE(case24);
    EXPECT_TRUE(case25);

    // �����N���X�w�肠��
    constexpr auto case26 = tudb::get_regex_char_range_matcher<"\\d">()("0123456789");
    constexpr auto case27 = tudb::get_regex_char_range_matcher<"\\d">()("0123456789a");
    constexpr auto case28 = tudb::get_regex_char_range_matcher<"^\\d">()("0aabc_%&\t\\[");
    constexpr auto case29 = tudb::get_regex_char_range_matcher<"^\\d">()("abc_%&\t\\[");
    constexpr auto case30 = tudb::get_regex_char_range_matcher<"\\D">()("0aabc_%&\t\\[");
    constexpr auto case31 = tudb::get_regex_char_range_matcher<"\\D">()("aabc_%&\t\\[");
    constexpr auto case32 = tudb::get_regex_char_range_matcher<"^\\D">()("0123456789");
    constexpr auto case33 = tudb::get_regex_char_range_matcher<"^\\D">()("0123456789a");

    // ���������֐���ێ��ł��邩���m�F(�����W�̃e�X�g)
    constexpr auto range_matcher1 = tudb::get_regex_char_range_matcher<"0-5a-dA\\-E\\t">();

    EXPECT_TRUE(case26);
    EXPECT_FALSE(case27);
    EXPECT_FALSE(case28);
    EXPECT_TRUE(case29);
    EXPECT_FALSE(case30);
    EXPECT_TRUE(case31);
    EXPECT_TRUE(case32);
    EXPECT_FALSE(case33);
    EXPECT_TRUE(range_matcher1("0123451bcdA-E\t"));
    EXPECT_FALSE(range_matcher1("0123451bcdA-Et"));
    EXPECT_FALSE(range_matcher1("0123451bcdABCDE"));
}
