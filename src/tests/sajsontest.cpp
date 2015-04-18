#include "../test.h"

#include "sajson/include/sajson.h"
#include <iostream>

using namespace sajson;

static void GenStat(Stat& stat, const value& v) {
    switch (v.get_type()) {
    case TYPE_ARRAY:
        {
            size_t size = v.get_length();
            for (size_t i = 0; i < size; i++)
                GenStat(stat, v.get_array_element(i));
            stat.arrayCount++;
            stat.elementCount += size;
        }
        break;

    case TYPE_OBJECT:
        {
            size_t size = v.get_length();
            for (size_t i = 0; i < size; i++) {
                GenStat(stat, v.get_object_value(i));
                stat.stringLength += v.get_object_key(i).length();
            }
            stat.objectCount++;
            stat.memberCount += size;
            stat.stringCount += size;
        }
        break;

    case TYPE_STRING: 
        stat.stringCount++;
        stat.stringLength += v.get_string_length();
        break;

    case TYPE_INTEGER:
    case TYPE_DOUBLE:
        stat.numberCount++;
        break;

    case TYPE_TRUE:
        stat.trueCount++;
        break;

    case TYPE_FALSE:
        stat.falseCount++;
        break;

    case TYPE_NULL:
        stat.nullCount++;
        break;
    }
}

class SajsonParseResult : public ParseResultBase {
public:
    SajsonParseResult(document&& d_) : d(std::move(d_)) {}
    document d;
};

class SajsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};
class SajsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "sajson (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        SajsonParseResult* pr = new SajsonParseResult(parse(literal(json)));
        if (!pr->d.is_valid()) {
            std::cout << "Error (" << pr->d.get_error_line() << ":" << pr->d.get_error_column() << "): " << pr->d.get_error_message() << std::endl;
        }
    	return pr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const SajsonParseResult* pr = static_cast<const SajsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->d.get_root());
        return true;
    }
#endif
};

REGISTER_TEST(SajsonTest);