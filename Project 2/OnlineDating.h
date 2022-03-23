//
//  OnlineDating.h
//  PROJECT 2

#ifndef ONLINEDATING_H
#define ONLINEDATING_H

#include <string>
typedef std::string OnlineType;

class OnlineDating
{
 private:
    struct Node {
        OnlineType firstName;
        OnlineType lastName;
        OnlineType value;
        Node *prev;
        Node *next;
    };
    Node *head=nullptr;
    int m_nMatches;

 public:
    // DC
    OnlineDating();
    // CC
    OnlineDating(const OnlineDating& src);
    // AO
    const OnlineDating& operator=(const OnlineDating& rhs);
    // D
    ~OnlineDating();
    
    // ACCESSOR FUNCTIONS
    bool noMatches() const {
        return (m_nMatches == 0);
    }

    int howManyMatches() const {
        return m_nMatches;
    }

    bool someoneAmongMatches(const std::string& firstName, const
                             std::string& lastName) const;

    bool lookAtMatches(const std::string& firstName, const
                       std::string& lastName, OnlineType& value) const;

    bool confirmMatch(int i, std::string& firstName, std::string&
                      lastName, OnlineType & value) const;
    
    // MUTATOR FUNCTIONS
    bool makeMatch(const std::string& firstName, const std::string&
                   lastName, const OnlineType& value);

    bool transformMatch(const std::string& firstName, const
                        std::string& lastName, const OnlineType & value);

    bool makeOrTransform(const std::string& firstName, const
                         std::string& lastName, const OnlineType& value);

    bool blockPreviousMatch(const std::string& firstName, const
                             std::string& lastName);

    void tradeMatches(OnlineDating& other);
};

bool mergeMatches(const OnlineDating & odOne,
                  const OnlineDating & odTwo,
                  OnlineDating & odJoined);

void authenticateMatches (const std::string& fsearch,
                          const std::string& lsearch,
                          const OnlineDating& odOne,
                          OnlineDating& odResult);

#endif /* ONLINEDATING_H */
