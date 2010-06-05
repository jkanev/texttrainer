
#ifndef PHRASE_H
#define PHRASE_H

#include "word.h"
#include "meaning.h"

class Word;
class Meaning;

using namespace std;

typedef unsigned int uint;

class Phrase
{
public:
	Phrase(int firstWord, int lastWord, Meaning *meaning) {
		phraseFirstWord = firstWord;
		phraseLastWord = lastWord;
		phraseMeaning = meaning;
	}
	
	~Phrase() {}
	
	uint getFirstWordIndex() {
		return phraseFirstWord;
	}
	uint getLastWordIndex() {
		return phraseLastWord;
	}
	int size() {
		return phraseLastWord - phraseFirstWord + 1;
	}
	Meaning *getMeaning() {
		return phraseMeaning;
	}

private:
	uint phraseFirstWord;
	uint phraseLastWord;
	Meaning *phraseMeaning;
};


#endif

