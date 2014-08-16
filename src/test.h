#pragma once

#include <vector>
#include <cstring>

#ifdef _MSC_VER
#define strdup _strdup
#endif

class TestBase;
typedef std::vector<const TestBase *> TestList;

class TestManager {
public:
    static TestManager& Instance() {
        static TestManager singleton;
        return singleton;
    }

    void AddTest(const TestBase* test) {
        mTests.push_back(test);
    }

    const TestList& GetTests() const {
        return mTests;
    }

    TestList& GetTests() {
        return mTests;
    }

private:
    TestList mTests;
};

struct Stat {
    size_t objectCount;
    size_t arrayCount;
    size_t numberCount;
    size_t stringCount;
    size_t trueCount;
    size_t falseCount;
    size_t nullCount;

    size_t memberCount;   // Number of members in all objects
    size_t elementCount;  // Number of elements in all arrays
    size_t stringLength;  // Number of code units in all strings
};

// Each test can customize what to be stored in parse result, 
// which will be passed to Stringify()/Prettify()/Statistics()
class ParseResultBase {
public:
    virtual ~ParseResultBase() {}
};

// Stringify()/Prettify() returns object derived from this class.
// So that it can prevents unncessary strdup().
class StringResultBase {
public:
    virtual ~StringResultBase() {}

    // The test framework call this function to get a null-terminated string.
    virtual const char* c_str() const = 0;
};

class TestBase {
public:
    TestBase(const char* name) : name_(name) {
        TestManager::Instance().AddTest(this);
    }

    const char* GetName() const {
        return name_;
    }

    bool operator<(const TestBase& rhs) const {
        return strcmp(name_, rhs.name_) < 0;
    }

    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        (void)json;
        return 0;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        (void)parseResult;
        return 0; 
    }

    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        (void)parseResult;
        return 0;
    }

    virtual Stat Statistics(const ParseResultBase* parseResult) const {
        (void)parseResult;
        Stat s;
        memset(&s, 0, sizeof(s));
        return s;
    }

protected:
    const char* name_;
};

#define STRINGIFY(x) #x
#define REGISTER_TEST(cls) static cls gRegister##cls
