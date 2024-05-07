
/**
 * @file ConfigReader.hpp
 * Class to parse and manage configuration data files.
 * 
 */

#ifndef ConfigReader_HPP
#define ConfigReader_HPP

#include <string>
#include <map>
#include "StringUtils.hpp"
#include "Matrix.hpp"



namespace gpstk
{

      /** @addtogroup formattedfile */
      //@{

      /** This is a class to parse and manage configuration data files.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *      // Declare a ConfigReader object
       *   ConfigReader confRead;
       *
       *      // Open and parse configuration file
       *   confRead.open("configuration.txt");
       *
       *      // Read variable 'name' from section 'ONSA' using '()' operator
       *   string rxName( confRead("name", "ONSA') );
       *
       *      // Get default tolerance from default section "DEFAULT"
       *   double tolerance;
       *   tolerance = confRead.getValueAsDouble("tolerance");
       *
       *      // Print "baseline" description, value and units
       *   cout << confRead.getVariableDescription("baseline") << endl;
       *   cout << confRead.getValueAsDouble("baseline")       << endl;
       *   cout << confRead.getValueDescription("baseline")    << endl;
       *
       *      // Read if receiver "BELL" will be treated as a reference rx
       *   bool bellRef( confRead.getValueAsBoolean("reference", "BELL") );
       *
       * @endcode
       *
       * ConfigReader class provides powerful objects to read and manage
       * configuration data files. They support multiple sections, variable
       * descriptions, and value descriptions (such as units).
       *
       * An example of configuration file follows:
       *
       * @code
       *
       * # This is a configuration file, and this is a comment line
       * ; This is also a comment line
       *
       * # The general format is:
       * ; variableName, variableComment = value, valueComment
       *
       * # 'variableComment' and 'valueComment' are optional
       *
       * # So far, there is no section declared, so the following couple of
       * # variables are stored in section "DEFAULT"
       *
       * baseline, baseline between receivers = 13.434510, kilometers
       * tolerance, allowed difference between time stamps = 1.5, secs
       * reportExceptions = TRUE    ; Recover this with 'getValueAsBoolean()'
       *
       * # Declare a section
       * [ONSA]
       *
       * name, 4-char station name = ONSA
       *
       * staX, X station coordinate = 3370658.5419, meters
       * staY, Y station coordinate =  711877.1496, meters
       * staZ, Z station coordinate = 5349786.9542, meters
       *
       * antennaType : AOAD/M_B   # Note that you can use ':' instead of '='
       *
       *
       * [BELL]
       * reference = TRUE
       *
       *
       * [ROVER]
       * speed = 0.223 , m/s
       *
       *
       * [ONSA]   ; you may reuse a previous section and add new variables
       *          ; without problems (but with different names!!!)
       *
       * sampling, sampling period = 30, s
       *
       * @endcode
       *
       * The configuration file follows the following format:
       *
       * - Anything after a '#' or a ';' is a comment
       * - Blank lines are ignored.
       * - No line may have more than 255 characters.
       * - Variable and section names are ALWAYS converted to uppercase.
       * - Variable and section names MUST start with a letter, and must only
       *   contain a mix of letters, numbers, dashes (-) and underscores (_).
       * - Section names must be enclosed in brackets ([]).
       * - The Variable/Value pairs are separated by either '=' or ':'.
       * - You may add comments to variables and values. Use a comma to
       *   separate such comments.
       * - If you use the same variable name within a given section, only the
       *   last value will be used. Remember that names are ALWAYS converted to
       *   uppercase.
       *
       * By default, values are returned as std::string's, but there are methods
       * available to get them as booleans, integers and doubles. Comments are
       * always returned as std::string's.
       *
       * This class is very strict and throws a 'ConfigurationException'
       * exception when trying to access variables that don't exist in the
       * configuration file. This default behaviour may be changed with the
       *'setIssueException()' method.
       *
       * The format of the Variable/Value pairs is inspired in the options file
       * used in the GNSSTk project.
       */

       /// A structure used to store variable's data.
   struct variableData
   {
         // Default constructor initializing the data in the structure
      variableData() : varComment(""), value(""), valueComment("") {};

      std::string varComment;      ///< Variable comment
      std::string value;           ///< Value of variable
      std::string valueComment;    ///< Value comment
   };

      /// Define 'variableMap' type
   typedef std::map<std::string, variableData> variableMap;

      /// Define 'confMap' type
   typedef std::map<std::string, variableMap> confMap;


   class ConfigReader                      
   {
   public:

         /// Default constructor
      ConfigReader()
         : issueException(true), fallback2Default(false) {};


         /** Common constructor. It will always open 'file' for read and will
          *  configuration data in one pass.
          *
          * @param file    Configuration data file to read
          *
          */
      ConfigReader(const char* file)
         : filename(file), issueException(true),
           fallback2Default(false)
      { 
          strm.open(filename, std::ios::in);
          if(!strm)
          {
              FileMissingException fe("configuration file is not found");
              GPSTK_THROW(fe);
          }

          loadData(); 
      };


         /** Common constructor. It will always open 'fn' for read and will
          *  configuration data in one pass.
          *
          * @param file    Configuration data file to read
          *
          */
      ConfigReader(const std::string& file)
         :  filename(file),
            issueException(true),
            fallback2Default(false)
      { 
          strm.open(filename.c_str(), std::ios::in);
          if(!strm)
          {
              FileMissingException fe("configuration file is not found");
              GPSTK_THROW(fe);
          }

          loadData(); 
      };

         /// Method to open AND load configuration data file.
      virtual void open(const char* fn);


         /// Method to open AND load configuration data file.
      virtual void open(const std::string& fn)
      { open( fn.c_str() ); };

         /** Method to get the value of a given variable as a string
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual variableMap getDefaultVariableData()
         noexcept(false);


         /** Method to get the value of a given variable as a string
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual std::string getValue( std::string variable,
                                    std::string section = "DEFAULT",
                                    std::string defaultVal = "")
         noexcept(false);


         /** Method to get the value of a given variable as a double
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual double getValueAsDouble( std::string variable,
                                       std::string section = "DEFAULT",
                                       double defaultVal = 0.0)
         noexcept(false)
      { 
         return StringUtils::asDouble( getValue(variable, section, 
                                                StringUtils::asString(defaultVal)) ); 
      };


         /** Method to get the value of a given variable as an integer
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual int getValueAsInt( std::string variable,
                                 std::string section = "DEFAULT",
                                 int    defaultVal = 0 )
         noexcept(false)
      { 
         return StringUtils::asInt( getValue(variable, section, 
                                    StringUtils::asString(defaultVal)) ); 
      };


         /** Method to get the value of a given variable as a boolean
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual bool getValueAsBoolean( std::string variable,
                                      std::string section = "DEFAULT", 
                                      bool   defaultVal = false )
         noexcept(false);


         /** Method to fetch (as string) the first value of a given
          *  variable list.
          *
          * In this context, a variable list is the same as a variable but
          * it is composed of several parts (words), separated by spaces.
          *
          * @param variableList   Variable list name.
          * @param section        Section the variable list belongs to.
          *
          * \warning This method will MODIFY the original content of
          * 'variableList'.
          */
      virtual std::string fetchListValue( std::string variableList,
                                          std::string section = "DEFAULT",
                                          std::string defaultVal = "" )
         noexcept(false);


         /** Method to fetch (as double) the first value of a given
          *  variable list.
          *
          * In this context, a variable list is the same as a variable but
          * it is composed of several parts (words), separated by spaces.
          *
          * @param variableList   Variable list name.
          * @param section        Section the variable list belongs to.
          *
          * \warning This method will MODIFY the original content of
          * 'variableList'.
          */
      virtual double fetchListValueAsDouble( std::string variableList,
                                             std::string section = "DEFAULT",
                                             double defaultVal = 0.0 )
         noexcept(false)
      { 
         return StringUtils::asDouble( fetchListValue(variableList,section,
                                                      StringUtils::asString(defaultVal))); 
      };


         /** Method to fetch (as integer) the first value of a given
          *  variable list.
          *
          * In this context, a variable list is the same as a variable but
          * it is composed of several parts (words), separated by spaces.
          *
          * @param variableList   Variable list name.
          * @param section        Section the variable list belongs to.
          *
          * \warning This method will MODIFY the original content of
          * 'variableList'.
          */
      virtual int fetchListValueAsInt( std::string variableList,
                                       std::string section = "DEFAULT",
                                       int    defaultVal = 0 )
         noexcept(false)
      { 
         return StringUtils::asInt( fetchListValue(variableList,section,
                                                   StringUtils::asString(defaultVal))); 
      };


         /** Method to fetch (as boolean) the first value of a given
          *  variable list.
          *
          * In this context, a variable list is the same as a variable but
          * it is composed of several parts (words), separated by spaces.
          *
          * @param variableList   Variable list name.
          * @param section        Section the variable list belongs to.
          *
          * \warning This method will MODIFY the original content of
          * 'variableList'.
          *
          * \warning If variable list is empty, it will return FALSE.
          */
      virtual bool fetchListValueAsBoolean( std::string variableList,
                                            std::string section = "DEFAULT",
                                            bool   defaultVal = false)
         noexcept(false);


         /** Method to get the number of items in a given variable list.
          *
          * In this context, a variable list is the same as a variable but
          * it is composed of several parts (words), separated by spaces.
          *
          * @param variableList   Variable list name.
          * @param section        Section the variable list belongs to.
          *
          */
      virtual int getNumItem( std::string variableList,
                              std::string section = "DEFAULT" )
         noexcept(false)
      { return StringUtils::numWords( getValue( variableList, section ) ); };


         /** Method to get the description of a given variable
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual std::string getVariableDescription( std::string variable,
                                                  std::string section = "DEFAULT" )
         noexcept(false);


         /** Method to get the description of a given value
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual std::string getValueDescription( std::string variable,
                                               std::string section = "DEFAULT" )
         noexcept(false);


         /** Method to get whether an exception will be issued
          *  when requesting an invalid variable (or section), or not.
          */
      virtual bool getIssueException( void ) const
      { return issueException; };


         /** Method to set whether an exception will be issued
          *  when requesting an invalid variable (or section), or not.
          *
          * @param issueEx    Whether an exception will be issued or not
          *
          */
      ConfigReader& setIssueException(bool issueEx)
      { issueException = issueEx; return (*this); }


         /** Method to get whether when a variable is looked for in a given
          *  section and not found, it will also be looked for in 'DEFAULT'.
          */
      virtual bool getFallback2Default( void ) const
      { return fallback2Default; };


         /** Method to set whether when a variable is looked for in a given
          *  section and not found, it will also be looked for in 'DEFAULT'.
          *
          * @param fallback    Whether we will fallback to 'DEFAULT' or not
          *
          */
      ConfigReader& setFallback2Default(bool fallback)
      { fallback2Default = fallback; return (*this); }


         /// Method to clear the stored variables.
      virtual ConfigReader& clear(void)
      { confData.clear(); return (*this); };


         /// Method to get the name of each section in order.
      virtual std::string getEachSection(void);


         /// Method to reset the iterator traversing section names. This method
         /// is intended to be used complementing method 'getEachSection()'.
      virtual void resetSection(void)
      { itCurrentSection = confData.begin(); return; };


         /** Method to check if a given section/variable pair exists.
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual bool ifExist( std::string variable,
                            std::string section = "DEFAULT" )
         noexcept(false);


         /** Operator to get the value of a given variable as a string
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual std::string operator()( std::string variable,
                                      std::string section = "DEFAULT" )
         noexcept(false)
      { return getValue(variable, section); };


         /// Destructor
      virtual ~ConfigReader() {}


   private:

      /// file stream for data reading
      std::fstream strm;

      /// file name;
      std::string filename;


      /// This boolean field determines whether an exception will be issued
      /// when requesting an invalid variable (or section), or not
      bool issueException;


      /// This boolean field determines if when a variable doesn't exist
      /// in a given section, it will also be looked for in 'DEFAULT'
      bool fallback2Default;




         /// Map holding the configuration information
      confMap confData;


         /// Iterator pointing to the current section
      confMap::const_iterator itCurrentSection;


         /** Method to check if the given parameter name is properly formed.
          *
          * @param name    Name to the checked.
          */
      virtual bool checkName(std::string name);


         /// Method to store conf data in this class' data map
      virtual void loadData(void)
         noexcept(false);


   }; // End of class 'ConfigReader'

      //@}

}  // End of namespace gpstk
#endif  // ConfigReader_HPP
