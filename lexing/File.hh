#pragma once

#include <ymir/utils/Ref.hh>
// #include <ymir/utils/OutBuffer.hh>
#include <ymir/utils/Memory.hh>

namespace Ymir {
    namespace Error {
	void halt (const char*);
    }
}

namespace lexing {

    class File;
    class IFile {
    protected :

	friend File;

	IFile ();

    public :

	virtual File clone () const = 0;
	
	virtual bool isOf (const IFile * type) const = 0;

	virtual bool isEof () const = 0;

	virtual ulong tell () const = 0;

	virtual void seek (ulong i) = 0;
	
	virtual std::string readln () = 0;

	virtual std::string readAll () = 0;

	virtual std::string getFilename () const = 0;
	
	virtual void close () = 0;

	virtual bool isClosed () const = 0;
	
	virtual ~IFile ();
	
    };
    
    class File : public RefProxy <IFile, File> {
    private :

	static File __empty__;

	/**
	 * Same as empty
	 */
	File ();
	
    public :

	File (IFile * f);

	static File empty ();

	File clone () const;
	
	bool isEmpty () const;
	
	bool isEof () const;

	ulong tell () const;

	std::string readln ();

	void seek (ulong);

	std::string readAll ();

	std::string getFilename () const;

	void close ();

	bool isClosed () const;
	
	/**
	 * \brief Cast the content pointer into the type (if possible)
	 * \brief Raise an internal error if that is not possible
	 */
	template <typename T>
	T& to ()  {	    
	    if (this-> _value == nullptr)
		Ymir::Error::halt ("");	    

	    T t;
	    if (!this-> _value.get ()-> isOf (&t))
		Ymir::Error::halt ("");
	    return *((T*) this-> _value.get ());	    
	}

	/**
	 * \brief Cast the content pointer into the type (if possible)
	 * \brief Raise an internal error if that is not possible
	 */
	template <typename T>
	const T& to () const  {	    
	    if (this-> _value == nullptr)
		Ymir::Error::halt ("");	    

	    T t;
	    if (!this-> _value.get ()-> isOf (&t))
		Ymir::Error::halt ("");
	    return *((const T*) this-> _value.get ());	    
	}

	/**
	 * \brief Tell if the inner type inside the proxy is of type T
	 */
	template <typename T>
	bool is () const  {	    
	    if (this-> _value == nullptr)
		return false;

	    T t;
	    return this-> _value.get ()-> isOf (&t); 			    
	}
	
    };
    
    


    class RealFile : public IFile {
    private :
	
	std::string _filename;
	FILE * _ptr;
	
    private :

	RealFile ();
	
	RealFile (const char * filename, FILE * ptr);

	RealFile (const RealFile & other);

	const RealFile & operator= (const RealFile & other);
	
    public :

	static File init (const char * filename);

	static File init (const std::string & filename);

	virtual File clone () const override;
	
	bool isOf (const IFile * type) const override;

	bool isEof () const override;

	ulong tell () const override;

	void seek (ulong i) override;
	
	std::string readln () override;

	std::string readAll () override;

	std::string getFilename () const override;

	void close () override;

	bool isClosed () const override;
	
	~RealFile ();
	
    };

    class StringFile : public IFile {
    private :

	std::string _content;
	unsigned long _cursor;
	
    private :

	friend File;

	StringFile ();

	StringFile (const std::string & content);
	
    public :

	static File init (const std::string & content);
	
	virtual File clone () const override;

	bool isOf (const IFile * type) const override;		

	bool isEof () const override;

	ulong tell () const override;

	void seek (ulong i) override;
	
	std::string readln () override;

	std::string readAll () override;

	std::string getFilename () const override;

	void close () override;

	bool isClosed () const override;
    };
    
        


}
