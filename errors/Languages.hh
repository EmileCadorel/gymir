#pragma once
#include "Error.hh"

namespace Ymir {

    namespace Languages {

	const char** fr () {
	    static const char* phrases [4];
	    static bool init = false;
	    if (!init) {
		phrases [SyntaxError] = "[%s] attendues, mais %s%s%s trouvé";
		phrases [SyntaxError2] = "%s%s%s inattendue";
		phrases [EscapeChar] = "caractère d'échappement inconnu";
		phrases [NotATemplate] = "L'élément '%s%s%s' n'est pas un template";
		phrases [EndOfFile] = "Fin de fichier inattendue";
		phrases [Unterminated] = "caractère de fin de chaîne '%s%s%s' manquant";
		init = true;
	    } 
	    return phrases;
	}

	const char** en () {
	    static const char* phrases [4];
	    static bool init = false;
	    if (!init) {
		phrases [SyntaxError] = "[%s] expected, when %s%s%s found";
		phrases [SyntaxError2] = "%s%s%s unexpected";
		phrases [EscapeChar] = "unknown escape character";
		phrases [NotATemplate] = "'%s%s%s' element is not a template";
		phrases [EndOfFile] = "unexpected end of file";
		phrases [Unterminated] = "missing terminating '%s%s%s' character";
		init = true;
	    } 
	    return phrases;
	}
	
	const char** getLanguage (Language ln) {
	    switch (ln) {
	    case FR : return fr ();
	    case EN : return en ();
	    }
	    return NULL;
	}
	


    }

	

}
