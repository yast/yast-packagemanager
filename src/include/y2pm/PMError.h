/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:       PMError.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Transport class for error values

/-*/
#ifndef PMError_h
#define PMError_h

#include <iosfwd>

#include <y2util/stringutil.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMError
/**
 * @short Transport class for error values
 *
 * Quite simple approach. Error values are unsigned. E_ok (== 0) means no error.
 *
 * Conversion of error values to string, or writing to ostreams, per default
 * prints the error number as <B>ERROR(10242)</B> ("ERROR" is translatable).
 * For E_ok <B>OK</B> (also translatable) is printed.
 *
 * <B>Primitive support for error classes:</B> Assume a <CODE>class InstSrcError</CODE>
 * providing nothing but an enum of common InstSrc error values. Returning those error
 * values through <CODE>PMError</CODE>, errors originating from this class will be
 * reported as <B>ERROR(InstSrc:short description of error)</B>, if an error text
 * is provided, otherwise <B>ERROR(InstSrc:10242)</B>.
 *
 * To accomplish this extend <CODE>enum ErrClass</CODE> and define a new first error
 * value for InstSrcError in multiples of <CODE>_valrange</CODE> (currently 1024).
 * Extend the switches in <CODE>PMError::errClass</CODE> and <CODE>PMError::errstr</CODE>
 * by cases for the new enum value. By this <CODE>_valrange</CODE> error numbers starting
 * at <CODE>C_InstSrcError</CODE> are assigned to <CODE>class InstSrcError</CODE>.
 * <PRE>
 *     enum ErrClass {
 *       ...
 *       <B>C_InstSrcError = 10*_valrange</B>
 *     };
 * </PRE>
 *
 * Now define <CODE>class InstSrcError</CODE>.
 *
 * <CODE>errclass</CODE> defines the string to be used as error class name.
 * <CODE>errtext</CODE> must return the text describing the error, or the
 * numerical value as string. Error values will be printed as
 * <B>ERROR(</B>errclass<B>:</B>errtext(err)<B>)</B>.
 *
 * <PRE>
 * #include &lt;y2pm/PMError.h>
 *
 * ///////////////////////////////////////////////////////////////////
 * //
 * //	CLASS NAME : InstSrcError
 * //
 * class InstSrcError {
 *
 *   private:
 *
 *     <B>friend class PMError;</B>
 *
 *     <B>static const std::string errclass;</B>
 *
 *     <B>static std::string errtext( const unsigned e );</B>
 *
 *   public:
 *
 *     enum Error {
 *       <B>E_ok    = PMError::E_ok,           // E_ok is always 0</B>
 *       <B>E_error = PMError::C_InstSrcError, // C_InstSrcError first error value for InstSrcError</B>
 *       E_no_media,
 *       ...
 *     };
 * };
 *
 * ///////////////////////////////////////////////////////////////////
 * </PRE>
 * Implementation will look like this:
 * <PRE>
 * #include &lt;y2pm/InstSrcError.h>
 *
 * using namespace std;
 *
 * ///////////////////////////////////////////////////////////////////
 * #ifndef N_
 * #  define N_(STR) STR
 * #endif
 * ///////////////////////////////////////////////////////////////////
 *
 * <B>// define the error class name</B>
 * <B>const std::string InstSrcError::errclass( "InstSrc" );</B>
 *
 * ///////////////////////////////////////////////////////////////////
 * //
 * //
 * //	METHOD NAME : InstSrcError::errtext
 * //	METHOD TYPE : std::string
 * //
 * //	<B>DESCRIPTION : Return textual description or numerical value</B>
 * //     <B>as string;</B>
 * //
 * <B>std::string InstSrcError::errtext( const unsigned e )</B>
 * {
 *   switch ( (Error)e ) {
 *
 *   case E_ok:       return PMError::OKstring;
 *   case E_error:    return N_("error");
 *   case E_no_media: return N_("no media");
 *   ...
 *
 *   ///////////////////////////////////////////////////////////////////
 *   // no default: let compiler warn '... not handled in switch'
 *   ///////////////////////////////////////////////////////////////////
 *   };
 *
 *   return stringutil::numstring( e );
 * }
 * </PRE>
 *
 **/
class PMError {

  static const unsigned _valrange = 0x400;
  static const unsigned _valmask  = _valrange-1;
  static const unsigned _repmask  = ~_valmask;

  static const std::string errstrPrefix;

  static std::string defaulterrstr( const std::string & cl, const std::string & txt );

  public:

    static const unsigned    E_ok = 0;
    static const std::string OKstring;
    static const std::string ERRORstring;

    enum ErrClass {
      C_Error        = 0,
      // ADD/DEL ENTRIES:
      // adjust switches in errClass() and errstr()
      C_InstSrcError = 10*_valrange,
      C_MediaError   = 11*_valrange,
      C_InstTargetError   = 12*_valrange
    };

    unsigned _errval;

  public:

    PMError( const unsigned e = E_ok ) { _errval = e; }

    operator unsigned() const { return _errval; }

    static std::string errstr( const unsigned e );

    std::string errstr() const { return errstr( _errval ); }

    static ErrClass errClass( const unsigned e );

    ErrClass errClass() const { return errClass( _errval ); }

    friend std::ostream & operator<<( std::ostream & str, const PMError & obj );
};

///////////////////////////////////////////////////////////////////

#if 0 //draft
class PMError
{
    public:
	enum Serverity
	{
	    None = 0,	// No error
	    Normal,
	    Grave,
	    Fatal	// segfault etc, program should be aborted
	};

	/**
	 * suggested Action in case of failure. If Severity == None,
	 * SuggestedAction should be Continue
	 * */
	enum SuggestedAction
	{
	    Continue = 0,
	    Retry,
	    Fail
	};

	/**
	 * Possible Actions to take in case of failure. If Severity == None,
	 * PossibleAction should always include Continue
	 * */
	enum PossibleAction
	{
	    Continue = 1,
	    Retry    = 2,
	    Fail     = 4
	};

    public:
	/**
	 * Constructor
	 *
	 * @param reporter module or program which caused this error (e.g. wget, mount, MediaCD, ...)
	 * @param s Severity of Error, default no Error
	 * @param message Short message to show user
	 * @param possibleAction or'ed value of ways to deal with this error
	 * @param a Suggested way of proceeding after this error Occured.
	 * */
	PMError(
	    const std::string& reporter,
	    enum Severity s = None,
	    const std::string& message = "",
	    unsigned possibleAction = Continue,
	    enum SuggestedAction a = Continue
	    );

	/**
	 * Constructor
	 *
	 * like above except the errno value is accepted instead of a string
	 * */
	PMError(
	    const std::string& reporter,
	    enum Severity s = None,
	    int errnoval, 
	    unsigned possibleAction = Continue,
	    enum SuggestedAction a = Continue
	    );

	/**
	 * set longer explanatory text why this error occured and what's the
	 * best way to deal with it. May optionally be displayed by UI
	 * */
	void setDescription(const std::string& descr);

	/**
	 * Set callback for asking the user how to proceed with this error
	 * */
	void setAskUserFunc(bool (*callbac)(const PMError&));


	/**
	 * invoke callback function
	 *
	 * @return what the user decided to do
	 * */
	enum SuggestedAction askUser();
	

	enum Severity getSeverity() const;

	enum SuggestedAction getSuggestedAction() const;

	unsigned getPossibleAction() const;

	bool isActionPossible(enum PossibleAction a) const;

	const std::string& getMessage() const;

	const std::string& getDescription() const;

	int getErrno() const;

    private:

	std::string _reporter;
	
	enum Severity _severity;

	std::string _message;

	int _errno; // -1 if not applicable

	enum SuggestedAction _suggestedAction;

	unsigned _possibleAction;

	std::string _description;

	bool (*_askuserfunc)(const PMError&);
};
#endif

#endif // PMError_h
