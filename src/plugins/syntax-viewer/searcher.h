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

#include "syntaxview.h"

struct search_match
{
    text_point start;
    text_point end;
};

class Searcher
{
public:
	Searcher(TextModel* h);
    
    search_match search(const WCHAR* pattern, bool forward, bool regular, bool hex_mode, bool whole_words, bool case_sensitive, int top_line, int bottom_line, bool next_or_prev = false);
    search_match continue_search();
	search_match find_next();
	search_match find_prev();
	bool is_forward_search() const { return forward; }

private:
	WCHAR* content = NULL;
	long content_size = 0;
	TextModel* h = NULL;

    bool active = false;
    std::wstring pattern;
    bool forward = false;
    bool regular = false;
	bool hex_mode = false;
	bool whole_words = false;
	bool case_sensitive = false;

	int search_start = 0;
    int search_end = 0;

	int last_match_offset_start = -1;
	int last_match_offset_end = -1;

    bool standard_search(int* res_start, int* res_end);
    bool regex_search(int* res_start, int* res_end);
	
    bool attributes_match(const WCHAR* pattern, bool forward, bool regular, bool hex_mode, bool whole_words, bool case_sensitive);
    void set_attributes(const WCHAR* pattern, bool forward, bool regular, bool hex_mode, bool whole_words, bool case_sensitive);
};