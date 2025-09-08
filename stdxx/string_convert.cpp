#include "string_convert.h"


namespace stdxx 
{
#if !defined(CODECVT_ENABLE)
	namespace detail
	{
		template <typename OutCharT, typename InCharT>
		std::basic_string<OutCharT> convert(
			const std::basic_string<InCharT>& source,
			const char* out_code, const char* in_code)
		{
			if (source.empty()) return std::basic_string<OutCharT>();
			iconv_t iconv = ::iconv_open(out_code, in_code);
			if (iconv == (iconv_t)-1)
			{
				perror("iconv_open error");
				return std::basic_string<OutCharT>();
			}

			size_t in_size = source.size() * sizeof(InCharT);
			size_t out_size = (source.size() + 1) * 8;  // 8 is redundance
			std::vector<OutCharT> buffer(out_size, 0);
			char* in_string = (char*)source.data();
			char* out_string = (char*)buffer.data();
			size_t ret_size = ::iconv(iconv, &in_string, &in_size, &out_string, &out_size);
			::iconv_close(iconv);
			if (ret_size == (size_t)-1)
			{
				perror("iconv convert failed!");
				return std::basic_string<OutCharT>();
			}
			else
			{
				return std::basic_string<OutCharT>(buffer.data());
			}
		}
	}  // namespace detail
#endif

	std::string to_string(const std::wstring& wstring, const std::locale& locale)
	{
		using codecvt_t = std::codecvt<wchar_t, char, std::mbstate_t>;

		const codecvt_t& codecvt(std::use_facet<codecvt_t>(locale));
		std::mbstate_t state;
		std::vector<char> buffer((wstring.size() + 1) * codecvt.max_length());
		const wchar_t* in_next{ wstring.c_str() };
		char* out_next{ &buffer[0] };

		std::codecvt_base::result result{ codecvt.out(state, wstring.c_str(), wstring.c_str() + wstring.size(), in_next, &buffer[0], &buffer[0] + buffer.size(), out_next) };

		//if (result == std::codecvt_base::error)
		//	throw std::runtime_error("can not convert wstring to string");

		return &buffer[0];
	}

	std::wstring to_wstring(const std::string& string, const std::locale& locale)
	{
		using codecvt_t = std::codecvt<wchar_t, char, std::mbstate_t>;

		const codecvt_t& codecvt(std::use_facet<codecvt_t>(locale));
		std::mbstate_t state{};
		std::vector<wchar_t> buffer(string.size() + 1);
		const char* in_next{ string.c_str() };
		wchar_t* out_next{ &buffer[0] };

		std::codecvt_base::result result{ codecvt.in(state, string.c_str(), string.c_str() + string.size(), in_next, &buffer[0], &buffer[0] + buffer.size(), out_next) };

		//if (result == std::codecvt_base::error)
		//	throw std::runtime_error("can not convert string to wstring");

		return &buffer[0];
	}


	std::u16string to_u16string(const std::string& u8string)
	{
#if defined(STDXX_WINDOWS) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8<int16_t>, int16_t> convert;
		auto data = convert.from_bytes(u8string);
		return std::u16string(reinterpret_cast<char16_t const *>(data.data()), data.length());
#elif defined(STDXX_POSIX) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
		return convert.from_bytes(u8string);
#else
		return detail::convert<char16_t>(u8string, "UCS-2", "UTF-8");
#endif
	}

	std::u32string to_u32string(const std::string& u8string)
	{
#if defined(STDXX_WINDOWS) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> convert;
		auto data = convert.from_bytes(u8string);
		return std::u32string(reinterpret_cast<char32_t const *>(data.data()), data.length());
#elif defined(STDXX_POSIX) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
		return convert.from_bytes(u8string);
#else
		return detail::convert<char32_t>(u8string, "UCS-4", "UTF-8");
#endif
	}

	std::string to_u8string(const std::u16string& u16string)
	{
#if defined(STDXX_WINDOWS) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
		auto data = reinterpret_cast<const int16_t *>(u16string.data());
		return convert.to_bytes(data, data + u16string.size());
#elif defined(STDXX_POSIX) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
		return convert.to_bytes(u16string);
#else
		return detail::convert<char>(u16string, "UTF-8", "UCS-2");
#endif
	}

	std::u32string to_u32string(const std::u16string& u16string)
	{
		std::string str{ to_u8string(u16string) };
		return to_u32string(str);
	}

	std::string to_u8string(const std::u32string& u32string)
	{
#if defined(STDXX_WINDOWS) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8_utf16<int32_t>, int32_t> convert;
		auto data = reinterpret_cast<const int32_t *>(u32string.data());
		return convert.to_bytes(data, data + u32string.size());
#elif defined(STDXX_POSIX) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
		return convert.to_bytes(u32string);
#else
		return detail::convert<char>(u32string, "UTF-8", "UCS-4");
#endif
	}


	std::u16string to_u16string(const std::u32string& u32string)
	{
		std::string str{ to_u8string(u32string) };
		return to_u16string(str);
	}

	std::string to_u8string(const std::string &string, const std::locale& locale)
	{
#if defined(CODECVT_ENABLE)
		std::wstring wstr = to_wstring(string, locale);
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		return convert.to_bytes(wstr);
#else
		return detail::convert<char>(string, "UTF-8", "GB18030");
#endif
	}

	std::string to_u8string(const std::wstring &wstring, const std::locale& locale)
	{
#if defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		return convert.to_bytes(wstring);
#else
		return detail::convert<char>(wstring, "UTF-8", "WCHAR_T");
#endif
	}

    std::string u8_to_string(const std::string &u8string)
	{
#if defined(CODECVT_ENABLE)
        return to_string(u8_to_wstring(u8string));
#else
		return detail::convert<char>(u8string, "GB18030", "UTF-8");
#endif
	}

    std::wstring u8_to_wstring(const std::string &u8string)
    {
#if defined(CODECVT_ENABLE)
        std::wstring_convert< std::codecvt_utf8<wchar_t> > convert;
        return convert.from_bytes(u8string);
#else
        return detail::convert<wchar_t>(u8string, "WCHAR_T", "UTF-8");
#endif
    }

	std::u16string to_ucs2(const std::string &u8string)
	{
#if defined(STDXX_WINDOWS) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8<short>, short> convert;
		auto data = convert.from_bytes(u8string);
		return std::u16string(reinterpret_cast<char16_t const *>(data.data()), data.length());
#elif defined(STDXX_POSIX) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
		return convert.from_bytes(u8string.data());
#else
		return detail::convert<char16_t>(u8string, "UCS-2", "UTF-8");
#endif
	}

	std::u32string to_ucs4(const std::string &u8string)
	{
#if defined(STDXX_WINDOWS) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> convert;
		auto data = convert.from_bytes(u8string);
		return std::u32string(reinterpret_cast<char32_t const *>(data.data()), data.length());
#elif defined(STDXX_POSIX) && defined(CODECVT_ENABLE)
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
		return convert.from_bytes(u8string);
#else
		return detail::convert<char32_t>(u8string, "UCS-4", "UTF-8");
#endif
	}

	std::u32string to_ucs4(const std::u16string &u16string)
	{
#if defined(STDXX_WINDOWS) && defined(CODECVT_ENABLE)
		return to_u32string(u16string);
#elif defined(STDXX_POSIX) && defined(CODECVT_ENABLE)
		const char16_t *data = u16string.c_str();
		std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> convert;
		return convert.from_bytes(reinterpret_cast<const char*>(data), reinterpret_cast<const char*>(data + u16string.length()));
#else
		return detail::convert<char32_t>(u16string, "UCS-4", "UCS-2");
#endif
    }

}
