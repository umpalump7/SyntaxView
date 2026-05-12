//****************************************************************************
// 
// Syntax Highlighting Plugin for Open Salamander
//
// Author: Michal Mores
//
// Email: xmoresm00@stud.fit.vut.cz
//
//****************************************************************************

#include "precomp.h"


#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>

#include "theme.h"
#include "pattern.h"
#include "reader.h"
#include "textmodel.h"
#include "helper.h"
#include "syntaxview.h"

#define ENC_UTF8_BOM 511
#define ENC_UTF16_LE 512
#define ENC_UTF16_BE 513

using namespace parse;

TextModel::TextModel(const std::string& path, UINT default_encoding)
{
    load_file(path, default_encoding);

    this->theme_name = CfgThemeFileName;

    std::string dll_path = getCurrentDllPath();

	load_grammar(getGrammarChoice(path));


    Json::Value root = loadJson(dll_path + "\\themes\\" + theme_name);
    if (!root.isNull())
    {
        theme = parse_theme_mod(root, theme_name);
    }
    
    // Find line positions
    line_positions.push_back(0);
    int longest_line_idx = 0;
    for (long i = 0; i < source_size - 1; i++)
    {
        if (source[i] == L'\n')
        {
            if (i > 0 && source[i - 1] == L'\r') {
                source[i - 1] = L'\n';
                source[i] = L'\r';
            }
            line_positions.push_back(static_cast<uint32_t>(i + 1));
			if (i - line_positions[line_positions.size() - 2] > longest_line_length)
			{
				longest_line_length = i - line_positions[line_positions.size() - 2];
				longest_line_idx = line_positions.size() - 2;
			}
        }
    }

	//blocks.reserve(line_positions.size() / 100 + 1);
	for (int i = 0; i <= line_positions.size()/100; i++)
	{
		blocks.push_back(Block(i * 100, i == line_positions.size()/100 ? line_positions.size()-1 : (i+1) * 100 - 1));
	}
    
    text = raw_doc();   // todo change to default viewer
    if (gm != NULL && theme != NULL) {
		parse_block(0);
    }
    longest_line_length = get_line_length(longest_line_idx);
}

void TextModel::load_grammar(const std::string& grammar)
{
    std::string dll_path = getCurrentDllPath();
    if (grammar != "") {   // todo change to default viewer
        Json::Value gm_json = loadJson(dll_path + "\\grammars\\" + grammar);
        if (!gm_json.isNull())
        {
            gm = parse_grammar(gm_json);
            if (ShowGrammarParsingErrors && PatternErrors != "")
            {
				MessageBox(NULL, PatternErrors.c_str(), "Pattern compilation errors", MB_ICONERROR);
            }
            grammar_name = grammar;
        }
        else
        {
            grammar_name = "";
        }
    }
    for (int i = 0; i < blocks.size(); i++)
    {
        blocks[i].parser_state = NULL;
    }
}

void TextModel::load_file(const std::string& path, UINT default_encoding)
{
    long size;
    // Open the file
    FILE* f = fopen(path.c_str(), "rb");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        rewind(f);

        data = new char[2 * size + 2];
        fread(data + size, 1, size, f);
        data[2 * size] = '\0'; // Null-terminate
        data[2 * size + 1] = '\0'; // Null-terminate
        fclose(f);
    }
    else
    {
        data = new char[2];
        data[0] = '\0'; // fallback empty
        data[1] = '\0'; // fallback empty
        size = 0;
    }

    UINT encoding = get_encoding((unsigned char*)data + size, size, default_encoding);

    if (encoding != default_encoding)
    {
        forced_utf = true;
    }

    if (encoding == ENC_UTF16_LE)
    {
        source = (WCHAR*)(data + size + 2);
        source_size = size / 2 - 1;
    }
    else if (encoding == ENC_UTF16_BE)
    {
        source = (WCHAR*)(data + size + 2);
        source_size = size / 2 - 1;
        for (long i = 0; i < source_size; i++)
        {
            WCHAR c = source[i];
            source[i] = ((c >> 8) & 0xFF) | ((c & 0xFF) << 8); // Swap bytes
        }
    }
    else
    {
        char* utf8_data = data + size + (encoding == ENC_UTF8_BOM ? 3 : 0); // Skip BOM if present
        long utf8_size = size - (encoding == ENC_UTF8_BOM ? 3 : 0);

        if (encoding == ENC_UTF8_BOM)
        {
            encoding = CP_UTF8;
        }
        // Get required buffer size
        int size_needed = MultiByteToWideChar(
            encoding,
            0,
            utf8_data,
            utf8_size,
            NULL,
            0
        );

        this->source = (WCHAR*)data;
        this->source_size = size_needed;


        // Perform conversion
        MultiByteToWideChar(
            encoding,
            0,
            utf8_data,
            utf8_size,
            this->source,
            size_needed
        );
    }
}

UINT TextModel::get_encoding(unsigned char* bom, long length, UINT default_encoding)
{
    if (length >= 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)
        return ENC_UTF8_BOM;
    else if (length >= 2 && bom[0] == 0xFF && bom[1] == 0xFE)
	    return ENC_UTF16_LE;
    else if (length >= 2 && bom[0] == 0xFE && bom[1] == 0xFF)
        return ENC_UTF16_BE;
    else
		return default_encoding;
}

TextModel::~TextModel()
{
    if (data != NULL)
    {
        free(data);
        data = NULL;
    }
    source = NULL;
	source_size = 0;
}

bool TextModel::update_grammar(const std::string& new_grammar)
{
    if (new_grammar != grammar_name)
    {
		load_grammar(new_grammar);
        return true;
    }
    return false;
}

bool TextModel::update_theme()
{
	if (theme_name != CfgThemeFileName)
	{
		theme_name = CfgThemeFileName;
		
        std::string dll_path = getCurrentDllPath();
		Json::Value root = loadJson(dll_path + "\\themes\\" + theme_name);

		theme = parse_theme_mod(root, theme_name);
		for (int i = 0; i < blocks.size(); i++)
		{
			blocks[i].parser_state = NULL;
		}
        return true;
	}
    return false;
}

std::vector<Token> TextModel::get_line(int n)
{
    if (n > line_positions.size() - 1)
    {
        std::vector<Token> empty;
        return empty;
    }
    int block_idx = n / 100;
    if (blocks[block_idx].parser_state == NULL && gm != NULL && theme != NULL)
    {
        parse_block(block_idx);
    }
    if (text.lines[n].tokens.empty())
    {
        text.lines[n].tokens.push_back(Token{ &source[text.lines[n].start_index], text.lines[n].end_index - text.lines[n].start_index, fg_convert(theme->foreground()) });
    }
    return text.lines[n].tokens;
}

void TextModel::parse_block(int block_idx) 
{
	int first = blocks[block_idx].first_line;
	int last = blocks[block_idx].last_line;
    //if (last > line_positions.size() - 1) {
    //    last = line_positions.size() - 1;
    //}
	if (block_idx == 0 || blocks[block_idx - 1].parser_state == NULL)
	{
        parser_state = gm->seed();
	}
    else // todo valid var ?? 
    {
		parser_state = blocks[block_idx - 1].parser_state;
    }
    
    for (int i = first; i <= last; i++) 
    {
        WCHAR* first_char = &source[line_positions[i]];
        WCHAR* last_char = (i == (line_positions.size() - 1)) ? &source[source_size - 1] : &source[line_positions[i + 1]];
        //if (first_char > last_char)
        //{
        //    break;
        //}

        if (last_char > first_char && *(last_char - 1) == L'\r') --last_char;

        bool firstLine = first == i;
        std::map<size_t, scope::scope_t> scopes;
        parser_state = parse::parse((char*)first_char, (char*)last_char, parser_state, scopes, firstLine);
        //tokens_to_file(std::wstring(first_char, last_char), scopes);
        if (firstLine)
        {
            blocks[block_idx].scopes = scopes;
        }
        process_line(i, scopes);
    }

	blocks[block_idx].parser_state = parser_state;
    if (block_idx < blocks.size() - 1) {
        if (blocks[block_idx + 1].parser_state != NULL) {// TODO toto dvakrat implementacia dat do funkcie
            


            // TODO UPRATAT do funkcie 
            WCHAR* first_char = &source[line_positions[blocks[block_idx+1].first_line]];
            WCHAR* last_char;
            if (blocks[block_idx + 1].first_line == blocks[block_idx + 1].last_line)
            {
                last_char = &source[source_size - 1];
            }
            else
            {
                last_char = &source[line_positions[blocks[block_idx + 1].first_line + 1]];
            }
            std::map<size_t, scope::scope_t> scopes;
            parser_state = parse::parse((char*)first_char, (char*)last_char, parser_state, scopes, false);


            if (!scopes_equal(scopes, blocks[block_idx + 1].scopes)) {
				blocks[block_idx + 1].parser_state = NULL;
                parsing_corrected = true;
            }
        }
    }
}

bool TextModel::scopes_equal(std::map<size_t, scope::scope_t> scopes1, std::map<size_t, scope::scope_t> scopes2)
{
	if (scopes1.size() != scopes2.size()) {
		return false;
	}
	std::map<size_t, scope::scope_t>::iterator it1 = scopes1.begin();
	std::map<size_t, scope::scope_t>::iterator it2 = scopes2.begin();
	while (it1 != scopes1.end() && it2 != scopes2.end()) {
		if (it1->first != it2->first || it1->second != it2->second) {
			return false;
		}
		++it1;
		++it2;
	}
    if (it1 != scopes1.end() || it2 != scopes2.end()) {
		return false;
	}
	return true;
}

void TextModel::process_line(int n, std::map<size_t, scope::scope_t> scopes)
{
    std::vector<Token> tokens;
    //tokens.reserve(scopes.size()); // todo

    std::map<size_t, scope::scope_t>::iterator it = scopes.begin();
    while (it != scopes.end())
    {
        size_t position = it->first/2;
        scope::scope_t scope = it->second;

        Token new_token;
        new_token.start_ptr = &source[text.lines[n].start_index + position];

        std::string scopeName(scope);
        color_info_t color = theme->styles_for_scope(scopeName).foreground;
        if (color.red == -1 && color.green == 0 && color.blue == -0)
        {
            new_token.fg_color = fg_convert(theme->foreground()); // TODO nejaky default? co to je akoze ten default farebny co davam na ciernu POZORR
        }
        else
        {
            new_token.fg_color = fg_convert(color);
        }
        ++it;

        if (it == scopes.end() || it->first/2 > text.lines[n].end_index)
        {
            new_token.token_len = text.lines[n].end_index - text.lines[n].start_index - position;
        }
        else
        {
            new_token.token_len = it->first/2 - position;
        }

        if (new_token.token_len > (text.lines[n].end_index - text.lines[n].start_index)) {
            new_token.token_len = 0;
        }

        if (position <= text.lines[n].end_index) // TODO
        {
            tokens.push_back(new_token);
        }

    }
    text.lines[n].tokens = tokens;
}

std::vector<int> TextModel::calculate_display_positions(int line_idx, int tab_size)
{
    std::vector<int> display_positions;
	display_positions.reserve(text.lines[line_idx].end_index - text.lines[line_idx].start_index + 1);

    for (int i = text.lines[line_idx].start_index; i <= text.lines[line_idx].end_index; i++)
    {
        int pos = 0;
        if (!display_positions.empty())
        {
            pos = display_positions[display_positions.size() - 1];
        }
        
        if (source[i] == L'\t')
        {
            if (source[i - 1] == L'\t')
            {
                pos++;
            }
            pos += tab_size - (pos % tab_size)-1; // pozor
        }
        else if (!display_positions.empty())
        {
            pos++;
        }
        display_positions.push_back(pos);
    }
    return display_positions;
}

bool TextModel::process_selection(WCHAR** string, int* len, text_point start, text_point end)
{
    if (start.y < 0)
    {
		start.y = 0;
		start.x = 0;
    }
	if (end.y < 0)
	{
		end.y = 0;
		end.x = 0;
	}
    if (start.y > text.lines.size() - 1)
    {
        start.y = text.lines.size() - 1;
        start.x = text.lines[start.y].end_index - text.lines[start.y].start_index;
    }
    if (end.y > text.lines.size() - 1)
    {
        end.y = text.lines.size() - 1;
        end.x = text.lines[end.y].end_index - text.lines[end.y].start_index;
    }


    std::vector<int> start_display_pos = calculate_display_positions(start.y, CfgTabSize);

	for (int i = 0; i < start_display_pos.size(); i++)
	{
		if (start.x <= start_display_pos[i])
		{
			start.x = i;
			break;
		}
	}
    std::vector<int> end_display_pos = start_display_pos;
    
    if (start.y != end.y)
    {
        end_display_pos = calculate_display_positions(end.y, CfgTabSize);
    }

    for (int i = 0; i < end_display_pos.size(); i++)
    {
        if (end.x <= end_display_pos[i])
        {
            end.x = i;
            break;
        }
    }

    int start_index = line_positions[start.y];
	if (start.x > text.lines[start.y].end_index - text.lines[start.y].start_index) {
		start_index = text.lines[start.y].end_index;
	}
	else {
		start_index += start.x;
	}

	int end_index = line_positions[end.y];
	if (end.x > text.lines[end.y].end_index - text.lines[end.y].start_index) {
		end_index = text.lines[end.y].end_index;
	}
	else {
		end_index += end.x;
	}
	if (start_index > end_index)
	{
		int temp = start_index;
		start_index = end_index;
		end_index = temp;
	}

	*len = end_index - start_index;
	*string = &source[start_index];
	return (*len > 0);
}

bool TextModel::data_to_screen_pos(text_point& start, text_point& end) {
    std::vector<int> start_display_pos = calculate_display_positions(start.y, CfgTabSize);

	std::vector<int> end_display_pos = start_display_pos;
    if (start.y != end.y)
    {
		calculate_display_positions(end.y, CfgTabSize);
    }
	if (start.x > start_display_pos.size() - 1 || end.x > end_display_pos.size() - 1)
	{
		return false;
	}
	start.x = start_display_pos[start.x];
	end.x = end_display_pos[end.x - 1] + 1;
	return true;
}

int TextModel::get_line_length(int line)
{//todo revise
    int col = 0;

    for (int i = text.lines[line].start_index; i < text.lines[line].end_index; ++i) {
        if (source[i] == '\t') {
            col += CfgTabSize - (col % CfgTabSize);
        }
        else {
            col += 1;
        }
    }

	if (source[text.lines[line].end_index] == L'\r' && source[text.lines[line].end_index-1] == L'\n') {
		col -= 1;
	}
    return col;
}

Document TextModel::raw_doc() {
    //std::vector<uint32_t> ranges;
    //ranges.reserve(line_positions.size() + 1);
    //ranges.insert(ranges.end(), line_positions.begin(), line_positions.end());
    line_positions.push_back(source_size + 1);

    Document doc;
	doc.lines.reserve(line_positions.size());
    COLORREF default_color = get_fg();
    for (int i = 0; i < line_positions.size() - 1; i++)
    {
        Line new_line(line_positions[i], line_positions[i + 1] - 1);
        //Token new_token = Token(&source[new_line.start_index], new_line.end_index - new_line.start_index, default_color);
        //new_line.tokens.push_back(new_token); 
        doc.lines.push_back(new_line);
    }
    line_positions.pop_back();
    return doc;
}

bool TextModel::reparse_occured(int first_line, int last_line)
{
    for (int block = first_line / 100; block <= last_line / 100; block++)
    {
        get_line(block * 100);
    }

    if (parsing_corrected) 
    {
        parsing_corrected = false;
        return true;
    }
    else 
    {
        return false;
    }
}

size_t TextModel::get_line_count()
{
    return line_positions.size();
}

text_point TextModel::offset_to_xy(int offset)
{
    text_point res = {-1,-1};
	if (offset < 0) {
		return res;
	}

    for (int i = 0; i < line_positions.size(); i++)
    {
        if (line_positions[i] > offset)
        {
            res.y = i - 1;
            break;
        }
        else if (line_positions[i] == offset)
        {
            res.y = i;
            break;
        }
    }
    if (res.y == -1 && offset < source_size) {
        res.y = line_positions.size() - 1;
    }

	res.x = offset - line_positions[res.y];
    return res;
}

COLORREF TextModel::to_rgb(color_info_t color)
{
    return RGB(color.red * 255, color.green * 255, color.blue * 255);
}

COLORREF TextModel::bg_convert(color_info_t color)
{
    if (color.red == -1 && color.green == 0 && color.blue == -0)
    {
        return RGB(255, 255, 255);
    }
    else
    {
        return to_rgb(color);
    }
}

COLORREF TextModel::fg_convert(color_info_t color)
{
    if (color.red == -1 && color.green == 0 && color.blue == -0)
    {
        return RGB(0, 0, 0);
    }
    else
    {
        return to_rgb(color);
    }
}

COLORREF TextModel::get_bg()
{
	if (theme != NULL) {
		return bg_convert(theme->background());
	}
    else {
		return RGB(255, 255, 255);
    }
}

COLORREF TextModel::get_fg() 
{
    if (theme != NULL) {
        return fg_convert(theme->foreground());
    }
    else {
		return RGB(0, 0, 0);
    }
}

int TextModel::line_to_offset(int line) 
{
	if (line >= line_positions.size())
	{
		return source_size;
	}
	return line_positions[line];
}

// debugging
void TextModel::tokens_to_file(std::wstring wstr, std::map<size_t, scope::scope_t>& scopes)
{
    std::ofstream out("C:\\Users\\micha\\School\\bakalarka\\src\\plugins\\syntax-viewer\\plugin-files\\parsed.txt", std::ios::app);

    if (!out)
        return; // or handle error

    out << std::string(wstr.begin(), wstr.end()) << std::endl;
    out << "stateScope: " << to_s(parser_state->scope) << std::endl;
    out << "stateRuleMatch: " << parser_state->rule->match_string << std::endl;
    out << "stateEndPattern: " << to_s(parser_state->end_pattern) << std::endl;
    out << "stateAnchor: " << std::hex << parser_state->anchor << std::dec << std::endl;
    out << std::endl;


    std::map<size_t, scope::scope_t>::iterator it = scopes.begin();
    int iterator = 0;



    while (it != scopes.end())
    {
        size_t n = it->first;
        scope::scope_t scope = it->second;
        style_t style = theme->styles_for_scope(scope);
        out << iterator++ << " - " << n / 2 << "  atoms: " << scope.size() << "  atoms: " << to_s(scope).c_str()
            << "   ---|---  " << "red " << style.foreground.red << ", green " << style.foreground.green << ", blue " << style.foreground.blue
            << std::endl;
        ++it;
    }
    out << std::endl;
    out << std::endl;
}