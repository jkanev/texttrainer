
#ifndef WORD_H
#define WORD_H

#include "phrase.h"

#include <vector>
#include <qstring.h>

class Phrase;

using namespace std;

class Word
{
	public:
		Word( QString content, bool endline = false, bool endofparagraph  = false );
		~Word();
		
		/// sets the word to be visible on the display
		void setVisible();
		
		/// sets the word to be replaced by a _ on the display
		void setHidden();
		
		/// sets the word to be followed by two newlines
		void setEndOfParagraph();
		
		/// sets the word to be followed by one newline
		void setEndOfLine();
		
		/// is the word visible on the display?
		bool isVisible();
		
		/// is the word followed by a newline?
		bool isEndline();
		
		/// is the word followed by two newlines?
		bool isEndOfParagraph();
		QString getContent();
		void setHintIndex(uint n);
		uint getHintIndex();
		uint getPhraseAmount();
		void addToPhrase( Phrase *phrase );
		void removeFromPhrases();
		Phrase *getPhrase(uint n);
		uint getPhraseBeginnings();
		uint getPhraseEndings();
		void addPhraseEnding();
		void removePhraseEnding();
		void removeFromPhrase( Phrase *phrase );
	
	private:
		bool wordIsVisible;
		bool wordIsEndline;
		bool wordIsEndOfParagraph;
		vector<Phrase *> wordPhraseList;
		uint wordPhraseBeginnings;
		uint wordPhraseEndings;
		QString wordContent;
		uint nHintIndex;
};

#endif

