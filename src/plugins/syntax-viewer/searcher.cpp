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

#include <algorithm>
#include "searcher.h"

Searcher::Searcher(TextModel* h) 
{
	this->content = h->get_content();
	this->content_size = h->get_source_size();
	this->h = h;
}

search_match Searcher::search(const WCHAR* pattern, bool forward, bool regular, bool hex_mode, bool whole_words, bool case_sensitive, int top_line, int bottom_line, bool next_or_prev)
{
	if ((active && attributes_match(pattern, forward, regular, hex_mode, whole_words, case_sensitive)) || next_or_prev) 
	{
        if (forward)
        {
            search_start = last_match_offset_end;
            if (next_or_prev)
            {
				search_end = content_size - 1;
            }
        }
        else
        {
            search_start = last_match_offset_start;
            if (next_or_prev)
            {
                search_end = 0;
            }
        }
	}
	else 
	{
		set_attributes(pattern, forward, regular, hex_mode, whole_words, case_sensitive);
		active = true;
		if (forward)
		{
			search_start = h->line_to_offset(top_line);
			search_end = content_size - 1;
		}
		else 
		{
			search_start = h->line_to_offset(bottom_line+1)-1;
			search_end = 0;
		}
	}
	int offset_start = -1;
	int offset_end = -1;
    search_match match = { {-1,-1}, {-1,-1} };

    if (regular)
    {
        regex_search(&offset_start, &offset_end);
    }
    else
    {
        standard_search(&offset_start, &offset_end);
    }
    match.start = h->offset_to_xy(offset_start);
    match.end = h->offset_to_xy(offset_end);

    if (offset_start != -1 && offset_end != -1)
    {
        last_match_offset_start = offset_start;
        last_match_offset_end = offset_end;
    }

    return match;
}


search_match Searcher::continue_search()
{
	if (!active) {
		return { {-1, -1}, {-1, -1} };
	}
	return search(pattern.c_str(), forward, regular, hex_mode, whole_words, case_sensitive, search_start, search_end);
}

search_match Searcher::find_next()
{
    if (!active) {
        return { {-1, -1}, {-1, -1} };
    }
    forward = true;
    return search(pattern.c_str(), forward, regular, hex_mode, whole_words, case_sensitive, search_start, search_end, true);
}

search_match Searcher::find_prev()
{
    if (!active) {
        return { {-1, -1}, {-1, -1} };
    }
    forward = false;
    return search(pattern.c_str(), forward, regular, hex_mode, whole_words, case_sensitive, search_start, search_end, true);
}

bool Searcher::standard_search(int* res_start, int* res_end)
{
    if (!pattern.length() || !content || content_size == 0) {
        return false;
    }

    // case insensitive pattern
    std::wstring search_pattern = pattern;
    if (!case_sensitive) {
		search_pattern = CharLowerW((LPWSTR)search_pattern.data());
    }

    if (forward) {
        // forward search
        for (int pos = search_start; pos <= search_end - (int)pattern.length(); pos++) {
            bool match = true;

            // Check if pattern match at pos
            for (size_t i = 0; i < pattern.length(); i++) {
                WCHAR content_char = content[pos + i];
                WCHAR pattern_char = search_pattern[i];

                if (!case_sensitive) {
                    content_char = (WCHAR)CharLowerW((LPWSTR)content_char);
                }

                if (content_char != pattern_char) {
                    match = false;
                    break;
                }
            }

            if (match) {
                // whole_words
                if (whole_words) {
                    bool valid_start = (pos == 0) || !isalnum(content[pos - 1]);
                    bool valid_end = (pos + pattern.length() >= content_size) || !isalnum(content[pos + pattern.length()]);

                    if (!valid_start || !valid_end) {
                        continue; // not whole word match
                    }
                }

                *res_start = pos;
                *res_end = pos + pattern.length();
                return true;
            }
        }
    }
    else {
        // backward search
        for (int pos = search_start - pattern.length(); pos >= search_end; pos--) {
            if (pos < 0) break;

            bool match = true;

            // Check if pattern match at pos
            for (size_t i = 0; i < pattern.length(); i++) {
                WCHAR content_char = content[pos + i];
                WCHAR pattern_char = search_pattern[i];

                if (!case_sensitive) {
                    content_char = tolower(content_char);
                }

                if (content_char != pattern_char) {
                    match = false;
                    break;
                }
            }

            if (match) {
                // whole words
                if (whole_words) {
                    bool valid_start = (pos == 0) || !isalnum(content[pos - 1]);
                    bool valid_end = (pos + pattern.length() >= content_size) || !isalnum(content[pos + pattern.length()]);

                    if (!valid_start || !valid_end) {
                        continue;
                    }
                }

                *res_start = pos;
                *res_end = pos + pattern.length();
                return true;
            }
        }
    }

    return false;
}


bool Searcher::regex_search(int* res_start, int* res_end) {
    OnigRegex reg;
    regex_t reg_buf;  // user-allocated regex buffer
    OnigErrorInfo einfo;
    OnigRegion* region;

    UChar* range;
    int r;
    search_match match = { -1,-1 };

    // Initialize Oniguruma
    //onig_initialize(NULL, 0);

    // Compile regex using user-provided buffer
    r = onig_new_without_alloc(
        &reg_buf,                 // pre-allocated buffer
        (UChar*)pattern.c_str(),
        (UChar*)pattern.c_str() + pattern.size()*2,
        ONIG_OPTION_DEFAULT,
        ONIG_ENCODING_UTF16_LE,
        ONIG_SYNTAX_DEFAULT,
        &einfo
    );

    if (r != ONIG_NORMAL) {
        UChar s[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(s, r, &einfo);
        fprintf(stderr, "ERROR: %s\n", s);
        return false;
    }

    reg = &reg_buf;

    region = onig_region_new();

    range = (UChar*)content+search_end;

    // Perform search
    r = onig_search(
        reg,
        (UChar*)content, (UChar*)content+(content_size-1)*2,
        (UChar*)content+search_start*2, range,
        region,
        ONIG_OPTION_NONE
    );

    if (r >= 0) {
        *res_start = region->beg[0]/2;
        *res_end = region->end[0]/2;
    }
    else if (r == ONIG_MISMATCH) {
        printf("No match\n");
    }
    else {
        printf("Search error\n");
    }

    // Cleanup
    onig_region_free(region, 1);
    onig_free_body(reg);  // important: only frees internal allocations
    onig_end();

    return r >= 0;
}

void Searcher::set_attributes(const WCHAR* pattern, bool forward, bool regular, bool hex_mode, bool whole_words, bool case_sensitive)
{
	this->pattern = pattern;
	this->forward = forward;
	this->regular = regular;
	this->hex_mode = hex_mode;
	this->whole_words = whole_words;
	this->case_sensitive = case_sensitive;
}

bool Searcher::attributes_match(const WCHAR* pattern, bool forward, bool regular, bool hex_mode, bool whole_words, bool case_sensitive)
{
	if (this->pattern == pattern && forward == this->forward && regular == this->regular && hex_mode == this->hex_mode && whole_words == this->whole_words && case_sensitive == this->case_sensitive)
	{
		return true;
	}
	else
	{
		return false;
	}

}