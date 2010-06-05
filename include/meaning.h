
#ifndef MEANING_H
#define MEANING_H

#include "word.h"
#include "phrase.h"

#include <QStringList>
#include <QString>
#include <vector>

using namespace std;

class Word;
class Phrase;

/// Container for different translations
class Meaning
{
	public:
		Meaning();
		Meaning(QStringList alternatives);
		Meaning( const Meaning &m );
		~Meaning();
		void edit(QString newContent);
		QString operator[](const uint &n) const;
		bool operator==(const Meaning &m) const;
		void findPhrases(vector<Word*> *words, vector<Phrase *> *phrases);
		QString toDisplayString();
		QString toSaveString();
		int size();
		const Meaning &operator+=(const Meaning &h);
		
		// flags
		bool isAutomatic();
		Meaning &setAutomatic(bool);
		bool hasChanged();
		Meaning &setChanged(bool);
		bool isOnTheFly();
		Meaning &setOnTheFly(bool);
		
		// edit from text display
		QString changeUsingDisplayString(QString displayString);
		int getNextEditablePosition( int position );
		
	private:
		QString meaningDisplayDelimiter;
		vector<QString> meaningTranslations;
		bool meaningIsAutomatic;
		bool meaningIsOnTheFly;
		bool meaningHasChanged;
};

#endif
