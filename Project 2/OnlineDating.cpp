//
//  OnlineDating.cpp
//  PROJECT 2

#include "OnlineDating.h"
#include <string>
using namespace std;

// default constructor
OnlineDating::OnlineDating() : head(NULL), m_nMatches(0){
}

// destructor
OnlineDating::~OnlineDating()
{
    Node *p;
    while (head != NULL) {
        p = head;
        head = head->next;
        delete p;
    }
}
// copy constructor
OnlineDating::OnlineDating(const OnlineDating& src)
{
    if (src.head == NULL)
        head = NULL;
    else {
        // copy m_nMatches
        m_nMatches = src.m_nMatches;
        // allocate and fill new head node
        head = new Node;
        head->firstName = src.head->firstName;
        head->lastName = src.head->lastName;
        head->value = src.head->value;
        head->prev = NULL;
        
        Node *new_node = head;
        Node *src_node = src.head->next;
        while (src_node != NULL)
        {
            // allocate and fill new node
            new_node->next = new Node;
            new_node->next->firstName = src_node->firstName;
            new_node->next->lastName = src_node->lastName;
            new_node->next->value = src_node->value;
            
            // link new node
            new_node->next->prev = new_node;
            new_node->next->next = NULL;
            
            // increment pointers
            src_node = src_node->next;
            new_node = new_node->next;
        }
    }
}
// assignment operator
const OnlineDating& OnlineDating::operator=(const OnlineDating& src)
{
    if (this == &src)
        return *this;
    
    // Free memory currently held
    Node *p = head;
    while (p != NULL)
    {
        Node *n = p->next;
        delete p;
        p = n;
    }
    
    // Determine memory, allocate memory, copy content
    if (src.head == NULL)
        head = NULL;
    else
    {
      // copy m_nMatches
        m_nMatches = src.m_nMatches;
      // allocate and fill new head node
        head = new Node;
        head->firstName = src.head->firstName;
        head->lastName = src.head->lastName;
        head->value = src.head->value;
        head->prev = NULL;
        
        Node *new_node = head;
        Node *src_node = src.head->next;
        while (src_node != NULL)
        {
            // allocate and fill new node
            new_node->next = new Node;
            new_node->next->firstName = src_node->firstName;
            new_node->next->lastName = src_node->lastName;
            new_node->next->value = src_node->value;
            
            // link new node
            new_node->next->prev = new_node;
            new_node->next->next = NULL;
            
            // increment pointers
            src_node = src_node->next;
            new_node = new_node->next;
        }
    }
    // Return a reference to the target variable
    return *this;
}

// add a node to the list
bool OnlineDating::makeMatch(const std::string& firstName, const std::string&
               lastName, const OnlineType& value)
{

    // if the full name has already been added, return false
    if (someoneAmongMatches(firstName, lastName))
        return false;
    
    // allocate new node and set its values
    ++m_nMatches;
    Node *new_node = new Node;
    new_node->firstName = firstName;
    new_node->lastName = lastName;
    new_node->value = value;

    // if the list is empty or the node should go first, insert to front
    if (head == NULL || lastName < head->lastName || (lastName == head->lastName && firstName < head->firstName))
    {
        // insert to front
        new_node->next = head;
        if (head != NULL)
            head->prev = new_node;
        new_node->prev = NULL;
        head = new_node;
        return true;
    }
    
    // if it should be inserted at the end, then put it at the end
    Node *c = head;
    while (c->next != NULL)         // increment pointer to last node
        c = c->next;
    if (lastName > c->lastName || (lastName == c->lastName && firstName > c->firstName))
    {
        // insert to back
        new_node->prev = c;
        c->next = new_node;
        new_node->next = NULL;
        return true;
    }
    
    // find insertion point between two nodes
    Node *b = head;
    while (b->next != NULL)
    {
        // if this node is meant to be between b and b->next
        if ((lastName == b->lastName && lastName == b->next->lastName && firstName < b->next->firstName)    // case 1
            || (lastName == b->lastName && firstName > b->firstName && lastName < b->next->lastName)        // case 2
            || (lastName > b->lastName && lastName == b->next->lastName && firstName < b->next->firstName)  // case 3
            || (lastName > b->lastName && lastName < b->next->lastName))                                    // case 4
            break;
        b = b->next;
    }
    
    // linking new node to surrounding nodes
    
    new_node->next = b->next;
    b->next = new_node;
    new_node->prev = b;
    if (new_node->next != NULL)
        new_node->next->prev = new_node;
    
    return true;
}

bool OnlineDating::transformMatch(const std::string& firstName, const
                    std::string& lastName, const OnlineType & value)

{
    Node *p = head;
    while (p != NULL)
    {
        if (firstName == p->firstName && lastName == p->lastName)
            break;
        p = p->next;
    }
    // p is pointing to the node with the same full name as the given parameter
    if (p != NULL)              // check that loop didn't traverse past end of list (and the list wasn't empty)
    {
        p->value = value;
        return true;
    }
    return false;
}

bool OnlineDating::makeOrTransform(const std::string& firstName, const
                     std::string& lastName, const OnlineType& value)
{
    // transformMatch if there was a match, makeMatch if there was no match
    if (someoneAmongMatches(firstName, lastName))
        transformMatch(firstName, lastName, value);
    else
        makeMatch(firstName, lastName, value);
    
    return true;
}

bool OnlineDating::blockPreviousMatch(const std::string& firstName, const
                        std::string& lastName)
{
    // if there was no full name match, return false
    if (!someoneAmongMatches(firstName, lastName))
        return false;
    
    --m_nMatches;
    // if the full name matches the head
    if (firstName == head->firstName && lastName == head->lastName)
    {
        Node *n = head->next;                               // make temp for deletion
        // check that the list wasn't just one node
        if (n != NULL)
            n->prev = NULL;
        delete head;                                        // free memory
        head = n;
        return true;
    }
    
    // increments either to the last node OR to the first matching node
    Node *p = head;
    while (p->next != NULL && (firstName != p->firstName || lastName != p->lastName))
        p = p->next;
    // if the tail value matches the full name
    if (p->next == NULL && (firstName == p->firstName && lastName == p->lastName))
        p->prev->next = NULL;
    // if a middle value matches the first name
    else
    {
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }
    
    // free memory and return true
    delete p;
    return true;
}

bool OnlineDating::someoneAmongMatches(const std::string& firstName, const
                         std::string& lastName) const
{
    Node *p = head;
    while (p != NULL && ((firstName != p->firstName || lastName != p->lastName)))
        p = p->next;
    
    // if p = NULL, then the full list has been traversed and there were no full name matches
    // this will return true if p!=NULL, or if the loop broke because the first and last names matched
    return (p != NULL);
}

bool OnlineDating::lookAtMatches(const std::string& firstName, const
                   std::string& lastName, OnlineType& value) const
{
   if (!someoneAmongMatches(firstName, lastName))
       return false;

    Node *p = head;
    while (p != NULL && (firstName != p->firstName || lastName != p->lastName))
        p = p->next;
    
    value = p->value;
    return true;
}

bool OnlineDating::confirmMatch(int i, std::string& firstName, std::string&
                  lastName, OnlineType & value) const
{
    if (i < 0 || i >= m_nMatches)
        return false;

    // traverse the list until p points to item i
    Node *p = head;
    for (int k = 0; k < i; ++k)
        p = p->next;
    
    firstName = p->firstName;
    lastName = p->lastName;
    value = p->value;
    return true;
}

void OnlineDating::tradeMatches(OnlineDating& other)
{
    Node* temp = other.head;
    other.head = head;
    head = temp;
    
    int n = m_nMatches;
    m_nMatches = other.m_nMatches;
    other.m_nMatches = n;
    
    return;
}


bool mergeMatches(const OnlineDating & odOne,
                  const OnlineDating & odTwo,
                  OnlineDating & odJoined)
{
    bool returnValue = true;
    OnlineDating listCombo;
    OnlineDating badMatch;
    OnlineDating goodMatch;

    // create strings to be filled using confirmMatch
    string first;
    string last;
    string val;
    
    // fills badMatch with any matching pairs that have diff corresponding values
    // fills goodMatch with any matching pairs that have matching corresponding values
    for (int k = 0; k < odTwo.howManyMatches(); ++k)
    {
        // retrieve the values of odTwo at position k
        odTwo.confirmMatch(k, first, last, val);
        
        string checkVal;
        // checks if the full name in the kth node of odTwo matches any full name in odOne
        if (odOne.lookAtMatches(first, last, checkVal))
        {
            // If a full name appears in both odOne and odTwo, with the same corresponding value in
            // both, then odJoined must contain an element with that full name and value.
            if (val == checkVal)                           // compares val of odOne to odTwo
                goodMatch.makeMatch(first, last, val);
            // If there exists a full name that appears in both odOne and odTwo, but with different
            // corresponding values, then this function returns false;
            else
            {
                badMatch.makeMatch(first, last, val);
                returnValue = false;
            }
        }
    }
    
    // fill listCombo with odOne values
    for (int i = 0; i < odOne.howManyMatches(); ++i)
    {
        // retrieve the values of odOne at position i
        odOne.confirmMatch(i, first, last, val);
        
        // fill listCombo with every instance of odOne that isn't a bad match
        if (!badMatch.someoneAmongMatches(first, last))
            listCombo.makeMatch(first, last, val);
    }
    
    // fill listCombo with odTwo values
    for (int i = 0; i < odTwo.howManyMatches(); ++i)
    {
        // retrieve the values of odTwo at position i
        odTwo.confirmMatch(i, first, last, val);
        
        // fill listCombo with every instance of odOne that isn't a bad match or a good match
        if (!badMatch.someoneAmongMatches(first, last) && !goodMatch.someoneAmongMatches(first, last))
            listCombo.makeMatch(first, last, val);
    }
    
    // Exchange the contents of listCombo with odJoined
    listCombo.tradeMatches(odJoined);
    return returnValue;
}

void authenticateMatches (const std::string& fsearch,
                          const std::string& lsearch,
                          const OnlineDating& odOne,
                          OnlineDating& odResult)
{
    // create an empty list
    OnlineDating list;
    
    // if both search terms are "wildcards," replace odResult with a copy of odOne
    if (fsearch == "*" && lsearch == "*")
    {
        list = odOne;                       // calls assignment operator
    }
    else
    {
        for (int i = 0; i < odOne.howManyMatches(); ++ i)
        {
            // retrieve the values of odOne at position i
            string first;
            string last;
            string val;
            odOne.confirmMatch(i, first, last, val);
        
            if (fsearch == "*")
            {
                if (last == lsearch)
                    list.makeMatch(first, last, val);
            }
            else if (lsearch == "*")
            {
                if (first == fsearch)
                    list.makeMatch(first, last, val);
            }
            else
            {
                if (first == fsearch && last == lsearch)
                    list.makeMatch(first, last, val);
            }
        }
    }
    // swap the empty list with odResult
    odResult.tradeMatches(list);
}


