//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2019-2020 Alexander Grund
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include "test.hpp"
#include "test_sets.hpp"
#include <boost/nowide/stackstring.hpp>
#include <iostream>
#include <vector>

#if defined(BOOST_MSVC) && BOOST_MSVC < 1700
#pragma warning(disable : 4428) // universal-character-name encountered in source
#endif

std::wstring stackstring_to_wide(const std::string& s)
{
    const boost::nowide::wstackstring ss(s.c_str());
    return ss.get();
}

std::string stackstring_to_narrow(const std::wstring& s)
{
    const boost::nowide::stackstring ss(s.c_str());
    return ss.get();
}

std::wstring heap_stackstring_to_wide(const std::string& s)
{
    const boost::nowide::basic_stackstring<wchar_t, char, 1> ss(s.c_str());
    return ss.get();
}

std::string heap_stackstring_to_narrow(const std::wstring& s)
{
    const boost::nowide::basic_stackstring<char, wchar_t, 1> ss(s.c_str());
    return ss.get();
}

int main()
{
    try
    {
        std::string hello = "\xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d";
        std::wstring whello = boost::nowide::widen(hello);
        const wchar_t* wempty = L"";

        {
            std::cout << "-- Default constructed string is NULL" << std::endl;
            const boost::nowide::short_stackstring s;
            TEST(s.get() == NULL);
        }
        {
            std::cout << "-- NULL ptr passed to ctor results in NULL" << std::endl;
            const boost::nowide::short_stackstring s(NULL);
            TEST(s.get() == NULL);
            const boost::nowide::short_stackstring s2(NULL, NULL);
            TEST(s2.get() == NULL);
        }
        {
            std::cout << "-- NULL ptr passed to convert results in NULL" << std::endl;
            boost::nowide::short_stackstring s(L"foo");
            TEST(s.get() == std::string("foo"));
            s.convert(NULL);
            TEST(s.get() == NULL);
            boost::nowide::short_stackstring s2(L"foo");
            TEST(s2.get() == std::string("foo"));
            s2.convert(NULL, NULL);
            TEST(s2.get() == NULL);
        }
        {
            std::cout << "-- An empty string is accepted" << std::endl;
            const boost::nowide::short_stackstring s(wempty);
            TEST(s.get());
            TEST(s.get() == std::string());
            const boost::nowide::short_stackstring s2(wempty, wempty);
            TEST(s2.get());
            TEST(s2.get() == std::string());
        }
        {
            std::cout << "-- An empty string is accepted" << std::endl;
            boost::nowide::short_stackstring s, s2;
            TEST(s.convert(wempty));
            TEST(s.get() == std::string());
            TEST(s2.convert(wempty, wempty));
            TEST(s2.get() == std::string());
        }
        {
            std::cout << "-- Will be put on heap" << std::endl;
            TEST(whello.size() >= 3);
            boost::nowide::basic_stackstring<wchar_t, char, 3> sw;
            TEST(sw.convert(hello.c_str()));
            TEST(sw.get() == whello);
            TEST(sw.convert(hello.c_str(), hello.c_str() + hello.size()));
            TEST(sw.get() == whello);
        }
        {
            std::cout << "-- Will be put on stack" << std::endl;
            TEST(whello.size() < 5);
            boost::nowide::basic_stackstring<wchar_t, char, 5> sw;
            TEST(sw.convert(hello.c_str()));
            TEST(sw.get() == whello);
            TEST(sw.convert(hello.c_str(), hello.c_str() + hello.size()));
            TEST(sw.get() == whello);
        }
        {
            std::cout << "-- Will be put on heap" << std::endl;
            TEST(hello.size() >= 5);
            boost::nowide::basic_stackstring<char, wchar_t, 5> sw;
            TEST(sw.convert(whello.c_str()));
            TEST(sw.get() == hello);
            TEST(sw.convert(whello.c_str(), whello.c_str() + whello.size()));
            TEST(sw.get() == hello);
        }
        {
            std::cout << "-- Will be put on stack" << std::endl;
            TEST(hello.size() < 10);
            boost::nowide::basic_stackstring<char, wchar_t, 10> sw;
            TEST(sw.convert(whello.c_str()));
            TEST(sw.get() == hello);
            TEST(sw.convert(whello.c_str(), whello.c_str() + whello.size()));
            TEST(sw.get() == hello);
        }
        {
            typedef boost::nowide::basic_stackstring<char, wchar_t, 5> stackstring;
            const std::string heapVal = hello;
            TEST(heapVal.size() >= 5); // Will be put on heap
            const std::wstring wtest = L"test";
            const std::string stackVal = "test";
            TEST(stackVal.size() < 5); // Will be put on stack
            const stackstring heap(whello.c_str());
            const stackstring stack(wtest.c_str());

            {
                stackstring sw2(heap), sw3;
                sw3 = heap;
                TEST(sw2.get() == heapVal);
                TEST(sw3.get() == heapVal);
            }
            {
                stackstring sw2(stack), sw3;
                sw3 = stack;
                TEST(sw2.get() == stackVal);
                TEST(sw3.get() == stackVal);
            }
            {
                stackstring sw2(stack);
                sw2 = heap;
                TEST(sw2.get() == heapVal);
            }
            {
                stackstring sw2(heap);
                sw2 = stack;
                TEST(sw2.get() == stackVal);
            }
            {
                stackstring sw2(heap), sw3(stack);
                swap(sw2, sw3);
                TEST(sw2.get() == stackVal);
                TEST(sw3.get() == heapVal);
                swap(sw2, sw3);
                TEST(sw2.get() == heapVal);
                TEST(sw3.get() == stackVal);
            }
            {
                stackstring sw2(heap), sw3(heap);
                sw3.get()[0] = 'z';
                const std::string val2 = sw3.get();
                swap(sw2, sw3);
                TEST(sw2.get() == val2);
                TEST(sw3.get() == heapVal);
            }
            {
                stackstring sw2(stack), sw3(stack);
                sw3.get()[0] = 'z';
                const std::string val2 = sw3.get();
                swap(sw2, sw3);
                TEST(sw2.get() == val2);
                TEST(sw3.get() == stackVal);
            }
            std::cout << "-- Sanity check" << std::endl;
            TEST(stack.get() == stackVal);
            TEST(heap.get() == heapVal);
        }
        {
            std::cout << "-- Test putting stackstrings into vector (done by args) class" << std::endl;
            // Use a smallish buffer, to have stack and heap values
            typedef boost::nowide::basic_stackstring<wchar_t, char, 5> stackstring;
            std::vector<stackstring> strings;
            strings.resize(2);
            TEST(strings[0].convert("1234") == std::wstring(L"1234"));
            TEST(strings[1].convert("Hello World") == std::wstring(L"Hello World"));
            strings.push_back(stackstring("FooBar"));
            TEST(strings[0].get() == std::wstring(L"1234"));
            TEST(strings[1].get() == std::wstring(L"Hello World"));
            TEST(strings[2].get() == std::wstring(L"FooBar"));
        }
        std::cout << "- Stackstring" << std::endl;
        run_all(stackstring_to_wide, stackstring_to_narrow);
        std::cout << "- Heap Stackstring" << std::endl;
        run_all(heap_stackstring_to_wide, heap_stackstring_to_narrow);
    } catch(const std::exception& e)
    {
        std::cerr << "Failed :" << e.what() << std::endl;
        return 1;
    }

    return 0;
}
