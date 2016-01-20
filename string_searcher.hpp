/* -*- coding: utf-8; tab-width: 4 -*- */
/**
 * @file	string_searcher.hpp
 * @brief	BM法およびその派生の文字列探索アルゴリズム
 * @author	Yasutaka SHINDOH / 新堂 安孝
 * @note	C++11向けに実装している。
 */

#ifndef	__STRING_SEARCHER_HPP__
#define	__STRING_SEARCHER_HPP__	"string_searcher.hpp"

#include <cstring>
#include <cassert>
#include <unordered_map>
#include <string>

namespace ys
{
	/**
	 * BM法およびその派生の文字列探索アルゴリズムのスーパー・クラス
	 * @note	引数 @a STYPE には符号なし整数を与えること。
	 */
	template<typename CTYPE = char, typename STYPE = size_t>
	class StringSearcher
	{
	protected:

		/// パターン文字列の各文字の末尾からの距離
		std::unordered_map<CTYPE, STYPE> table_;

		/// パターン文字列
		CTYPE* pattern_;

		/// パターン文字列の長さ
		size_t length_;

		/// 文字列の次の探索開始位置
		size_t next_;

#ifndef	_NDEBUG
		/// 文字列比較処理の実施回数
		size_t count_;
#endif	// !_NDEBUG

		/**
		 * パターン文字列を格納
		 * @param[in]	pattern	パターン文字列
		 * @param[in]	length	配列 @a pattern の要素数
		 */
		void
		copy_pattern(const CTYPE* pattern,
					 size_t length)
			{
				assert(pattern);
				assert(0 < length);

				if (pattern_) {
					delete [] pattern_;
					pattern_ = 0;
					length_ = 0;
				}

				pattern_ = new CTYPE[length];
				std::memcpy((void*)pattern_, (const void*)pattern, sizeof(CTYPE) * length);
				length_ = length;
				next_ = 0;
			}

	public:

		/**
		 * コンストラクタ
		 */
		StringSearcher()
			: pattern_(0)
#ifndef	_NDEBUG
			, count_(0)
#endif	// !_NDEBUG
			{
				;
			}

		/**
		 * コンストラクタ
		 * @param[in]	pattern	パターン文字列
		 * @param[in]	length	配列 @a pattern の要素数
		 */
		StringSearcher(const CTYPE* pattern,
					   size_t length)
#ifndef	_NDEBUG
			: count_(0)
#endif	// !_NDEBUG
			{
				assert(pattern);
				assert(0 < length);

				copy_pattern(pattern, length);
			}

		/**
		 * コンストラクタ
		 * @param[in]	pattern	パターン文字列
		 */
		StringSearcher(const std::basic_string<CTYPE>& pattern)
#ifndef	_NDEBUG
			: count_(0)
#endif	// !_NDEBUG
			{
				assert(0 < pattern.length());

				copy_pattern(pattern.c_str(), pattern.length());
			}

		/**
		 * コピー・コンストラクタ (使用禁止)
		 */
		StringSearcher(const StringSearcher<CTYPE, STYPE>&) = delete;

		/**
		 * 代入演算子 (使用禁止)
		 */
		StringSearcher&
		operator =(const StringSearcher<CTYPE, STYPE>&) = delete;

		/**
		 * デストラクタ
		 */
		virtual
		~StringSearcher()
			{
				if (pattern_) delete [] pattern_;
			}

		/**
		 * パターンを設定
		 * @param[in]	pattern	パターン文字列
		 * @param[in]	length	配列 @a pattern の要素数
		 */
		virtual void
		prepare(const CTYPE* pattern,
				size_t length) = 0;

		/**
		 * パターンを設定
		 * @param[in]	pattern	パターン文字列
		 */
		virtual void
		prepare(const std::basic_string<CTYPE>& pattern) = 0;

		/**
		 * 探索の状態を初期化
		 * @note	本メソッドにより、
					メソッド @a search を呼び出す前に状態を戻すことができる。
		 */
		virtual void
		rewind()
			{
				next_ = 0;
			}

		/**
		 * 探索
		 * @param[in]	buffer	探索対象文字列
		 * @param[in]	length	文字列 @a buffer の長さ
		 * @return	文字列 @a buffer 内のパターン文字列の位置
		 * @note	探索失敗時には @a STYPE の最大値が返却される。
		 * @note	本メソッドを連続で呼び出すことで、
					@a buffer 内の全パターン文字列が抽出できる。
		 */
		virtual size_t
		search(const CTYPE* buffer,
			   size_t length) = 0;

		/**
		 * 探索
		 * @param[in]	buffer	探索対象文字列
		 * @return	文字列 @a buffer 内のパターン文字列の位置
		 * @note	探索失敗時には @a STYPE の最大値が返却される。
		 * @note	本メソッドを連続で呼び出すことで、
					@a buffer 内の全パターン文字列が抽出できる。
		 */
		virtual size_t
		search(const std::basic_string<CTYPE>& buffer) = 0;

#ifndef	_NDEBUG
		/**
		 * 文字列比較の実施回数を取得
		 * @return	文字列比較の実施回数
		 */
		virtual size_t
		get_count() const
			{
				return count_;
			}
#endif	// !_NDEBUG
	};

	/**
	 * BM法アルゴリズムのクラス
	 */
	template<typename CTYPE = char, typename STYPE = size_t>
	class BoyerMooreSearcher
		: public StringSearcher<CTYPE, STYPE>
	{
	public:

		/**
		 * コンストラクタ
		 */
		BoyerMooreSearcher() = default;

		/**
		 * コンストラクタ
		 * @param[in]	pattern	パターン文字列
		 * @param[in]	length	配列 @a pattern の要素数
		 */
		BoyerMooreSearcher(const CTYPE* pattern,
						   size_t length)
			: BoyerMooreSearcher::StringSearcher(pattern, length)
			{
				for (size_t i(0); i + 1 < length; ++i) {
					this->table_[pattern[i]] = (STYPE)(length - i - 1);
				}

				this->table_.rehash(this->table_.size());
			}

		/**
		 * コンストラクタ
		 * @param[in]	pattern	パターン文字列
		 */
		BoyerMooreSearcher(const std::basic_string<CTYPE>& pattern)
			: BoyerMooreSearcher::StringSearcher(pattern)
			{
				for (size_t i(0); i + 1 < this->length_; ++i) {
					this->table_[pattern[i]] = (STYPE)(this->length_ - i - 1);
				}

				this->table_.rehash(this->table_.size());
			}

		/**
		 * コピー・コンストラクタ (使用禁止)
		 */
		BoyerMooreSearcher(const BoyerMooreSearcher<CTYPE, STYPE>&) = delete;

		/**
		 * 代入演算子 (使用禁止)
		 */
		BoyerMooreSearcher&
		operator =(const BoyerMooreSearcher<CTYPE, STYPE>&) = delete;

		/**
		 * デストラクタ
		 */
		virtual
		~BoyerMooreSearcher() = default;

		/**
		 * パターンを設定
		 * @param[in]	pattern	パターン文字列
		 * @param[in]	length	配列 @a pattern の要素数
		 */
		virtual void
		prepare(const CTYPE* pattern,
				size_t length)
			{
				assert(pattern);
				assert(0 < length);

				this->copy_pattern(pattern, length);

				for (size_t i(0); i + 1 < length; ++i) {
					this->table_[pattern[i]] = (STYPE)(length - i - 1);
				}

				this->table_.rehash(this->table_.size());
			}

		/**
		 * パターンを設定
		 * @param[in]	pattern	パターン文字列
		 */
		virtual void
		prepare(const std::basic_string<CTYPE>& pattern)
			{
				prepare(pattern.c_str(), pattern.length());
			}

		/**
		 * 探索
		 * @param[in]	buffer	探索対象文字列
		 * @param[in]	length	文字列 @a buffer の長さ
		 * @return	文字列 @a buffer 内のパターン文字列の位置
		 * @note	探索失敗時には @a STYPE の最大値が返却される。
		 * @note	本メソッドを連続で呼び出すことで、
					@a buffer 内の全パターン文字列が抽出できる。
		 */
		virtual size_t
		search(const CTYPE* buffer,
			   size_t length)
			{
				assert(buffer);
				assert(0 < length);
				assert(this->pattern_);
				assert(0 < this->length_);
				assert(this->next_ < ~(size_t)0);

				size_t n = this->length_;
				size_t d = this->next_ < n - 1 ? n - 1 : this->next_;

				for (size_t i(d); i < length; ++i) {
					bool f(false);
#ifndef	_NDEBUG
					this->count_++;
#endif	// !_NDEBUG
					for (size_t j(0); j < n; ++j) {
						if (buffer[i-j] == this->pattern_[n-j-1]) continue;
						f = true;
						size_t k = n;
						if (this->table_.find(buffer[i-j]) != this->table_.end()) {
							k = (size_t)this->table_[buffer[i-j]];
						}
						if (j < k) i += k - j - 1;	// スキップ
						break;
					}
					if (f) continue;

					this->next_ = i + 1;
					return i - (n - 1);
				}

				return this->next_ = ~(size_t)0;
			}

		/**
		 * 探索
		 * @param[in]	buffer	探索対象文字列
		 * @return	文字列 @a buffer 内のパターン文字列の位置
		 * @note	探索失敗時には @a STYPE の最大値が返却される。
		 * @note	本メソッドを連続で呼び出すことで、
					@a buffer 内の全パターン文字列が抽出できる。
		 */
		virtual size_t
		search(const std::basic_string<CTYPE>& buffer)
			{
				return search(buffer.c_str(), buffer.length());
			}
	};

	/**
	 * BMH法アルゴリズムのクラス
	 */
	template<typename CTYPE = char, typename STYPE = size_t>
	class HorspoolSearcher
		: public BoyerMooreSearcher<CTYPE, STYPE>
	{
	public:

		/**
		 * コンストラクタ
		 */
		HorspoolSearcher() = default;

		/**
		 * コンストラクタ
		 * @param[in]	pattern	パターン文字列
		 * @param[in]	length	配列 @a pattern の要素数
		 */
		HorspoolSearcher(const CTYPE* pattern,
						 size_t length)
			: HorspoolSearcher::BoyerMooreSearcher(pattern, length)
			{
				;
			}

		/**
		 * コンストラクタ
		 * @param[in]	pattern	パターン文字列
		 */
		HorspoolSearcher(const std::basic_string<CTYPE>& pattern)
			: HorspoolSearcher::BoyerMooreSearcher(pattern)
			{
				;
			}

		/**
		 * コピー・コンストラクタ (使用禁止)
		 */
		HorspoolSearcher(const HorspoolSearcher<CTYPE, STYPE>&) = delete;

		/**
		 * 代入演算子 (使用禁止)
		 */
		HorspoolSearcher&
		operator =(const HorspoolSearcher<CTYPE, STYPE>&) = delete;

		/**
		 * デストラクタ
		 */
		virtual
		~HorspoolSearcher() = default;

		/**
		 * 探索
		 * @param[in]	buffer	探索対象文字列
		 * @param[in]	length	文字列 @a buffer の長さ
		 * @return	文字列 @a buffer 内のパターン文字列の位置
		 * @note	探索失敗時には @a STYPE の最大値が返却される。
		 * @note	本メソッドを連続で呼び出すことで、
					@a buffer 内の全パターン文字列が抽出できる。
		 */
		virtual size_t
		search(const CTYPE* buffer,
			   size_t length)
			{
				assert(buffer);
				assert(0 < length);
				assert(this->pattern_);
				assert(0 < this->length_);
				assert(this->next_ < ~(size_t)0);

				size_t n = this->length_;
				size_t i(this->next_);

				while (i + n - 1 < length) {
#ifndef	_NDEBUG
					this->count_++;
#endif	// !_NDEBUG
					if (std::memcmp((void*)(buffer + i), (const void*)this->pattern_, sizeof(CTYPE) * n) == 0) {
						this->next_ = i + 1;
						return i;
					}
					size_t k = n;
					if (this->table_.find(buffer[i+n-1]) != this->table_.end()) {
						k = (size_t)this->table_[buffer[i+n-1]];
					}
					i += k;	// スキップ
				}

				return this->next_ = ~(size_t)0;
			}

		/**
		 * 探索
		 * @param[in]	buffer	探索対象文字列
		 * @return	文字列 @a buffer 内のパターン文字列の位置
		 * @note	探索失敗時には @a STYPE の最大値が返却される。
		 * @note	本メソッドを連続で呼び出すことで、
					@a buffer 内の全パターン文字列が抽出できる。
		 */
		virtual size_t
		search(const std::basic_string<CTYPE>& buffer)
			{
				return search(buffer.c_str(), buffer.length());
			}
	};

	/**
	 * BMS法 (Quick Search法) アルゴリズムのクラス
	 */
	template<typename CTYPE = char, typename STYPE = size_t>
	class SundaySearcher
		: public StringSearcher<CTYPE, STYPE>
	{
	public:

		/**
		 * コンストラクタ
		 */
		SundaySearcher() = default;

		/**
		 * コンストラクタ
		 * @param[in]	pattern	パターン文字列
		 * @param[in]	length	配列 @a pattern の要素数
		 */
		SundaySearcher(const CTYPE* pattern,
					   size_t length)
			: SundaySearcher::StringSearcher(pattern, length)
			{
				assert(pattern);
				assert(0 < length);

				this->copy_pattern(pattern, length);

				for (size_t i(0); i < length; ++i) {
					this->table_[pattern[i]] = (STYPE)(length - i - 1);
				}

				this->table_.rehash(this->table_.size());
			}

		/**
		 * コンストラクタ
		 * @param[in]	pattern	パターン文字列
		 */
		SundaySearcher(const std::basic_string<CTYPE>& pattern)
			: SundaySearcher::StringSearcher(pattern)
			{
				assert(0 < pattern.length());

				this->copy_pattern(pattern.c_str(), pattern.length());

				for (size_t i(0); i < this->length_; ++i) {
					this->table_[pattern[i]] = (STYPE)(this->length_ - i - 1);
				}

				this->table_.rehash(this->table_.size());
			}

		/**
		 * コピー・コンストラクタ (使用禁止)
		 */
		SundaySearcher(const SundaySearcher<CTYPE, STYPE>&) = delete;

		/**
		 * 代入演算子 (使用禁止)
		 */
		SundaySearcher&
		operator =(const SundaySearcher<CTYPE, STYPE>&) = delete;

		/**
		 * デストラクタ
		 */
		virtual
		~SundaySearcher() = default;

		/**
		 * パターンを設定
		 * @param[in]	pattern	パターン文字列
		 * @param[in]	length	配列 @a pattern の要素数
		 */
		virtual void
		prepare(const CTYPE* pattern,
				size_t length)
			{
				assert(pattern);
				assert(0 < length);

				this->copy_pattern(pattern, length);

				for (size_t i(0); i < length; ++i) {
					this->table_[pattern[i]] = (STYPE)(length - i - 1);
				}

				this->table_.rehash(this->table_.size());
			}

		/**
		 * パターンを設定
		 * @param[in]	pattern	パターン文字列
		 */
		virtual void
		prepare(const std::basic_string<CTYPE>& pattern)
			{
				prepare(pattern.c_str(), pattern.length());
			}

		/**
		 * 探索
		 * @param[in]	buffer	探索対象文字列
		 * @param[in]	length	文字列 @a buffer の長さ
		 * @return	文字列 @a buffer 内のパターン文字列の位置
		 * @note	探索失敗時には @a STYPE の最大値が返却される。
		 * @note	本メソッドを連続で呼び出すことで、
					@a buffer 内の全パターン文字列が抽出できる。
		 */
		virtual size_t
		search(const CTYPE* buffer,
			   size_t length)
			{
				assert(buffer);
				assert(0 < length);
				assert(this->pattern_);
				assert(0 < this->length_);
				assert(this->next_ < ~(size_t)0);

				size_t n = this->length_;
				size_t i(this->next_);

				while (i + n - 1 < length) {
#ifndef	_NDEBUG
					this->count_++;
#endif	// !_NDEBUG
					if (std::memcmp((void*)(buffer + i), (const void*)this->pattern_, sizeof(CTYPE) * n) == 0) {
						this->next_ = i + 1;
						return i;
					}
					size_t k = n;
					if (this->table_.find(buffer[i+n]) != this->table_.end()) {
						k = (size_t)this->table_[buffer[i+n]];
					}
					i += k + 1;	// スキップ
				}

				return this->next_ = ~(size_t)0;
			}

		/**
		 * 探索
		 * @param[in]	buffer	探索対象文字列
		 * @return	文字列 @a buffer 内のパターン文字列の位置
		 * @note	探索失敗時には @a STYPE の最大値が返却される。
		 * @note	本メソッドを連続で呼び出すことで、
					@a buffer 内の全パターン文字列が抽出できる。
		 */
		virtual size_t
		search(const std::basic_string<CTYPE>& buffer)
			{
				return search(buffer.c_str(), buffer.length());
			}
	};
};

#endif	// __STRING_SEARCHER_HPP__
