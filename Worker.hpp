#ifndef WORKER_HPP
#define WORKER_HPP

#include <string>
#include <thread>

#include "Wordlist.hpp"
#include "Subject.hpp"
#include "MatcherOptions.hpp"

class Worker
{
private:
    const Subject& _subject;
    const Wordlist& _passwordList;
    const Wordlist& _cipherList;
    const int _threadId;
    const std::string _matcherName;
    const MatcherOptions& _matcherOptions;
    
    volatile unsigned long _stats;
    std::thread _thread;
    
    void runner();
    void pkcsUnpad(unsigned char* pt, size_t size);

public:
    Worker(const Subject& subject, const Wordlist& passwordList, const Wordlist& cipherList, const int threadId, const std::string& matcherName, const MatcherOptions& matcherOptions)
        : _subject(subject)
        , _passwordList(passwordList)
        , _cipherList(cipherList)
        , _threadId(threadId)
        , _matcherName(matcherName)
        , _matcherOptions(matcherOptions)
        , _stats(0)
    {}

    ~Worker()
    {}

    void run();
    std::thread& getThread() { return _thread; }
};

#endif
