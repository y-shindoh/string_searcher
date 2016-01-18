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
		std::basic_string<CTYPE> pattern_;

		/// 文字列の次の探索開始位置
		size_t next_;

#ifndef	_NDEBUG
		/// 文字列比較処理の実施回数
		size_t count_;
#endif	// !_NDEBUG

	public:

		/**
		 * コンストラクタ
		 */
		StringSearcher()
#ifndef	_NDEBUG
			: count_(0)
#endif	// !_NDEBUG
			{
				;
			}

		/**
		 * コンストラクタ
		 * @param[in]	pattern	パターン文字列
		 */
		StringSearcher(const CTYPE* pattern)
			: pattern_(pattern), next_(0)
#ifndef	_NDEBUG
			, count_(0)
#endif	// !_NDEBUG
			{
				assert(pattern);
				assert(0 < pattern_.length());
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
		~StringSearcher() = default;

		/**
		 * パターンを設定
		 * @param[in]	pattern	パターン文字列
		 */
		virtual void
		prepare(const CTYPE* pattern) = 0;

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
		 * 文字列比較の実施回数を取得
		 * @return	文字列比較の実施回数
		 */
		virtual size_t
		get_count() const
			{
				return count_;
			}
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
		 */
		BoyerMooreSearcher(const CTYPE* pattern)
			: BoyerMooreSearcher::StringSearcher(pattern)
			{
				size_t n = this->pattern_.length();

				for (size_t i(0); i + 1 < n; ++i) {
					this->table_[pattern[i]] = (STYPE)(n - i - 1);
				}

				this->table_.rehash(this->table_.size());
			}

		/**
		 * デストラクタ
		 */
		virtual
		~BoyerMooreSearcher() = default;

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
		 * パターンを設定
		 * @param[in]	pattern	パターン文字列
		 */
		virtual void
		prepare(const CTYPE* pattern)
			{
				assert(pattern);
				assert(0 < std::strlen(pattern));

				this->pattern_ = pattern;
				this->next_ = 0;

				size_t n = this->pattern_.length();

				for (size_t i(0); i + 1 < n; ++i) {
					this->table_[pattern[i]] = (STYPE)(n - i - 1);
				}

				this->table_.rehash(this->table_.size());
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
				assert(this->next_ < ~(size_t)0);

				size_t n = this->pattern_.length();
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
		 */
		HorspoolSearcher(const CTYPE* pattern)
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
				assert(this->next_ < ~(size_t)0);

				size_t n = this->pattern_.length();
				size_t i(this->next_);

				while (i + n - 1 < length) {
#ifndef	_NDEBUG
					this->count_++;
#endif	// !_NDEBUG
					if (std::memcmp(buffer + i, this->pattern_.c_str(), sizeof(CTYPE) * n) == 0) {
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
		 */
		SundaySearcher(const CTYPE* pattern)
			: SundaySearcher::StringSearcher(pattern)
			{
				size_t n = this->pattern_.length();

				for (size_t i(0); i < n; ++i) {
					this->table_[pattern[i]] = (STYPE)(n - i - 1);
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
		 */
		virtual void
		prepare(const CTYPE* pattern)
			{
				assert(pattern);
				assert(0 < std::strlen(pattern));

				this->pattern_ = pattern;
				this->next_ = 0;

				size_t n = this->pattern_.length();

				for (size_t i(0); i < n; ++i) {
					this->table_[pattern[i]] = (STYPE)(n - i - 1);
				}

				this->table_.rehash(this->table_.size());
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
				assert(this->next_ < ~(size_t)0);

				size_t n = this->pattern_.length();
				size_t i(this->next_);

				while (i + n - 1 < length) {
#ifndef	_NDEBUG
					this->count_++;
#endif	// !_NDEBUG
					if (std::memcmp(buffer + i, this->pattern_.c_str(), sizeof(CTYPE) * n) == 0) {
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
	};
};

#endif	// __STRING_SEARCHER_HPP__
