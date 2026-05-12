//****************************************************************************
// 
// Syntax Highlighting Plugin for Open Salamander
//
// Author: Michal Mores
//
// Email: xmoresm00@stud.fit.vut.cz
//
//****************************************************************************

#pragma once
#include <unordered_map>

#include "parse.h"
#include "grammar.h"
#include "scope.h"
#include "windows.h" // Todo pri debug build nepotrebne 

//TODOOOO TEMP REMOVE TO OBEJCT!!
typedef struct text_point {
    long x;
    long y;
} text_point;

typedef struct Token
{
    WCHAR* start_ptr;
    size_t token_len;
    COLORREF fg_color;
    Token() = default;
    Token(WCHAR* start, size_t len, COLORREF color)
        : start_ptr(start), token_len(len), fg_color(color) {}
};

typedef struct Line
{
    uint32_t start_index;
    uint32_t end_index;
    std::vector<Token> tokens;
    Line() = default;
    Line(uint32_t start, uint32_t end)
        : start_index(start), end_index(end) {}
};

typedef struct Document
{
    std::vector<Line> lines;
};

typedef struct Block
{
    int first_line;
	int last_line;
	parse::stack_ptr parser_state;
	std::map<size_t, scope::scope_t> scopes;
    Block() = default;
	Block(int first, int last)
		: first_line(first), last_line(last), parser_state(NULL), scopes() {
	}
};

class TextModel
{
public:
    TextModel(const std::string& path, UINT default_encoding);
	~TextModel();

    std::vector<Token> get_line(int n);
    size_t get_line_count();
    COLORREF get_bg();
    COLORREF get_fg();
    bool update_theme();
    bool process_selection(WCHAR** string, int* len, text_point start, text_point end);
	WCHAR* get_content() const { return source; };
    long get_source_size() const { return source_size; };
    text_point offset_to_xy(int offset);
    int line_to_offset(int line);
    bool data_to_screen_pos(text_point& start, text_point& end);
    int get_line_length(int line);
    int get_longest_line_length() const { return longest_line_length; };
    void set_longest_line_length(int length) { longest_line_length = length; };
    bool reparse_occured(int first_line, int last_line);
	std::string get_active_grammar() const { return grammar_name; };
	bool update_grammar(const std::string& grammar_name);
	bool encoding_forced() const { return forced_utf; };

private:
    char* data;
    WCHAR* source;
    long source_size = 0;
	int longest_line_length = 0;

    parse::grammar_ptr gm = NULL;
    theme_ptr theme = NULL;
    parse::stack_ptr parser_state = NULL;
    std::string theme_name;
    std::string grammar_name;

    std::vector<uint32_t> line_positions;
    std::vector<Block> blocks;
    Document text;

    bool forced_utf = false;

    bool parsing_corrected = false;

    void load_file(const std::string& path, UINT default_encoding);
	void load_grammar(const std::string& grammar);
    UINT get_encoding(unsigned char* bom, long length, UINT default_encoding);

    void parse_block(int block_idx);
    void process_line(int n, std::map<size_t, scope::scope_t> scopes);
    bool scopes_equal(std::map<size_t, scope::scope_t> scopes1, std::map<size_t, scope::scope_t> scopes2);
    Document raw_doc();
    std::vector<int> calculate_display_positions(int line_idx, int tab_size);

    COLORREF to_rgb(color_info_t color);
    COLORREF bg_convert(color_info_t color);
    COLORREF fg_convert(color_info_t color);

    // debugging
    void tokens_to_file(std::wstring wstr, std::map<size_t, scope::scope_t>& scopes);
};
