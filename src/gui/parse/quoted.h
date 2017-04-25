
#pragma once
#include <string>
#include <iosfwd>

namespace parse {

	void printQuotedAscii(std::ostream &out, const std::string &str);

	void parseQuotedAscii(std::istream &in, std::string &str);

	template <typename T>
	class QuotedAsciiManip;

	template <>
	class QuotedAsciiManip<const std::string> {
	public:
		QuotedAsciiManip(const std::string &toPrint) : toPrint(toPrint) { }

		friend std::ostream & operator <<(std::ostream &out, const QuotedAsciiManip &qa) {
			printQuotedAscii(out, qa.toPrint);
			return out;
		}

	private:
		const std::string &toPrint;
	};

	template <>
	class QuotedAsciiManip<std::string> {
	public:
		QuotedAsciiManip(std::string &parseInto) : parseInto(parseInto) { }

		friend std::istream & operator >> (std::istream &in, QuotedAsciiManip &&qa) {
			parseQuotedAscii(in, qa.parseInto);
			return in;
		}

		friend std::ostream & operator <<(std::ostream &out, const QuotedAsciiManip &qa) {
			printQuotedAscii(out, qa.parseInto);
			return out;
		}

	private:
		std::string &parseInto;
	};

	// Call quoted(std::string &) or quoted(const std::string &).
	template <typename T>
	QuotedAsciiManip<T> quoted(T &str) {
		return QuotedAsciiManip<T>(str);
	}
}
