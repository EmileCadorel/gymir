#include "Lexer.hh"

#include "config.h"
#include "coretypes.h"
#include "input.h"
#include "diagnostic.h"
#include "safe-ctype.h"

namespace Lexical {

  Lexer :: Lexer (const char * filename, FILE * input)
    : currentLine(1),
      currentColumn (1),
      currentWord (0),
      line_map (NULL)
  {
    line_map = ::linemap_add (::line_table, ::LC_ENTER,
			      /* sysp */0, filename,
			      /* currentLine */ 1);
    this -> input = input;
  }

  Lexer :: ~Lexer () {
    ::linemap_add (::line_table, ::LC_LEAVE,
		   /* sysp */ 0,
		   /* filename */ NULL,
		   /* to_line */0);
  }

  location_t Lexer::getCurrentLocation () {
    return linemap_position_for_column (::line_table, currentColumn);
  }

  TokenPtr Lexer::next () {
    if (this->currentWord >= read.size () - 1) {
      TokenPtr ret;
      do {
	ret = Token::makeEof();
	ret = get ();
	TokenId com;
	if ((com = isComment (ret)) != Token::EOF_TOKEN () && commentOn) {
	  do {
	    ret = Token::makeEof ();
	    ret = get ();	    
	  } while (ret->getId () != com && ret->getId () != Token::EOF_TOKEN());
	}
      } while (isSkip (ret) && ret->getId () != Token::EOF_TOKEN());
      return ret;
    } else {
      this->currentWord ++;
      return read [currentWord];
    }
  }

  TokenId Lexer::isComment (TokenPtr tok) {
    auto val = tokenIdToStr (tok->getId ());
    for (auto & it : comments) {
      if (it.first == val) {
	return (getFromStr (it.second));
      }
    }
    return Token::EOF_TOKEN ();
  }

  bool Lexer::isSkip (TokenPtr tok) {
    auto val = tokenIdToStr (tok->getId ());
    for (auto & it : skip) {
      if (it == val) return true;
    }
    return false;
  }

  void Lexer::setSkip (std::vector < std::string > skip) {
    this->skip = skip;
  }

  void Lexer::rewind (unsigned long nb) {
    auto re = (long)this->currentWord - (long)nb;
    if (re < 0) currentWord = 0;
    else currentWord = re;
  }
  
  void Lexer::setComments (std::vector < std::pair < std::string, std::string > > comments) {
    this->comments = comments;
  }

  void Lexer::setComments (bool on) {
    this->commentOn = on;
  }
  
  const char * Lexer::readln (FILE * i) {
    unsigned long max = 10;
    std::string final = "";
    while (1) {
      char * buf = (char*)malloc (sizeof (char) * max);
      char * aux = fgets(buf, max, i);
      if (aux == NULL) return aux;
      std::string ret = std::string (buf);
      final += ret;
      free (buf);
      if (ret.size () != max - 1) return final.c_str();
      else max *= 2;      
    }
  }
  
  TokenPtr Lexer::get () {
    const char * line = readln (input);
  }

  
  
}
