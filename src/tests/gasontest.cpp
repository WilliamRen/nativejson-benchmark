#if (defined(_MSC_VER) && _MSC_VER >= 1700) || (__cplusplus >= 201103L)

#include "../test.h"
#ifdef _MSC_VER
#pragma warning (disable:4244) // conversion from 'int' to 'char', possible loss of data
#pragma warning (disable:4800) // 'uint64_t' : forcing value to bool 'true' or 'false' (performance warning)
#endif
#include "gason/gason.cpp"
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <sstream>

static void GenStat(Stat& stat, const JsonValue& v) {
    switch (v.getTag()) {
    case JSON_TAG_ARRAY:
        for (auto i : v) {
            GenStat(stat, i->value);
            stat.elementCount++;
        }
        stat.arrayCount++;
        break;

    case JSON_TAG_OBJECT:
        for (auto i : v) {
            GenStat(stat, i->value);
            stat.memberCount++;
            stat.stringLength += strlen(i->key);
            stat.stringCount++;   // Key
        }
        stat.objectCount++;
        break;

    case JSON_TAG_STRING: 
        stat.stringCount++;
        stat.stringLength += strlen(v.toString());
        break;

    case JSON_TAG_NUMBER:
        stat.numberCount++;
        break;

    case JSON_TAG_BOOL:
        if (v.toBool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case JSON_TAG_NULL:
        stat.nullCount++;
        break;
    }
}

static void dumpString(std::ostringstream& os, const char *s) {
    os.put('"');
    while (*s) {
        char c = *s++;
        switch (c) {
        case '\b':
            os << "\\b";
            break;
        case '\f':
            os << "\\f";
            break;
        case '\n':
            os << "\\n";
            break;
        case '\r':
            os << "\\r";
            break;
        case '\t':
            os << "\\t";
            break;
        case '\\':
            os << "\\\\";
            break;
        case '"':
            os << "\\\"";
            break;
        default:
            os.put(c);
        }
    }
    os << s << "\"";
}

static void dumpValue(std::ostringstream& os, const JsonValue& o, int shiftWidth, int indent = 0) {
    switch (o.getTag()) {
    case JSON_TAG_NUMBER:
        char buffer[32];
        sprintf(buffer, "%f", o.toNumber());
        os << buffer;
        break;
    case JSON_TAG_BOOL:
        os << (o.toBool() ? "true" : "false");
        break;
    case JSON_TAG_STRING:
        dumpString(os, o.toString());
        break;
    case JSON_TAG_ARRAY:
        // It is not necessary to use o.toNode() to check if an array or object
        // is empty before iterating over its members, we do it here to allow
        // nicer pretty printing.
        if (!o.toNode()) {
            os << "[]";
            break;
        }
        os << "[\n";
        for (auto i : o) {
            if (shiftWidth > 0)
                os << std::setw(indent + shiftWidth) << " " << std::setw(0);
            dumpValue(os, i->value, shiftWidth, indent + shiftWidth);
            os << (i->next ? ",\n" : "\n");
        }
        if (indent > 0)
            os << std::setw(indent) << " " << std::setw(0);
        os.put(']');
        break;
    case JSON_TAG_OBJECT:
        if (!o.toNode()) {
            os << "{}";
            break;
        }
        os << "{\n";
        for (auto i : o) {
            if (shiftWidth > 0)
                os << std::setw(indent + shiftWidth) << " " << std::setw(0);
            dumpString(os, i->key);
            os << ": ";
            dumpValue(os, i->value, shiftWidth, indent + shiftWidth);
            os << (i->next ? ",\n" : "\n");
        }
        if (indent > 0)
            os << std::setw(indent) << " " << std::setw(0);
        os.put('}');
        break;
    case JSON_TAG_NULL:
        os << "null";
        break;
    }
}

class GasonParseResult : public ParseResultBase {
public:
    GasonParseResult() : json() {}
    ~GasonParseResult() { free(json); }

    JsonAllocator allocator;
    JsonValue value;
    char *json;
};

class GasonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const {
        return s.c_str();
    }

    std::string s;
};

class GasonTest : public TestBase {
public:
	GasonTest() : TestBase("Gason") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        GasonParseResult* pr = new GasonParseResult;
        char* end = 0;
        pr->json = strdup(json);
        if (jsonParse(pr->json, &end, &pr->value, pr->allocator) != JSON_PARSE_OK) {
            delete pr;
            return 0;
        }
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const GasonParseResult* pr = static_cast<const GasonParseResult*>(parseResult);
        std::ostringstream os;
        dumpValue(os, pr->value, 0);
        GasonStringResult* sr = new GasonStringResult;
        sr->s = os.str();
        return sr;
    }

    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const GasonParseResult* pr = static_cast<const GasonParseResult*>(parseResult);
        std::ostringstream os;
        dumpValue(os, pr->value, 4);
        GasonStringResult* sr = new GasonStringResult;
        sr->s = os.str();
        return sr;
    }

    virtual Stat Statistics(const ParseResultBase* parseResult) const {
        const GasonParseResult* pr = static_cast<const GasonParseResult*>(parseResult);
        Stat s;
        memset(&s, 0, sizeof(s));
        GenStat(s, pr->value);
        return s;
    }
};

REGISTER_TEST(GasonTest);

#endif