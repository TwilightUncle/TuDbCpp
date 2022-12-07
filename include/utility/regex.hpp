///----------------------------------
/// @file regex.hpp
/// @brief 定数式に置ける正規表現(ユニコード未対応)
///----------------------------------
#pragma once
#include <regex>

namespace tudb
{

    enum {
        // 通常範囲に置ける特殊文字の性質
        REGEX_SPECIAL       = 0x0001u,  // 特殊文字(バックスラッシュでエスケープ)
        REGEX_BK_SPECIAL     = 0x0002u,  // 特殊文字(バックスラッシュを付与した場合)
        REGEX_BRANCKET_BE   = 0x0004u,  // 括弧始まり
        REGEX_BRANCKET_EN   = 0x0008u,  // 括弧終わり
        REGEX_PREV_REF      = 0x0010u,  // 前の文字を参照する
        REGEX_NEXT_REF      = 0x0020u,  // 次の文字を参照する
        REGEX_BEGIN         = 0x0040u,  // 先頭にしか指定できない
        REGEX_END           = 0x0080u,  // 末尾にしか指定できない

        // []内における特殊文字の性質
        // REGEX_CLASSES_SPECIAL以外にビットがったていなければバックスラッシュでエスケープが必要、
        // REGEX_CLASSES_SPECIAL以外にビットが立っている場合、該当する全てを満たしていない場合では特殊文字として認識してはいけない
        REGEX_CLASSES_SPECIAL       = 0x0100u,  // 特殊文字([]内における)
        REGEX_CLASSES_BK_SPECIAL    = 0x0200u,
        REGEX_CLASSES_PREV_REF      = 0x0400u,
        REGEX_CLASSES_NEXT_REF      = 0x0800u,
        REGEX_CLASSES_BEGIN         = 0x1000u,  // 先頭
        REGEX_CLASSES_END           = 0x2000u,

        // 特殊文字かどうかの判定用マスク
        REGEX_SPECIAL_MASK = REGEX_SPECIAL | REGEX_BK_SPECIAL | REGEX_CLASSES_SPECIAL | REGEX_CLASSES_BK_SPECIAL,
    };

    /**
     * @fn
     * @brief 指定した文字の正規表現としての文法的な性質を示す属性のビット列を取得する
     * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions/Cheatsheet
    */
    inline constexpr auto get_regex_character_attribute(char ch)
    {
        std::uint32_t attrs[(std::numeric_limits<char>::max)() + 1] = {};
        // Character classes
        attrs['['] |= REGEX_SPECIAL | REGEX_BRANCKET_BE | REGEX_CLASSES_SPECIAL;
        attrs[']'] |= REGEX_SPECIAL | REGEX_BRANCKET_EN | REGEX_CLASSES_SPECIAL;
        attrs['-'] |= REGEX_CLASSES_SPECIAL | REGEX_CLASSES_PREV_REF | REGEX_CLASSES_NEXT_REF;
        attrs['^'] |= REGEX_CLASSES_SPECIAL | REGEX_CLASSES_BEGIN;
        attrs['.'] |= REGEX_SPECIAL;
        attrs['d'] |= REGEX_BK_SPECIAL;
        attrs['D'] |= REGEX_BK_SPECIAL;
        attrs['w'] |= REGEX_BK_SPECIAL;
        attrs['W'] |= REGEX_BK_SPECIAL;
        attrs['s'] |= REGEX_BK_SPECIAL;
        attrs['S'] |= REGEX_BK_SPECIAL;
        attrs['t'] |= REGEX_BK_SPECIAL;
        attrs['r'] |= REGEX_BK_SPECIAL;
        attrs['n'] |= REGEX_BK_SPECIAL;
        attrs['v'] |= REGEX_BK_SPECIAL;
        attrs['f'] |= REGEX_BK_SPECIAL;
        attrs['b'] |= REGEX_CLASSES_BK_SPECIAL;
        attrs['0'] |= REGEX_BK_SPECIAL;
        attrs['c'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['x'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['u'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['\\'] |= REGEX_SPECIAL | REGEX_NEXT_REF | REGEX_CLASSES_SPECIAL | REGEX_CLASSES_NEXT_REF;
        attrs['|'] |= REGEX_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        // Assertions
        attrs['^'] |= REGEX_SPECIAL | REGEX_BEGIN;
        attrs['$'] |= REGEX_SPECIAL | REGEX_END;
        attrs['b'] |= REGEX_BK_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        attrs['B'] |= REGEX_BK_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        attrs['?'] |= REGEX_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        // Groups and backreferences
        attrs['('] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        attrs[')'] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        for (const auto c : std::views::iota('1', '9'))
            attrs[c] |= REGEX_BK_SPECIAL;
        attrs['k'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        // Quantifiers
        attrs['*'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['+'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['?'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['{'] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        attrs['}'] |= REGEX_SPECIAL | REGEX_BRANCKET_EN;
        // Unicode property escapes
        attrs['p'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['P'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;

        return attrs[ch];
    }

    /**
     * @fn
     * @brief 正しくない書き方がされているバックスラッシュが存在する場合偽が返る
    */
    inline constexpr bool is_collect_regex_back_slash(const std::string_view& target)
    {
        for (int i = 0; i < target.size(); i++)
            if (target[i] == '\\' && (i == target.size() - 1 || !get_regex_character_attribute(target[++i])))
                return false;
        return true;
    }

    /**
     * @fn
     * @brief 許可リストまたは拒否リストとして指定したchar_listを参照し、targetが有効か検証する。バックスラッシュを含む場合、二つで一文字と認識する
     * @param target 検証する文字列
     * @param allow_or_deny allowの場合trueを指定、denyの場合falseを指定
     * @param char_list 許可または拒否文字のリスト。allow_or_denyによって拒否リストか許可リストか制御する
    */
    inline constexpr bool is_allowed_string(
        const std::string& target,
        bool allow_or_deny,
        const std::string& char_list,
        const std::string& bk_char_list,
        bool is_bk_escape = true
    ) {
        const auto collate_list = [&allow_or_deny](const std::string& list, char ch)->bool {
            // 許可リストの中から未発見のものがあった場合または、
            // 拒否リストの中から発見されたものがあった場合、失敗
            return (list.find_first_of(ch) == std::string::npos) == allow_or_deny;
        };

        for (int i = 0; i < target.size(); i++) {
            if ((target[i] != '\\' || !is_bk_escape) && collate_list(char_list, target[i])) return false;
            else if (target[i] == '\\' && i < target.size() - 1 && collate_list(bk_char_list, target[++i])) return false;
        }
        return true;
    }

    /**
     * @fn
     * @brief indexの位置の一つ前の文字が指定の文字か確認する
    */
    inline constexpr bool eq_before_char(const std::string_view& target, int index, char ch)
    {
        return index > 0 && target[(std::max)(index - 1, 0)] == ch;
    }

    /**
     * @fn
     * @brief 括弧の終了インデックスと対応エラーを取得する
    */
    inline constexpr auto get_regex_brancket_index(const std::string_view& target, const int brancket_begin_pos)
    {
        const auto brancket_begin = target[brancket_begin_pos];

        // 対応するとじ括弧と、エラー発生時の該当エラーの組を取得
        const auto [brancket_end, error_value] = [](char ch)->std::pair<char, std::regex_constants::error_type> {
            switch (ch) {
                case '[': return {']', std::regex_constants::error_brack};
                case '(': return {')', std::regex_constants::error_paren};
                case '{': return {'}', std::regex_constants::error_brace};
                case '<': return {'>', std::regex_constants::error_collate};
            }
            return {(char)-1, std::regex_constants::error_type{}};
        }(brancket_begin);

        // 引数の指定が不正
        assert(brancket_end >= 0);

        const auto brancket_str = target.substr(brancket_begin_pos + 1);
        auto brancket_end_pos = std::string_view::npos;
        for (const auto i : std::views::iota((std::size_t)0, brancket_str.size())) {
            // とじ括弧かつエスケープされていない場合、位置を記録し終了
            if (brancket_str[i] == brancket_end && !eq_before_char(brancket_str, i, '\\')) {
                // substrする前のtargetの位置基準
                brancket_end_pos = i + brancket_begin_pos + 1;
                break;
            }
        }
        return std::tuple{brancket_end_pos, error_value};
    }

    /**
     * @fn
     * @brief 括弧開始の文字インデックスを渡すことで括弧内の文字列、対応するとじ括弧のインデックス、エラーがある場合はエラーの組を返す
    */
    inline constexpr auto extract_regex_brancket_inner(const std::string_view& pattern_view, const int brancket_begin_pos)
    {
        const auto [brancket_end_pos, error_value] = get_regex_brancket_index(pattern_view, brancket_begin_pos);

        if (brancket_end_pos == std::string_view::npos || brancket_end_pos - brancket_begin_pos <= 1)
            return std::tuple{
                std::string_view{},
                brancket_end_pos,
                std::optional{error_value}
            };

        // 抽出範囲に括弧は含まない
        const auto brancket_inner = pattern_view.substr(brancket_begin_pos + 1, brancket_end_pos - brancket_begin_pos - 1);
        return std::tuple{
            brancket_inner,
            brancket_end_pos,
            std::optional<std::regex_constants::error_type>{}
        };
    }

    struct regex_char_class
    {
    private:
        static constexpr auto digits = cstr{"0123456789"};
        static constexpr auto words = cstr{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"};
        static constexpr auto space = cstr{"\f\t\v"};
        static constexpr auto strends = cstr{"\n\r"};
        static constexpr auto bk_space = cstr{"ftv"};
        static constexpr auto bk_strends = cstr{"nr"};
        static constexpr auto others = cstr{"-!\"#$%&'()=^~|`@{}+;*:,<.>/?\b\\"};
        static constexpr auto bk_others = cstr{"b0[]\\"};

        static constexpr auto true_words = concat(digits, words);
        static constexpr auto true_space = concat(space, strends);
        static constexpr auto bk_true_space = concat(bk_space, bk_strends);
        static constexpr auto dot = concat(digits, words, space, others);
        static constexpr auto bk_dot = concat(bk_space, bk_others);
        static constexpr auto anti_digits = concat(words, space, strends, others);
        static constexpr auto bk_anti_digits = concat(bk_space, bk_strends, bk_others);
        static constexpr auto anti_words = concat(space, strends, others);
        static constexpr auto bk_anti_words = concat(bk_space, bk_others, bk_others);
        static constexpr auto anti_space = concat(digits, words, others);

    public:
        // 特殊文字に該当する文字集合を取得する
        inline static constexpr auto get_char_set(const char ch)
        {
            switch (ch) {
                case 'd': return std::pair{digits.view(), std::string_view{}};
                case 'w': return std::pair{true_words.view(), std::string_view{}};
                case 's': return std::pair{true_space.view(), bk_true_space.view()};
                case '.': return std::pair{dot.view(), bk_dot.view()};
                case 'D': return std::pair{anti_digits.view(), bk_anti_digits.view()};
                case 'W': return std::pair{anti_words.view(), bk_anti_words.view()};
                case 'S': return std::pair{anti_space.view(), bk_others.view()};
            }
            return std::pair{std::string_view{}, std::string_view{}};
        }

        template <char C>
        static constexpr auto get_const_char_set()
        {
            if constexpr (C == 'd') return digits;
            else if constexpr (C == 'w') return true_words;
            else if constexpr (C == 's') return true_space;
            else if constexpr (C == '.') return dot;
            else if constexpr (C == 'D') return anti_digits;
            else if constexpr (C == 'W') return anti_words;
            else if constexpr (C == 'S') return anti_space;
            else if constexpr (C == 'f') return cstr{"\f"};
            else if constexpr (C == 't') return cstr{"\t"};
            else if constexpr (C == 'v') return cstr{"\v"};
            else if constexpr (C == 'n') return cstr{"\n"};
            else if constexpr (C == 'r') return cstr{"\r"};
            else if constexpr (C == 'b') return cstr{"\b"};
            else return cstr{{C, '\0'}};
        }

        template <char C>
        static constexpr auto get_const_bk_char_set()
        {
            if constexpr (C == 'd') return cstr{""};
            else if constexpr (C == 'w') return cstr{""};
            else if constexpr (C == 's') return bk_true_space;
            else if constexpr (C == '.') return bk_dot;
            else if constexpr (C == 'D') return bk_anti_digits;
            else if constexpr (C == 'W') return bk_anti_words;
            else if constexpr (C == 'S') return bk_others;
            else return cstr{{C, '\0'}};
        }
    };

    template <cstr CharRange>
    requires (CharRange.size() > 0)
    struct regex_range_parser
    {
        static_assert(is_collect_regex_back_slash(CharRange.view()));

        static constexpr auto allow_or_deny = CharRange[0] != '^';
        static constexpr auto begin_index = int(!allow_or_deny);
        static constexpr auto size = CharRange.size();

        /**
         * @fn
         * @brief 文字集合を構築する
        */
        template <int N>
        static constexpr auto make_regex_char_list()
        {
            if constexpr (N >= size) return cstr{""};
            else return concat(substr<N, 1, CharRange.max_size + 1>(CharRange), make_regex_char_list<N + 1>());
        }
        template <int N>
        requires (CharRange[N] == '-')
        static constexpr auto make_regex_char_list()
        {
            if constexpr (begin_index == N || size - 1 == N)
                return concat(cstr{"-"}, make_regex_char_list<N + 1>());
            else {
                constexpr char begin_char = (std::min)(CharRange[N - 1], CharRange[N + 1]) + 1;
                constexpr char end_char = (std::max)(CharRange[N - 1], CharRange[N + 1]);
                cstr<2 + end_char - begin_char> str{};
                for (char c = 0; c < str.max_size; c++) str[c] = c + begin_char;
                return concat(str, make_regex_char_list<N + 2>());
            }
        }
        template <int N>
        requires (CharRange[N] == '\\')
        static constexpr auto make_regex_char_list()
        {
            // 文字範囲の括弧内では「.」は特殊文字ではない
            if constexpr (CharRange[N + 1] != '.')
                return concat(regex_char_class::get_const_char_set<CharRange[N + 1]>(), make_regex_char_list<N + 2>());
            else return concat(cstr{"."}, make_regex_char_list<N + 2>());
        }

        /**
         * @fn
         * @brief バックスラッシュとセットの場合の文字集合を構築する
        */
        template <int N>
        static constexpr auto make_regex_bk_char_list()
        {
            if constexpr (N >= size) return cstr{""};
            else if constexpr (CharRange[N] == '\\' && CharRange[N + 1] != '.')
                return concat(regex_char_class::get_const_bk_char_set<CharRange[N + 1]>(), make_regex_bk_char_list<N + 2>());
            else return make_regex_bk_char_list<N + 1>();
        }

        static constexpr auto value = make_regex_char_list<begin_index>();
        static constexpr auto bk_value = make_regex_bk_char_list<begin_index>();
    };

    /**
     * @fn
     * @brief 渡された文字範囲を許可/拒否リストに展開し、与えられた文字列がマッチするか判定する関数オブジェクトを返却する
    */
    template <cstr Target>
    constexpr auto get_regex_char_range_matcher()
    {
        return [](const std::string& comp) -> bool {
            using range_parser = regex_range_parser<Target>;
            return is_allowed_string(
                comp,
                range_parser::allow_or_deny,
                range_parser::value.data(),
                range_parser::bk_value.data(),
                false
            );
        };
    }

    /**
     * @class
     * @brief 正規表現格納オブジェクト。動的に生成されたパターンについては考慮しない
    */
    template <cstr Pattern>
    struct regex
    {
        static_assert(is_collect_regex_back_slash(Pattern.view()));

        using flag_type = std::regex_constants::syntax_option_type;

        /**
         * @fn
         * @brief 括弧内の正規表現検証
        */
        static constexpr std::optional<std::regex_constants::error_type> parse_brancket_inner(const std::string_view& brancket_inner, const char brancket_be)
        {
            switch (brancket_be) {
                case '[':
                    // 再帰的な括弧は存在しないため、拒否リストに開始括弧が含まれていたらアウト
                    if (!is_allowed_string(std::string(brancket_inner), false, "[", "")) return std::regex_constants::error_brack;
                    break;
                case '(':
                    // キャプチャリストは再帰的に
                    if (const auto error = parse_pattern(brancket_inner)) return error;
                    break;
                case '{':
                    // 数量詞とユニコードの指定のみのため、許可リストで確認
                    if (!is_allowed_string(std::string(brancket_inner), true, ",0123456789abcdef", "")) return std::regex_constants::error_brack;
                    break;
            }
            return std::nullopt;
        }

        /**
         * @fn
         * @brief 正規表現の文法の検証
        */
        static constexpr std::optional<std::regex_constants::error_type> parse_pattern(const std::string_view& pattern_view)
        {
            // バックスラッシュの検証
            if (is_collect_regex_back_slash(pattern_view)) return std::regex_constants::error_escape;

            // 一文字ずつ検証
            for (
                auto [i, ch] = std::pair{0, pattern_view[0]};
                i < pattern_view.size();
                ch = pattern_view[++i]
            ) {
                const auto attr = get_regex_character_attribute(ch);
                // 特殊文字ではない
                if (!(attr & REGEX_SPECIAL_MASK)) continue;

                // 特殊文字の前にバックスラッシュありかの判定
                const auto is_before_bk = eq_before_char(pattern_view, i, '//');
                if ((attr & (REGEX_BK_SPECIAL | REGEX_CLASSES_BK_SPECIAL) && !is_before_bk)) continue;
                if ((attr & (REGEX_SPECIAL | REGEX_CLASSES_SPECIAL)) && is_before_bk) continue;

                // とじ括弧がいきなり現れるのはおかしいためエラー
                if (attr & REGEX_BRANCKET_EN) {
                    switch (ch) {
                        case ']': return std::regex_constants::error_brack;
                        case ')': return std::regex_constants::error_paren;
                        case '}': return std::regex_constants::error_brace;
                    }
                }

                // 括弧を検出
                if (attr & REGEX_BRANCKET_BE) {
                    const auto [inner, en_pos, br_error] = extract_regex_brancket_inner(pattern_view, i);
                    if (br_error) return br_error;
                    // 括弧内部の検証は別関数に委譲のため、とじ括弧までインデックスを進める
                    if (const auto inner_error = parse_brancket_inner(inner, ch)) return inner_error;
                    i = en_pos;
                    continue;
                }
            }
            return std::nullopt;
        }

        // consteval regex()
        // {
        //     // 正規表現のフラグ,文法チェック
        //     // assert(validate_syntax_option(syntax));
        //     static_assert(is_collect_regex_back_slash(Pattern.view()));
        //     // assert(validate_regex_syntax(pattern.view(), syntax));
        // };
        
        /**
         * @fn
         * @brief syntaxを指定するビットのみ抜き出す
        */
        static constexpr flag_type get_syntax_bits(flag_type syntax)
        {
            constexpr auto syntax_mask = std::regex_constants::ECMAScript
                | std::regex_constants::basic
                | std::regex_constants::extended
                | std::regex_constants::awk
                | std::regex_constants::grep
                | std::regex_constants::egrep;
            return syntax & syntax_mask;
        }

        /**
         * @fn
         * @brief syntaxに一つも指定されていない場合、ECMAScriptのビットを立てる
        */
        static constexpr flag_type shap_syntax_option(flag_type syntax)
        {
            if (!get_syntax_bits(syntax))
                syntax |= flag_type::ECMAScript;
            return syntax;
        }
    
        /**
         * @fn
         * @brief syntaxの検証
        */
        static constexpr bool validate_syntax_option(flag_type syntax)
        {
            // return std::popcount((std::uint64_t)get_syntax_bits(syntax)) <= 1;
            // 現在ECMAScriptバージョンしか実装していないので、ほかの文法が指定されていたらNGとする
            return get_syntax_bits(shap_syntax_option(syntax)) == flag_type::ECMAScript;
        }
    };

    using empty_regex = regex<"">;
}
