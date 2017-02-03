#ifndef CLPP_HPP___
#define CLPP_HPP___

#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <locale>

#define CLPP_DEBUG 1

// ---------------------------------------------------------------------------------------
// Flags
// -p or /p <-- those are same. They enable the 'p' flag
//

// Special Flags
// --p

// Variable
// key=value

// 
// ---------------------------------------------------------------------------------------

namespace lmolly {

class clpp {
private:
   enum TOKEN_TYPE {
      TT_FLAG_MARKER,
      TT_SPECIAL_FLAG_MARKER,
      TT_STRING_LITERAL,
      TT_NAME,
      TT_NUMBER,
      TT_EQUAL
   };

   struct token {
      TOKEN_TYPE   mTT;
      std::string  mValue;
   };

public:
   clpp() :  mIsLexed(false),
             mHelpFlagEnabled(false) {

   }
   ~clpp() {

   }

   bool process(std::string arguments) {
      bool rval = true;

      // Save the argument string
      mArguments = arguments;

      std::cout << "Assembled arguments: " << mArguments << std::endl; 

      if (lex(mArguments)) {
         rval = parse();
      }
      else {
         rval = false;
         std::cout << "Error in arguments" << std::endl;
         help();
      }
      return rval;
   }

   void process(int argc, char *argv[]) {
      std::string assembled_arguments;

      mProgramName = argv[0];

      for (int i=1;i<argc;++i) {
         assembled_arguments += argv[i];
         assembled_arguments += " ";
      }
      process(assembled_arguments);

      // If help enabled 
      if (mHelpFlagEnabled) {
         if (has_flag_set(mHelpFlag)) {
            help();
         }
      }
   }

   std::string get_string(const std::string &key) {
      std::string rval = mKeyValueString[key];
      return rval;
   }

   unsigned int get_number(const std::string &key) {
      unsigned int rval = mKeyValueNumber[key];
      return rval;
   }

   void help() {
      std::cout << mProgramName << " usage" << std::endl;
      std::cout << "-----------------------------------------------------------" << std::endl;
      if (mValidFlags.size() > 0) {
         std::cout << "Flags : " << std::endl;
         for (auto &x: mValidFlags) {
            std::cout << "-" << x.first << " or /" << x.first << " : " << x.second << std::endl;
         }
         std::cout << std::endl;
      }

      if (mValidSpecialFlags.size() > 0) {
         std::cout << "Special Flags : " << std::endl;
         for (auto &x: mValidSpecialFlags) {
            std::cout << "--" << x.first << " : " << x.second << std::endl;
         }
         std::cout << std::endl;
      }
   }

   void set_help_flag(char hf) {
      mHelpFlagEnabled = true;
      mHelpFlag = hf;
   }

   bool has_flag_set(char f) {
      bool rval = false;

      if (std::find(mOnFlags.begin(), mOnFlags.end(), f) != mOnFlags.end()) {
         rval = true;
      }

      return rval;
   }

   bool has_special_flag_set(char f) {
      bool rval = false;

      if (std::find(mOnSpecialFlags.begin(), mOnSpecialFlags.end(), f) != mOnSpecialFlags.end()) {
         rval = true;
      }

      return rval;
   }

   void add_flag(char c, std::string s) {
      mValidFlags[c] = s;
   }

   void add_special_flag(char c, std::string s) {
      mValidSpecialFlags[c] = s;
   }

   friend std::ostream& operator<< (std::ostream &out, const clpp::token &t);

   // Debugging
   void PrintFlags() {
      for (auto &x : mOnFlags) {
         std::cout << x << std::endl;
      }
   }
   void PrintKeyValueStrings() {
      for (auto &x : mKeyValueString) {
         std::cout << x.first << " = " << x.second << std::endl;
      }
   }
   void PrintKeyValueNumbers() {
      for (auto &x : mKeyValueNumber) {
         std::cout << x.first << " = " << x.second << std::endl;
      }
   }

private:
   // Stores the argument list in one consecutive string 
   // That's the input
   std::string     mArguments;
   std::string     mProgramName;

   std::list<char>                     mOnFlags;
   std::list<char>                     mOnSpecialFlags;
   std::list<std::string>              mWords;
   std::map<std::string, std::string>  mKeyValueString;
   std::map<std::string, unsigned int> mKeyValueNumber;

   bool mHelpFlagEnabled;
   char mHelpFlag;

   std::map<char, std::string> mValidFlags;
   std::map<char, std::string> mValidSpecialFlags;

   bool            mIsLexed;
   std::list<clpp::token> mTokenList;

   std::string     mLastValue;

   bool lex(std::string arguments) {
      bool rval = true;
      auto strit = arguments.begin();
      const auto strit_end =  arguments.end();
      std::locale loc;
      while (strit != strit_end) {
         // Found a flag or a special flag
         if (*strit == '-') {
	    std::cout << "Dash" << std::endl;
            ++strit;
            if (*strit == '-') {
               ++strit;
               std::string value;
               value += *strit++;
               token t{TT_SPECIAL_FLAG_MARKER, value};
               std::cout << t;
               mTokenList.push_back(t);
            }
            else {
               std::string value;
               value += *strit++;
               token t{TT_FLAG_MARKER, value};
               std::cout << t;
               mTokenList.push_back(t);
            }
         }
         else if (*strit == '"') {
	    std::cout << "Quote" << std::endl;
            ++strit;
            std::string value;
            while ((*strit != '"')) {
               value += *strit;
               ++strit;
            }
            ++strit;
            token t{TT_STRING_LITERAL, value};
            std::cout << t;
            mTokenList.push_back(t);
         }
         // Found a flag 
         else if (*strit == '/') {
	    std::cout << "Slash" << std::endl;
            strit++;
            std::string value;
            value += *strit++;
            token t{TT_FLAG_MARKER, value};
            std::cout << t;
            mTokenList.push_back(t);
         }
         // Found a name
         else if (std::isalpha(*strit, loc) || (*strit == '_')) {
	    std::cout << "Name" << std::endl;
            std::string value;
            while (std::isalnum(*strit, loc) || (*strit == '_') || (*strit == '.') ) {
               value += *strit;
               ++strit;
            }
            token t{TT_NAME, value};
            std::cout << t;
            mTokenList.push_back(t);
         }
         // Found a number
         else if (std::isdigit(*strit, loc)) {
	    std::cout << "Number" << std::endl;
            std::string value;
            while (std::isdigit(*strit, loc)) {
               value += *strit;
               ++strit;
            }
            token t{TT_NUMBER, value};
            std::cout << t;
            mTokenList.push_back(t);
         }
         // Found an equal 
         else if (*strit == '=') {
	    std::cout << "Equal" << std::endl;
            ++strit;
            token t{TT_EQUAL, " "};
            std::cout << t;
            mTokenList.push_back(t);
         }
         // Skip all spaces
         else if (*strit == ' ') {
            ++strit;
         }
         // Some error
         else {
            rval = false;
            break;
         }
      }

      if (rval == true) {
         mIsLexed = true;
      }

      return rval;
   }

   bool parse() {
      bool rval = true;

      if (mIsLexed) {
         auto tokenit = mTokenList.begin();
         const auto tokenit_end = mTokenList.end();

         while ((tokenit != tokenit_end) && (rval == true)) {
            rval = declaration(tokenit);
         }
      }
      else {
         rval = false;
      }

      return rval;
   }

   // GRAMMAR
   // --------------------------------------------------------------------
   bool declaration(std::list<clpp::token>::iterator &it) {
      bool rval = true;
      if (consume(it, clpp::TT_FLAG_MARKER)) {
         //std::cout << "Found Flag" << std::endl;
         mOnFlags.push_back(mLastValue[0]);
      }
      else if (consume(it, clpp::TT_SPECIAL_FLAG_MARKER)) {
         //std::cout << "Found Special Flag" << std::endl;
         mOnSpecialFlags.push_back(mLastValue[0]);
      }
      else if (consume(it, clpp::TT_NAME)) {
         if (isnext(it, TT_EQUAL)) {
            //std::cout << "Found NAME = ";
            std::string key = mLastValue;
            rval = consume(it, clpp::TT_EQUAL);
            if (consume(it, clpp::TT_NAME)) {
               //std::cout << "NAME" << std::endl;
               mKeyValueString[key] = mLastValue;
            }
            else  if (consume(it, clpp::TT_STRING_LITERAL)) {
               //std::cout << "NAME" << std::endl;
               mKeyValueString[key] = mLastValue;
            }
            else if (consume(it, clpp::TT_NUMBER)) {
               //std::cout << "NUMBER";
               mKeyValueNumber[key] = std::stoi(mLastValue);
            }
            else {
               //std::cout << "NUMBER";
               rval = false;
            }
            std::cout << std::endl;
         }
         else {
            //std::cout << "Found NAME" << std::endl;
            mWords.push_back(mLastValue);
         }
      }
      else {
         rval = false;
      }
      return rval;
   }
   // --------------------------------------------------------------------

   bool consume(std::list<clpp::token>::iterator &it, clpp::TOKEN_TYPE tt) {
      bool rval = false;
      if (it->mTT == tt) {
         mLastValue = it->mValue;
         ++it;
         rval = true;
      }
      return rval;
   }

   bool isnext(std::list<clpp::token>::iterator &it, clpp::TOKEN_TYPE tt) {
      bool rval = false;
      if (it->mTT == tt) {
         rval = true;
      }
      return rval;
   }
};

#if (CLPP_DEBUG == 1) 
std::ostream& operator<< (std::ostream &out, const clpp::token &t) {
   switch (t.mTT) {
   case clpp::TT_FLAG_MARKER:
      out << "FLAG = " << t.mValue << std::endl;
      break;
   case clpp::TT_SPECIAL_FLAG_MARKER:
      out << "SPECIAL FLAG = " << t.mValue << std::endl;
      break;
   case clpp::TT_NAME:
      out << "NAME = " << t.mValue << std::endl;
      break;
   case clpp::TT_NUMBER:
      out << "NUMBER = " << t.mValue << std::endl;
      break;
   case clpp::TT_STRING_LITERAL:
      out << "STRING LITERAL = " << t.mValue << std::endl;
      break;          
   case clpp::TT_EQUAL:
      out << "EQUAL = " << t.mValue << std::endl;
      break;
   }
   return out;
}
#else
std::ostream& operator<< (std::ostream &out, const clpp::token &t) {
   return out;
}
#endif

}

#endif
