
#ifdef Q_OS_ANDROID
#include "ui_texttrainer-android.h"
#else
#include "ui_texttrainer.h"
#endif

#include "word.h"

#include <QStringList>
#include <QSettings>
#include <QString>
#include <vector>
#include <iostream>

using namespace std;
/// help strings
class HelpTexts : public QObject
{
	Q_OBJECT
	
	public:
		QString modeNoFile;
		QString modeDisplay;
		QString modeEditText;
		QString modeEditTranslations;
		QString gotoModeDisplay;
		QString gotoModeEditText;
		QString gotoModeEditTranslations;
		
		HelpTexts() {}
		
		void init();
};

/// mode for the whole program
typedef enum {NO_FILE, DISPLAY, EDIT_TEXT, EDIT_TRANSLATIONS} EditMode;

class TextTrainer : public QMainWindow, public Ui::QTextTrainer
{
	Q_OBJECT
			
	public:
	
		TextTrainer(QApplication&);
		~TextTrainer();
		
	public slots:
	
        void on_actionModeDisplay_triggered() {
			setMode(DISPLAY);
		}
		
        void on_actionModeEditText_triggered() {
			setMode(EDIT_TEXT);
		}
		
        void on_actionModeEditTranslations_triggered() {
			setMode(EDIT_TRANSLATIONS);
		}
		
        void on_actionNew_triggered() {
			clear();
            setMode(EDIT_TEXT);
        }
		
        void on_actionSave_triggered() {
			saveTextFile();
		}
		
        void on_actionSaveAs_triggered() {
			saveTextFile(false);
		}
		
	/// the file open action is calling
        void on_actionOpenTextFile_triggered() {
			openTextFile();
        }
		
	/// the translation file open action is calling
        void on_actionOpenTranslationFile_triggered() {
			openTranslationFile();
        }
		
	/// the "show next" button has been released
	void on_pushNextStep_released() {
		displayNextText();
        }
		
	/// the "repeat" button is released
	void on_pushRepeatStep_released() {
		displayCurrentText();
	};

        /// the "hide hints" button is released
        void on_pushHideHints_released() {
		textHint->hide();
#ifdef Q_OS_ANDROID
		pushHideHints->hide();
#endif
        };
		
        void on_actionIncreaseFont_triggered() {
		changeFontSize(1);
        }

        void on_actionDecreaseFont_triggered() {
		changeFontSize(-1);
        }

        void on_actionSystemFont_triggered() {
		changeFontSize(0);
        }
        
        void on_actionMorningTheme_triggered() {
		setTheme("morning");
	}
	
	void on_actionDayTheme_triggered() {
		setTheme("day");
	}
	
	void on_actionEveningTheme_triggered() {
		setTheme("evening");
	}
	
	void on_actionNightTheme_triggered() {
		setTheme("night");
	}
	
	void on_actionSystemTheme_triggered() {
		setTheme("system");
	}

        /// the text cursor position has changed
		void on_textMain_cursorPositionChanged() {
			if (!trainerIgnoresMessages) {
				trainerIgnoresMessages = true;
				int firstWord = -1;
				int lastWord = -1;
				uint firstPosition = textMain->textCursor().selectionStart();
				uint lastPosition = textMain->textCursor().selectionEnd();
				if (firstPosition < trainerCharToWordTable.size())
					firstWord = trainerCharToWordTable[firstPosition];
				if (lastPosition < trainerCharToWordTable.size())
					lastWord = trainerCharToWordTable[lastPosition];
				displayHint( firstWord, lastWord );
				if (trainerMode == EDIT_TRANSLATIONS) {
					QTextCursor textCursor = textHint->textCursor();
					textCursor.setPosition( textHint->toPlainText().length() );
					textHint->setTextCursor( textCursor );
					textHint->setFocus();
				}
				trainerIgnoresMessages = false;
			}
		};
		
		/// the text in the translation display has changed
		void on_textHint_textChanged() {
			textHint->show();
#ifdef Q_OS_ANDROID
			pushHideHints->show();
#endif
			if (!trainerIgnoresMessages) {
				trainerIgnoresMessages = true;
				if (trainerCurrentMeaning) {
					QTextCursor textCursor = textHint->textCursor();
					int position = textCursor.position();
					textHint->setText( trainerCurrentMeaning->changeUsingDisplayString( textHint->toPlainText() ) );
					position = trainerCurrentMeaning->getNextEditablePosition( position );
					textCursor.setPosition(position);
					textHint->setTextCursor( textCursor );
				}
				trainerIgnoresMessages = false;
			}
		};
		
		/// a different learning curve has been selected
		void on_listLearningCurve_currentIndexChanged(int n) {
			setLearningCurve(n);
		};
		
		/// the progress list index has been changed
		void on_listProgress_currentIndexChanged(int n) {
			displayText(n);
		};
		
	private:
	
		/// shows/hides toolbar and menu
        void setGui( bool writeMain, bool writeHint, bool file, bool editText, bool editTranslations, bool displayLearningWidgets, const QString& helpText );
		
		/// opens a text file
		void openTextFile();
		
		/// opens a translation file
		void openTranslationFile();
		
		/// saves a text file
		void saveTextFile( bool useCurrentPath = true );
		
        /// changes the font size
        /*! if change ==-1  size is decreased,
            if change == 0  size is set according to system setting,
            if change == 1  size is increased
            if change  > 1  size is set to value of "change" */
		void changeFontSize(int change);

		/// applies the font size from settings to GUI
		void applyCurrentFontSize();

		/// writes a new theme into settings
		void setTheme(QString name);

		/// applies the currently set theme
		void applyCurrentTheme();
	    
		/// displays the next text view
		void displayNextText();
		
		/// displays text view of progress step n
		void displayText(int n);
		
		/// displays text view for current progress
		void displayCurrentText();
		
		/// repeats the current text view with changed gaps
		void textView(int n);
		
		/// displays hint for a group of words
		void displayHint(int firstWord, int lastWord);
		
		/// clears and possibly saves an edited meaning
		void clearCurrentMeaning();
		
		/// creates a main text for display
		QString createText(bool hideWords, vector<uint> *charTable) const;
		
		/// creates a hint
		Meaning createHint(uint firstWord, uint lastWord);
		
		/// sets the learning curve
		void setLearningCurve(int n);
		
		/// sets the current mode
		void setMode(EditMode m);
		
		/// initialise the phrases and meanings
		void initPhrases();
		
		/// adds a text line or a group translation
		/*! If the line contains no "=" it is a text line, otherwise a translation. A text line is broken into words and added to the word list, a meaning line is created using the syntax "phrase of words = first translation | second translation | third translation | ..." */
		void readLine (
			QString line ///< line of words: x x = y y | z z | ...
		);
		
		/// clears all lists
		void clear();

        /// application
        QApplication *trainerParent;
		
		/// settings
		QSettings trainerSettings;
		
		/// all help texts
		HelpTexts helpTexts;
		
		/// vector to translate line and char into word index
		vector< uint > trainerCharToWordTable;
		
		/// currently edited meanings
		Meaning *trainerCurrentMeaning;
		
		/// list of words
		vector<Word *> trainerWordList;
		
		/// list of word groups
		vector<Phrase *> trainerPhraseList;
		
		/// list of group translations
		vector<Meaning *> trainerMeaningList;
		
		/// list of coefficients for learning curve
		vector<double> trainerLearningCurve;
		
		/// current learning curve
		uint trainerCurveIndex;
		
		/// current mode - edit (true) or display (false)
		EditMode trainerMode;
		
		/// ignoring messages to avoid recursion
		bool trainerIgnoresMessages;
		
		/// path of the current text file
		QString trainerTextFileName;
};
