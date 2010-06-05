
#include "../include/word.h"

#include <qstringlist.h>
#include <iostream>

//______________________________________________________________________
//

Word::Word( QString content, bool endline, bool endofparagraph )
{
	wordContent = content;
	setVisible();
	wordIsEndline = endline;
	wordIsEndOfParagraph = endofparagraph;
	wordPhraseBeginnings = 0;
	wordPhraseEndings = 0;
}
		
//______________________________________________________________________
//

Word::~Word() {}

//______________________________________________________________________
//   add another phrase ending to this word

void Word::addPhraseEnding()
{
	wordPhraseEndings++;
}

//______________________________________________________________________
//

uint Word::getPhraseBeginnings()
{
	return wordPhraseBeginnings;
}

//______________________________________________________________________
//

uint Word::getPhraseEndings()
{
	return wordPhraseEndings;
}

//______________________________________________________________________
//

void Word::setVisible()
{
	wordIsVisible = true;
}

//______________________________________________________________________
//

void Word::setHidden()
{
	wordIsVisible = false;
}


//______________________________________________________________________
//

void Word::setEndOfParagraph()
{
	wordIsEndOfParagraph = true;
}

//______________________________________________________________________
//

void Word::setEndOfLine()
{
	wordIsEndline = true;
}

//______________________________________________________________________
//

bool Word::isVisible()
{
	return wordIsVisible;
}

//______________________________________________________________________
//

bool Word::isEndline()
{
	return wordIsEndline;
}

//______________________________________________________________________
//

bool Word::isEndOfParagraph()
{
	return wordIsEndOfParagraph;
}

//______________________________________________________________________
//
uint Word::getPhraseAmount()
{
	return wordPhraseList.size();
}

//______________________________________________________________________
//

Phrase *Word::getPhrase(uint n)
{
	return wordPhraseList[n];
}

//______________________________________________________________________
//

void Word::setHintIndex(uint n)
{
	nHintIndex = n;
}

//______________________________________________________________________
//

uint Word::getHintIndex()
{
	return nHintIndex;
}

//______________________________________________________________________
//

QString Word::getContent()
{
	return wordContent;
}

//______________________________________________________________________
//

void Word::addToPhrase( Phrase *phrase )
{
	wordPhraseList.push_back(phrase);
	wordPhraseBeginnings++;
}

void Word::removeFromPhrase( Phrase *phrase )
{
	bool found = false;
	uint index;
	for (index=0; index<wordPhraseList.size(); ++index)
		if (wordPhraseList[index]==phrase) {
			found = true;
			break;
		}
		
	if (found) {
		wordPhraseList.erase( wordPhraseList.begin() + index );
		wordPhraseBeginnings--;
	}
}

void Word::removePhraseEnding()
{
	wordPhraseEndings--;
}

//______________________________________________________________________
//

void Word::removeFromPhrases()
{
	wordPhraseList.clear();
	wordPhraseBeginnings = 0;
	wordPhraseEndings = 0;
}
