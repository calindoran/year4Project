#ifndef STDSTRINGFMT_H
#define STDSTRINGFMT_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <type_traits>

#ifdef QT_CORE_LIB
    #include <QString>
    #include <QStringList>
#endif

//usage std::string s = string_format() << "Operation with id = " << id << " failed, because data1 (" << data1 << ") is incompatible with data2 (" << data2 << ")"
//ref: https://habrahabr.ru/post/131977/

#ifndef _WIN32
    #include <climits>
    #include <cstdlib>
#else
    #include <windows.h>
#endif

namespace utility
{
    class string_format
    {
    public:
        template<class T>
        string_format& operator<< (const T& arg)
        {
            m_stream << arg;
            return *this;
        }
        operator std::string() const
        {
            return m_stream.str();
        }

#ifdef QT_CORE_LIB
        operator QString() const
        {
            return QString::fromUtf8(m_stream.str().c_str());
        }
#endif

    protected:
        std::stringstream m_stream;
    };


    template< typename... Args >
    std::string string_sprintf( const char* format, Args... args )
    {
        auto length = std::snprintf( nullptr, 0, format, args... );
        std::vector<char> buf;
        buf.resize(length + 1, 0);
        std::snprintf( buf.data(), length + 1, format, args... );
        return std::string( buf.data() );
    }

    //gets filename without path and WITH extension
    inline std::string baseFileName(const std::string& pathname)
    {
        return {std::find_if(pathname.rbegin(), pathname.rend(),
                             [](char c)
        {
            return c == '/' || c == '\\';
        }).base(),
        pathname.end()};
    }

    inline std::string getAbsolutePath(const std::string& fileName)
    {
        std::string result;
#ifndef _WIN32
        char *full_path = realpath(fileName.c_str(), nullptr);
        if (full_path)
        {
            result = std::string(full_path);
            free(full_path);
        }
#else
        TCHAR full_path[MAX_PATH];
        GetFullPathName(_T(fileName.c_str()), MAX_PATH, full_path, NULL);
        result = std::string(full_path);
#error Revise this code for windows
#endif
        return result;
    }


    //ref: http://cpp.indi.frih.net/blog/2014/09/how-to-read-an-entire-file-into-memory-in-cpp/
    template <typename CharT = char,
              typename Traits = std::char_traits<char>>
    std::streamsize streamSizeToEnd(std::basic_istream<CharT, Traits>& in)
    {
        auto const start_pos = in.tellg();
        if (std::streamsize(-1) == start_pos)
            throw std::ios_base::failure{"error"};

        if (!in.ignore(std::numeric_limits<std::streamsize>::max()))
            throw std::ios_base::failure{"error"};

        const std::streamsize char_count = in.gcount();

        if (!in.seekg(start_pos))
            throw std::ios_base::failure{"error"};

        return char_count;
    }

    template <typename Container = std::string,
              typename CharT = char,
              typename Traits = std::char_traits<char>>
    Container read_stream_into_container(
        std::basic_istream<CharT, Traits>& in,
        typename Container::allocator_type alloc = {})
    {
        static_assert(
            // Allow only strings...
            std::is_same<Container, std::basic_string<CharT,
            Traits,
            typename Container::allocator_type>>::value ||
            // ... and vectors of the plain, signed, and
            // unsigned flavours of CharT.
            std::is_same<Container, std::vector<CharT,
            typename Container::allocator_type>>::value ||
            std::is_same<Container, std::vector<
            typename std::make_unsigned<CharT>::type,
            typename Container::allocator_type>>::value ||
            std::is_same<Container, std::vector<
            typename std::make_signed<CharT>::type,
            typename Container::allocator_type>>::value,
            "only strings and vectors of ((un)signed) CharT allowed");

        auto const char_count = streamSizeToEnd(in);

        auto container = Container(std::move(alloc));
        container.resize(char_count);

        if (0 != container.size())
        {
            if (!in.read(reinterpret_cast<CharT*>(&container[0]), container.size()))
                throw std::ios_base::failure{"File size differs"};
        }
        return container;
    }

    inline std::string toLower(std::string src)
    {
        std::transform(src.begin(), src.end(), src.begin(), ::tolower);
        return src;
    }

#ifdef QT_CORE_LIB
    inline QString toLower(const QString& src)
    {
        return src.toLower();
    }
#endif

    inline bool endsWith (std::string const &fullString, std::string const &ending)
    {
        if (fullString.length() >= ending.length())
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        return false;
    }

    inline std::string removeExtension(const std::string& fileName)
    {
        size_t lastindex = fileName.find_last_of(".");
        return fileName.substr(0, lastindex);
    }

#ifdef QT_CORE_LIB
    inline bool strcontains(const QString& src, const QString& what)
    {
        return src.contains(what);
    }
#endif

    inline bool strcontains(const std::string& src, const std::string& what)
    {
        return std::string::npos != src.find(what);
    }

    //check if src string contains one of substrings listed in what
    template <class T>
    bool strcontains(const T& src, const std::vector<T>& what)
    {
        for (const auto& w : what)
        {
            if (strcontains(src, w))
                return true;
        }
        return false;
    }

    namespace ToStdStringNS
    {
        template<typename T>
        struct HasToStringMethod
        {
            template<typename U, size_t (U::*)() const> struct SFINAE {};
            template<typename U> static char Test(SFINAE<U, &U::toString>*);
            template<typename U> static int Test(...);
            static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
        };

        template<typename T>
        std::string toStdStrHelper(const T& m, std::true_type)
        {
            return m.toString();
        }

        template<typename T>
        std::string toStdStrHelper(const T& m, std::false_type)
        {
            return m; //use implicit conversion if any there
        }
    }

#ifdef QT_CORE_LIB
    inline std::string toStdStr(const QString& src)
    {
        return src.toStdString();
    }
#endif

    template <class Anything>
    inline std::string toStdStr(const Anything& src)
    {
        return ToStdStringNS::toStdStrHelper(src, std::integral_constant<bool, HasToStringMethod<Anything>::Has>());
    }
}
#endif // STDSTRINGFMT_H
