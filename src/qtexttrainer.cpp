
#include <QApplication>
#include <QTranslator>
#include <QFileDialog>
#include <QRegExp>
#include <QStringList>
#include <QString>
#include <QFile>
#include <QTextBrowser>
#include <QTextStream>
#include <QTextCodec>
#include <QComboBox>

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <queue>
#include <vector>
#include <utility>

#include "../include/qtexttrainer.h"


TextTrainer::TextTrainer(QApplication &parent)
	: QMainWindow(), trainerSettings("TextTrainer", "TextTrainer")
{
	// load language file
QTranslator translator;
	QString locale
			= QString("texttrainer_")
			+ QLocale::system().name().replace( QRegExp("_.*"), "");
	QString i18nDir = trainerSettings.value("i18n").toString();
	translator.load(locale, i18nDir);
	parent.installTranslator(&translator);
	
	// translate help texts
	helpTexts.init();
	
	// init all variables
	trainerCurrentMeaning = 0;
	trainerIgnoresMessages = false;
	trainerMode = DISPLAY;
	setupUi(this);
	setLearningCurve(0);
	setMode(NO_FILE);
}

TextTrainer::~TextTrainer()
{
	for (uint i=0; i<trainerWordList.size(); ++i)
		delete trainerWordList[i];
	for (uint i=0; i<trainerPhraseList.size(); ++i)
		delete trainerPhraseList[i];
	for (uint i=0; i<trainerMeaningList.size(); ++i)
		delete trainerMeaningList[i];
}


//______________________________________________________________
// clear all

void TextTrainer::setMode(EditMode m)
{
	// if no change, do nothing
	if (trainerMode == m)
		return;
	
	// highlight first element and show text
	listProgress->setCurrentIndex(0);
	
	// store results of old mode
	switch(trainerMode) {
		case NO_FILE:
		case DISPLAY: 
			break;
		case EDIT_TEXT: {
			// read text into wordList
			trainerCharToWordTable.clear();
			trainerWordList.clear();
			QStringList lines = textMain->toPlainText().split("\n");
			for (int i=0; i<lines.size(); ++i)
				readLine( lines[i] );
			// re-match translations
			initPhrases();
			// re-display
			displayText(0);
			break;
		}
		case EDIT_TRANSLATIONS:
			// save old hint if necessary
			clearCurrentMeaning();
			break;
	}
	
	// set new mode
	trainerMode = m;
	initPhrases();
	displayText(0);
	
	// set gui
	switch(trainerMode) {
		case DISPLAY:
			setGui(false, false, true, false, false, true);
			labelHelp->setText ( helpTexts.modeDisplay );
			break;
		case EDIT_TEXT:
			setGui(true, false, false, true, false, false);
			labelHelp->setText( helpTexts.modeEditText + helpTexts.gotoModeDisplay );
			break;
		case EDIT_TRANSLATIONS:
			setGui(false, true, false, false, true, false);
			labelHelp->setText( helpTexts.modeEditTranslations + helpTexts.gotoModeDisplay );
			break;
		case NO_FILE:
			setGui(false, false, true, false, false, false);
			labelHelp->setText( helpTexts.modeNoFile );
			break;
	}
}


//______________________________________________________________
// clear all

void TextTrainer::setGui( bool writeMain, bool writeHint, bool file, bool editText, bool editTranslations, bool learningWidgets )
{
	// ro/rw mode of main views
	textMain->setReadOnly(!writeMain);
	textHint->setReadOnly(!writeHint);
	
	menuBar()->clear();
	
	// file menu and tool bar
	if (file) {
		toolbarFile->show();
		menuBar()->addMenu(menuFile);
	} else {
		toolbarFile->hide();
	}
	
	// translation menu and tool bar
	if (editTranslations) {
		toolbarEditTranslations->show();
		menuBar()->addMenu(menuEditTranslations);
	} else {
		toolbarEditTranslations->hide();
	}
	
	// edit text tool bar and menu
	if (editText) {
		toolbarEditText->show();
		menuBar()->addMenu(menuEditText);
	} else {
		toolbarEditText->hide();
	}
	
	menuBar()->addMenu(menuHelp);
	
	// learning helpers
	if (learningWidgets) {
		listLearningCurve->show();
		labelLearningCurve->show();
		listProgress->show();
		labelProgress->show();
		pushRepeatStep->show();
		pushNextStep->show();
	} else {
		listLearningCurve->hide();
		labelLearningCurve->hide();
		listProgress->hide();
		labelProgress->hide();
		pushRepeatStep->hide();
		pushNextStep->hide();
	}
}

//______________________________________________________________
// clear all

void TextTrainer::clear()
{
	for (uint i=0; i<trainerWordList.size(); ++i)
		delete trainerWordList[i];
	trainerWordList.clear();
	for (uint i=0; i<trainerMeaningList.size(); ++i)
		delete trainerMeaningList[i];
	trainerMeaningList.clear();
	for (uint i=0; i<trainerPhraseList.size(); ++i)
		delete trainerPhraseList[i];
	trainerPhraseList.clear();
	
	textMain->setText("");
	textHint->setText("");
	
	setMode(NO_FILE);
}

//______________________________________________________________
// open file

void TextTrainer::openTextFile()
{
	clear();
	
	// get filename from user
	QString fileName = QFileDialog::getOpenFileName(
		this,
		"Open Text File",
		trainerSettings.value("LastText").toString(),
		"All files (*)" );
	
	// if file open dialog returns OK
	QFile file( fileName );
	if ( file.open( QIODevice::ReadOnly ) ) {
		
		// read file
		QTextStream stream( &file );
		stream.setCodec( QTextCodec::codecForName("UTF-8") );
		while ( !stream.atEnd() )
			readLine( stream.readLine() );
		file.close();
		
		// save name and dir in settings
		trainerSettings.setValue("LastText", fileName);
	}

	// initialise phrases
	initPhrases();
	
	// display text	
	if (trainerCurveIndex)
		listProgress->setCurrentIndex(0);
	else
		displayCurrentText();
		
	// set gui
	setMode(DISPLAY);
}


//______________________________________________________________
// read a text or a meaning line

void TextTrainer::readLine(QString line)
{
	if (line.contains('=')) {
		// if translation line, create meaning and add
		QStringList wordList;
		wordList = line.trimmed().split( QRegExp(" = ")  );
		if (wordList.size()>1) {
			QStringList translations( wordList[0] );
			translations += wordList[1].split( QRegExp(" \\| ") );
		
			bool exists = false;
			Meaning *meaning = new Meaning(translations);
			for (uint i=0; i<trainerMeaningList.size(); ++i)
				if (*meaning == *trainerMeaningList[i]) {
					exists = true;
					delete meaning;
					break;
				}
			if (!exists) {
				meaning->setAutomatic(false).setOnTheFly(false);
				trainerMeaningList.push_back( meaning );
			}
		}
		
	} else {
		
		// otherwise, read text line and add words
		QStringList wordList = line.trimmed().split( QRegExp("\\s") );
		if (!wordList.size() && trainerWordList.size()) {
			trainerWordList.back()->setEndOfParagraph();
		} else {
			for (int i=0; i<wordList.size(); ++i)
				trainerWordList.push_back( new Word(wordList[i]) );
			trainerWordList.back()->setEndOfLine();
		}
	}
}

//______________________________________________________________
// open translation file

void TextTrainer::openTranslationFile()
{
	// user chooses file
	QString fileName = QFileDialog::getOpenFileName (
		this,
		"open translation file dialog",
		".",
		"All files (*)"  );
	
	// read translations
	QFile file( fileName );
	if ( file.open( QIODevice::ReadOnly ) ) {
		QTextStream stream( &file );
		while ( !stream.atEnd() ) {
			QString line;
			line = stream.readLine();	
			if (line.contains("="))
				readLine(line);
		}
		file.close();
	}
	
	// initialise phrases
	initPhrases();
	
	// re-generate display
	displayCurrentText();
}

//______________________________________________________________
// save a text file

void TextTrainer::saveTextFile(bool useCurrentPath)
{
	if (!useCurrentPath)
		// get filename from user
		trainerTextFileName = QFileDialog::getSaveFileName(
			this,
			"Save Text File",
			".",
			"All files (*)" );
			
	// open file
	QFile file( trainerTextFileName );
	if ( file.open( QIODevice::ReadWrite ) ) {
		
		// store all files as utf8
		QTextStream stream( &file );
		stream.setCodec( QTextCodec::codecForName("UTF-8") );
		
		// append current text
		stream << createText(false, 0) << endl << endl;
		
		for (uint i=0; i<trainerMeaningList.size(); ++i)
			stream << trainerMeaningList[i]->toSaveString();
	}
}

//______________________________________________________________
// create phrases and meanings

void TextTrainer::initPhrases()
{
	// clean up first
	for (uint i=0; i<trainerPhraseList.size(); ++i)
		delete trainerPhraseList[i];
	trainerPhraseList.clear();
	for (uint i=0; i<trainerWordList.size(); ++i)
		trainerWordList[i]->removeFromPhrases();
	
	// remove automatic meanings
	for (uint i=trainerMeaningList.size()-1; i+1; --i) 
		if (trainerMeaningList[i]->isAutomatic())
			trainerMeaningList.erase( trainerMeaningList.begin()+i );

	// find phrases for all meanings
	for (uint i=0; i<trainerMeaningList.size(); ++i)
		trainerMeaningList[i]->findPhrases( &trainerWordList, &trainerPhraseList );
	
	// create automatic phrases and meanings for all meaningless words
	for (uint i=0; i<trainerWordList.size(); ++i) {
		if (!trainerWordList[i]->getPhraseAmount()) {
			QString word = trainerWordList[i]->getContent();
			Meaning *meaning = new Meaning( QStringList(word) );
			meaning->findPhrases( &trainerWordList, &trainerPhraseList );
			meaning->setOnTheFly(false).setAutomatic(true);
			trainerMeaningList.push_back(meaning);
		}
	}
	
}

//______________________________________________________________
/// populate list view

void TextTrainer::setLearningCurve(int n)
{
	// clear listbox
	listProgress->clear();
	trainerLearningCurve.clear();
	
	// determine learning curve
	int steps = 0, function = 0;
	switch (n) {
		case 0:
			steps = 10;
			function = -1;
			break;
		case 1:
			steps = 20;
			function = -1;
			break;
		case 2:
			steps = 50;
			function = -1;
			break;
		case 3:
			steps = 10;
			break;
		case 4:
			steps = 20;
			break;
		case 5:
			steps = 50;
			break;
		case 6:
			steps = 10;
			function = 1;
			break;
		case 7:
			steps = 20;
			function = 1;
			break;
		case 8:
			steps = 50;
			function = 1;
			break;
	}
	
	// calculate learning steps and build string list
	QStringList entries;
	double val = 1.0;
	for (int i=0; i<steps; i++, val-=1.0/double(steps-1)) {
		double newVal = 0.0;
		switch (function) {
			case -1:
				newVal = 0.5*sqrt(val) + val - 0.5*val*val;
				break;
			case 0:
				newVal = val;
				break;
			case 1:
				newVal = - 0.5*sqrt(1.0-val) + val + 0.5*(1.0-val)*(1.0-val);
				break;
		}
		newVal = val<0.0 ? 0.0 : newVal; // newVal can be less than 0 due to impressision
		trainerLearningCurve.push_back(round(100.0*newVal)/100.0);
		stringstream text;
		text << uint(round(100.0*(1.0 - newVal))) << "%";
		entries << text.str().c_str();
	}
	listProgress->insertItems(0, entries);
	listProgress->setCurrentIndex(0); // highlight first element and show text
}


//______________________________________________________________
// populate list view

Meaning TextTrainer::createHint(uint firstWord, uint lastWord)
{
	// return emty string if no solution found
	Meaning m;

	// start only when something is loaded
	if (trainerWordList.size()) {
	
		// queue for h*-algorithm: solution is a vector with start and end words
		typedef vector<Phrase *> PhraseList;
		deque< PhraseList > phraseQueue;
		
		// insert empty solution
		phraseQueue.push_back( vector<Phrase *>() );
		
		// while first solution is incomplete
		while ( phraseQueue.size()
			&& (!phraseQueue.front().size()
			|| phraseQueue.front().front()->getFirstWordIndex() != firstWord
			|| phraseQueue.front().back()->getLastWordIndex() != lastWord) ) {
		
			// take first solution out of queue
			PhraseList solution = phraseQueue.front();
			phraseQueue.pop_front();
			
			// build feasible expansions
			PhraseList nextPhrases;
			uint start;
			if (solution.size())
				start = solution.back()->getLastWordIndex() + 1; // one after last word of last phrase
			else start = firstWord;
			
			if (start > lastWord)
				continue;
			for (uint i=0; i<trainerWordList[start]->getPhraseAmount(); ++i) {
				Phrase *candidate = trainerWordList[start]->getPhrase(i);
				if (candidate->getFirstWordIndex() == start
						&& candidate->getLastWordIndex() <= lastWord)
					nextPhrases.push_back( candidate );
			}
			
			// for each possible expansion
			for (uint i=0; i<nextPhrases.size(); ++i) {
				
				// expand first solution
				PhraseList nextSolution = solution;
				nextSolution.push_back( nextPhrases[i] );
					
				// calculate cost of expanded solution
				uint wordAmountNewEntry
					= nextSolution.back()->getLastWordIndex()
						- nextSolution.front()->getFirstWordIndex();
				double costNewEntry = double(nextSolution.size()) / double(wordAmountNewEntry);
				
				// insert expanded solution according to cost
				bool inserted = false;
				for (uint j=0; j<phraseQueue.size(); j++) {
					uint wordAmount
						= phraseQueue[j].back()->getLastWordIndex()
							- phraseQueue[j].front()->getFirstWordIndex();
					double cost = double(phraseQueue[j].size()) / double(wordAmount);;
					if (cost > costNewEntry) {
						phraseQueue.insert(phraseQueue.begin() + j, nextSolution);
						inserted = true;
						break;
					}
				}
				if (!inserted)
					phraseQueue.push_back(nextSolution);
			}
		}
		
		
		// create string from phrase list
		if (trainerWordList.size() && phraseQueue.size()) {
			PhraseList solution = phraseQueue.front();
			if (solution.size() == 1)
				m = *solution[0]->getMeaning();
			else 
				for (uint i=0; i<solution.size(); ++i)
					m += *solution[i]->getMeaning();
		}
		
		m.setChanged(false);
	}
	
	return m;
}


//______________________________________________________________
// create a display text

QString TextTrainer::createText(bool hideWords, vector<uint> *charTable) const
{
	// build string and position matrix
	QString displayText = "";
	if (charTable)
		charTable->clear();
	
	for (uint i=0; i<trainerWordList.size(); i++) {
	
		// record word content
		QString word  = trainerWordList[i]->getContent();
		if (hideWords && !trainerWordList[i]->isVisible() && word != "")
			word = "__";
		
		// create appropriate space
		QString space;
		if (trainerWordList[i]->isEndOfParagraph()) {
			space = "\n\n";
		} else if (trainerWordList[i]->isEndline()) {
			space = "\n";
		} else if ( hideWords
					&& !trainerWordList[i]->isVisible()
					&& i+1<trainerWordList.size()
					&& !trainerWordList[i+1]->isVisible() ) {
				space = "";
		} else {
			space = " ";
		}
		
		// collect opening and closing brackets
		QString openingBrackets("");
		QString closingBrackets("");
		if (trainerMode == EDIT_TRANSLATIONS) {
			bool automatic = trainerWordList[i]->getPhrase(0)->getMeaning()->isAutomatic();
			for (uint b = automatic ? 1 : 0; b<trainerWordList[i]->getPhraseBeginnings(); ++b)
				openingBrackets += "[";
			for (uint e = automatic ? 1 : 0; e<trainerWordList[i]->getPhraseEndings(); ++e)
				closingBrackets += "]";
		}
		
		// assemble word
		word = openingBrackets + word + closingBrackets + space;
		
		// record text position
		if (charTable)
			for (int c=0; c<word.length(); c++)
				charTable->push_back(i);
		
		displayText += word;
	}
	return displayText;	
}


//______________________________________________________________
// get settings for next gap view and display

void TextTrainer::displayNextText()
{
	// get current item
	uint n = (uint)listProgress->currentIndex();
	if (n < trainerLearningCurve.size()-1)
		n++;
	
	// highlight next item (triggers next view automatically.
	listProgress->setCurrentIndex(n);
}


//______________________________________________________________
// display nth gap view 

void TextTrainer::displayText(int n)
{
	trainerCurveIndex = n;
	displayCurrentText();
}


//______________________________________________________________
// display gap view with current settings

void TextTrainer::displayCurrentText()
{
	// hide/show words
	for (uint i=0; i<trainerWordList.size(); i++) {
		double randValue = double(rand()) / double(RAND_MAX);
		if (randValue < trainerLearningCurve[trainerCurveIndex])
			trainerWordList[i]->setVisible();
		else
			trainerWordList[i]->setHidden();
	}
	
	// create and display string
	textMain->setText( createText(true, &trainerCharToWordTable) );
}


//______________________________________________________________
// clears and possibly saves the current meaning

void TextTrainer::clearCurrentMeaning()
{
	if (trainerCurrentMeaning) {
		if ( trainerCurrentMeaning->isOnTheFly() ) {
			if ( trainerCurrentMeaning->hasChanged() ) {
				trainerCurrentMeaning->setOnTheFly(false).setChanged(false).setAutomatic(false);
				trainerCurrentMeaning->findPhrases( &trainerWordList, &trainerPhraseList );
				trainerMeaningList.push_back(trainerCurrentMeaning);
				displayText(0);
			} else {
				delete trainerCurrentMeaning;
			}
		} else {
			if ( trainerCurrentMeaning->hasChanged() ) {
				trainerCurrentMeaning->setChanged(false).setAutomatic(false);
				displayText(0);
			}
		}
		trainerCurrentMeaning = 0;
	}
}


//______________________________________________________________
// a word has been clicked

void TextTrainer::displayHint(int firstWord, int lastWord)
{
	if (firstWord == -1 || lastWord == -1)
		return;
		
	// if word is hidden, find whole group of hidden words
	uint groupBegin = (uint)firstWord;
	uint groupEnd = (uint)lastWord;
	
	// find first hidden word to the left
	for ( int i=firstWord;
		i+1 && !trainerWordList[i]->isVisible()
			&& (i==firstWord || !trainerWordList[i]->isEndline());
		--i )
		groupBegin = i;
	
	// find last hidden word to the right
	for ( int i=lastWord;
		i<(int)trainerWordList.size() 
			&& !trainerWordList[i]->isVisible()
			&& (i==lastWord || !i || !trainerWordList[i-1]->isEndline());
		++i )
		groupEnd = i;
	
	switch (trainerMode) {
		case EDIT_TEXT:
		case DISPLAY:
			textHint->setText( createHint(groupBegin, groupEnd).toDisplayString() );
			break;
			
		case EDIT_TRANSLATIONS: {
			
			// save old hint if necessary
			clearCurrentMeaning();
			
			// does hint exist
			Phrase *phrase = 0;
			Word *word = trainerWordList[groupBegin];
			for (uint i=0; !phrase && i<word->getPhraseAmount(); ++i)
				if (word->getPhrase(i)->getLastWordIndex() == groupEnd)
					phrase = word->getPhrase(i);
			
			// if yes, set to current
			if (phrase)
				trainerCurrentMeaning = phrase->getMeaning();
			
			// if no, create new automatic meaning and set to current
			else 
				trainerCurrentMeaning = new Meaning( createHint(groupBegin, groupEnd) );
			
			textHint->setText( trainerCurrentMeaning->toDisplayString() );
		}
		
		case NO_FILE:
		default:
			break;
	}	

}

 
//______________________________________________________________
// help texts

void HelpTexts::init()
{
		modeNoFile = tr("To start, you must open a file (use button \"Open Text File\") or start a new text (use button \"Edit Text\"). ");
		modeDisplay = tr("Click on a word in the \"Main Text\" area to get hints or translations. Use the \"Repeat Step\" button to hide different words or the \"Next Step\" button to hide more words. ");
		modeEditText = tr("Edit your text in the \"Main Text\" area. ");
		modeEditTranslations = tr("Select a word or phrase in the \"Main Text\" area, and edit translations in the \"Hint\" area. With the \"Arrow Up/Down\" key you select different translations. Brackets (\"[\" and \"]\") around words or phrases indicate existing translations. ");
		gotoModeDisplay = tr("Use the \"Display\" button to go back to normal mode. ");
		gotoModeEditText = tr("Use the \"Edit Text\" button to edit a text. ");
		gotoModeEditTranslations = tr("Use the \"Edit Translations\" button to add or change translations. ");
};


//______________________________________________________________
// main function

int main( int argc, char *argv[] ) 
{
	// create application and main window
	QApplication app( argc, argv );
	TextTrainer main( app );
	main.show();
	app.exec();
	
	return 0;
}
