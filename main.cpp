/* -*- coding: utf-8; tab-width: 4 -*- */
/**
 * @file	main.cpp
 * @brief	string_searcher.hppの動作確認用コマンド
 * @author	Yasutaka SHINDOH / 新堂 安孝
 */

#include <cstdio>
#include <cstring>
#include <algorithm>
#include "string_searcher.hpp"

int main()
{
	const char input[] = "あらゆるげんじつをすべてじぶんのほうへねじまげたのだ。";
	const char pattern[] = "じぶん";
	size_t l = std::strlen(input);
	size_t m = std::strlen(pattern);

	ys::StringSearcher<char, size_t>* searchers[3];

	searchers[0] = new ys::BoyerMooreSearcher<char, size_t>(pattern);
	searchers[1] = new ys::HorspoolSearcher<char, size_t>(pattern);
	searchers[2] = new ys::SundaySearcher<char, size_t>(pattern);

	std::printf("[-] %s\n", input);

	for (size_t h(0); h < 3; ++h) {
		size_t i(0);

		while (i < l) {
			i = searchers[h]->search(input, l);
			if (i == ~(size_t)0) break;
			std::printf("[%lu] _", h);
			for (size_t j(0); j < m; ++j) std::printf("%c", input[i+j]);
			std::printf("_%s\n", input + i + m);
		}
	}

	std::for_each(searchers, searchers + 3, [](ys::StringSearcher<char, size_t>* p) { delete p; });

	return 0;
}
