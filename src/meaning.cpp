
#include "../include/meaning.h"

#include <QRegExp>
#include <iostream>

//______________________________________________________________________
// construct

Meaning::Meaning(QStringList translations)
{
	for (int i=0; i<translations.size(); ++i)
		meaningTranslations.push_back( translations[i].trimmed() );
	
	meaningDisplayDelimiter = "\n__________\n\n";
	meaningIsAutomatic = false;
	meaningIsOnTheFly = false;
	meaningHasChanged = false;
}

Meaning::Meaning()
{
	meaningDisplayDelimiter = "\n__________\n\n";
	meaningIsAutomatic = true;
	meaningIsOnTheFly = true;
	meaningHasChanged = false;
}

Meaning::Meaning( const Meaning &m )
{
	meaningDisplayDelimiter = "\n__________\n\n";
	meaningTranslations = m.meaningTranslations;
	meaningIsAutomatic = m.meaningIsAutomatic;
	meaningHasChanged = m.meaningHasChanged;
	meaningIsOnTheFly = m.meaningIsOnTheFly;
}


//______________________________________________________________________
// destruct

Meaning::~Meaning()
{
}


//______________________________________________________________________
// set / get flags

bool Meaning::isAutomatic()
{
	return meaningIsAutomatic;
}

Meaning &Meaning::setAutomatic( bool isAutomatic )
{
	meaningIsAutomatic = isAutomatic;
	return *this;
}

bool Meaning::hasChanged()
{
	return meaningHasChanged;
}

Meaning &Meaning::setChanged( bool hasChanged )
{
	meaningHasChanged = hasChanged;
	return *this;
}

bool Meaning::isOnTheFly()
{
	return meaningIsOnTheFly;
}

Meaning &Meaning::setOnTheFly( bool isOnTheFly )
{
	meaningIsOnTheFly = isOnTheFly;
	return *this;
}

//______________________________________________________________________
// get a translation

QString Meaning::operator[](const uint &n) const
{
	if (n<meaningTranslations.size())
		return meaningTranslations[n];
	else
		return meaningTranslations[0];
}


//______________________________________________________________________
// size

int Meaning::size()
{
	return meaningTranslations.size();
}


//______________________________________________________________________
// check for equality

bool Meaning::operator==(const Meaning &m) const
{
	return m[0] == meaningTranslations[0];
}
	

//______________________________________________________________________
// find phrases in a word list, create phrase objects for them, and link

 void Meaning::findPhrases(vector<Word *> *allWords, vector<Phrase *> *allPhrases)
{
	// split translation 0 into words
	QStringList phraseWords = meaningTranslations[0].split( QRegExp("\\s") );

	// match in given words
	int allWordsAmount = allWords->size();
	int phraseWordsAmount = phraseWords.size();
	for (int i=0; i<allWordsAmount - phraseWordsAmount + 1; ++i) {
		bool match = true;
		for (int j=0; j<phraseWordsAmount; ++j)
			if ((*allWords)[i+j]->getContent() != phraseWords[j]) {
				match = false;
				break;
			}
		
		// create phrase and connect words to phrase
		if (match) {
			Phrase *phrase = new Phrase(i, i+phraseWordsAmount-1, this); // empty phrase with size 0
			allPhrases->push_back( phrase );
			(*allWords)[i]->addToPhrase( phrase );
			(*allWords)[i+phraseWordsAmount-1]->addPhraseEnding();
		}
	}
}


//________________________________________________________________________________
// convert to string

QString Meaning::toDisplayString()
{
	QString s;
	if (meaningTranslations.size()>0) {
		s = meaningTranslations[0];
		for (uint i=1; i<meaningTranslations.size(); i++) {
			s += meaningDisplayDelimiter;
			s += meaningTranslations[i];
		}
	}
	return s;
};


//________________________________________________________________________________
// read from display string
		
QString Meaning::changeUsingDisplayString(QString displayString)
{
	// split according to display
	QStringList newContent = displayString.split(  QRegExp( "(" + meaningDisplayDelimiter + ")+|(\n)" )  );
	
	// remove empty ending
//	if (newContent.last() == "")
//		newContent.pop_back();
	
	// ignore not-allowed changes
	if (meaningTranslations[0] != newContent[0])
		return toDisplayString();
	for (int i=0; i<newContent.size(); ++i)
		if (newContent[i].contains("_"))
			return toDisplayString();
			
	// add new translations, skipping deleted ones
	uint m=0;
	for ( ; m<newContent.size(); ++m) {
		if (meaningTranslations.size() <= m)
			meaningTranslations.push_back(newContent[m]);
		else
			meaningTranslations[m] = newContent[m];
	}
	
	// remove old translations
	if (m<=meaningTranslations.size())
		meaningTranslations.erase(meaningTranslations.begin()+m, meaningTranslations.end());
		
	// set changed-flag
	meaningHasChanged = true;
	
	return toDisplayString();
}


//________________________________________________________________________________
// gets the next position in the display string which is editable

int Meaning::getNextEditablePosition( int currentPosition )
{	
	int checkPosition = 0; // the point we're currently looking at
	
	// for each translation,
	for (uint i=0; i<meaningTranslations.size(); ++i) {
	
		// start edit zone. if we've passed the cursor, return this point
		if (checkPosition>=currentPosition)
			return checkPosition ;
			
		// add edit zone length
		checkPosition += meaningTranslations[i].length();
			
		// end edit zone. if we've passed the cursor, the current position is okay
		if (i && checkPosition>=currentPosition)
			return currentPosition;
			
		// add non-edit zone length
		checkPosition += meaningDisplayDelimiter.length();
	}
	
	// we didn't meet the cursor - set it to the end of the text
	return checkPosition;
}


//________________________________________________________________________________
// convert to string

QString Meaning::toSaveString()
{
	QString s = "";
	if (meaningTranslations.size()>1) {
		s = meaningTranslations[0];
		s += " = " + meaningTranslations[1];
		for (uint i=2; i<meaningTranslations.size(); i++) {
			s += " | ";
			s += meaningTranslations[i];
		}
		s += "\n";
	}
	return s;
};


//________________________________________________________________________________
// append another meaning to this meaning. Translation lists are added, missing translations
// are filled with the original.

const Meaning &Meaning::operator+=(const Meaning &h)
{
	// add all new translations
	uint thisSize = meaningTranslations.size();
	uint newSize = h.meaningTranslations.size();
	uint allSize = max(thisSize, newSize);
	QString thisOriginal = thisSize ? meaningTranslations[0] : "";
	QString newOriginal = newSize ? '[' + h.meaningTranslations[0] + ']'  : "";
	for (uint i=0; i<allSize; i++) {
		// expand current list if necessary
		if (i >= thisSize) {
			meaningTranslations.push_back( thisOriginal );
			thisSize++;
		}
		
		// add translation i, or (if not present) original meaning
		QString whiteSpace = meaningTranslations[i].length() ? " " : "";
		if (i >= newSize)
			meaningTranslations[i] += whiteSpace + newOriginal;
		else
			meaningTranslations[i] += whiteSpace +  h.meaningTranslations[i];
	}
	
	return *this;
}


