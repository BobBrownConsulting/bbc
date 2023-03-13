/*
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#pragma once

#include "BBCAssert.h"
#include "Singleton.h"

// StartupOptions requires the use of Boost or TinyXML
//
#ifdef BBC_USE_TINYXML2
#include "tinyxml2.h"

using namespace tinyxml2;
#endif

#ifdef BBC_USE_BOOST
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
using namespace boost::property_tree;
#else
static_assert(true, "StartupOptions::initialize - Requires BBC_USE_BOOST!");
#endif

/// Boost tag for reading attributes
///
#define XMLATTR "<xmlattr>"

/// Support for packing/parsing a StartupOptions file
/// Used as XML data
///
#define STARTUP_OPTIONS_STARTUPOPTIONS "StartupOptions"
#define STARTUP_OPTIONS_DOT "."
#define STARTUP_OPTIONS_OPTION "Option"
#define STARTUP_OPTIONS_TYPE "type"
#define STARTUP_OPTIONS_NAME "name"
#define STARTUP_OPTIONS_VALUE "value"

/// The following are the TypeName identifiers for supported StartupOptions.
///
#define STARTUP_OPTIONS_TYPENAME_BOOL "StartupOptionBool"
#define STARTUP_OPTIONS_TYPENAME_INT32 "StartupOptionInt32"
#define STARTUP_OPTIONS_TYPENAME_INT64 "StartupOptionInt64"
#define STARTUP_OPTIONS_TYPENAME_FLOAT "StartupOptionFloat"
#define STARTUP_OPTIONS_TYPENAME_STRING "StartupOptionString"

/// Forward declarations needed by IStartupOptions
///
class StartupOptions;
class StartupOption;

///
/// \brief IStartupOptions is a utility class for creating startup options.
///
class IStartupOptions : public Singleton<StartupOptions>
{
    friend class StartupOptionBool;
    friend class StartupOptionInt32;
    friend class StartupOptionInt64;
    friend class StartupOptionFloat;
    friend class StartupOptionString;

public:
    
    virtual ~IStartupOptions() {}
    
    ///
    /// Provides access to the IStartupOptions interface.
    ///
    /// The interface needs it's own factory such that we can have access to the interface APIs.
    /// The base Singleton doesn't have all of the needed APIs.
    ///
    /// @return References to the IStartupOptions singleton
    ///
    static IStartupOptions& instance()
    {
        return reinterpret_cast<IStartupOptions&>(Singleton<StartupOptions>::instance());
    }

    ///
    /// Initializes the IStartupOptions with the contents of the StartupOption file.
    ///
    /// @param[in] iStartupOptionsFile is the path to a valid StartupOption file
    ///
    /// @return True if initialization was successful
    ///
    virtual bool initialize(const std::string& iStartupOptionsFile) = 0;

    ///
    /// Writes a default StartupOption file with all known StartupOptions and their defaults.
    ///
    /// @param[in] iStartupOptionsFile is the path for the default StartupOption file
    ///
    /// @return True if the file was created successfully
    ///
    virtual bool writeDefaultStartupOptionsFile(const std::string& iStartupOptionsFile) = 0;
    
protected:
    
    ///
    /// Adds a StartupOption
    ///
    /// Note: This method asserts if duplicate StartupOptions are added.
    ///       A duplicate is defined as a StartupOption with the same name
    ///
    /// @param[in] iOption is the StartupOption to add to the IStartupOptions singleton
    ///
    /// @return True if the StartupOption was added successfully
    ///
    virtual bool add(StartupOption* iOption) = 0;
};

/**
 * Base class for any StartupOption.
 *
 * Provides basic information about the name and type
 */
class StartupOption
{
public:
    
    /// Constructor
    StartupOption() {}

    /// Destructor
    virtual ~StartupOption() {}
    
    /**
     * Returns the name of the option.
     *
     * Note - All StartupOptions must have a unique name.
     *
     * @return std::string containing the name of the StartupOption
     */
    const std::string& name()
    {
        return name_;
    }

    /**
     * Returns the typeName of the option.
     *
     * The typeName is a textual representation of the type of StartupOption.
     * See STARTUP_OPTIONS_TYPENAME_* for the available type names.
     *
     * @return std::string containing the name of the StartupOption
     */
    const std::string& typeName()
    {
        return typeName_;
    }

protected:
    std::string name_;
    std::string typeName_;
};

/**
 * Templated base class for a type specific StartupOption
 *
 * Provides standard set, get, and type APIs
 */
template<class _class_>
class StartupOptionTyped : public StartupOption
{
public:
    
    /// Constructor
    StartupOptionTyped() {}
    
    /// Destructor
    virtual ~StartupOptionTyped() {}
    
    /**
     * Sets the value of the StartupOption
     */
    void set(const _class_& iVal)
    {
        value_ = iVal;
    }
    
    /**
     * Returns the default value of the StartupOption
     *
     * @return default value of _class_ type
     */
    const _class_& getDefault()
    {
        return defaultValue_;
    }
    
    /**
     * Returns the value of the StartupOption
     *
     * @return value of _class_ type
     */
    operator _class_()
    {
        return value_;
    }
    
protected:
    _class_ value_;
    _class_ defaultValue_;
};

/**
 * StartupOptionBool for boolean types
 */
class StartupOptionBool : public StartupOptionTyped<bool>
{
public:
    
    StartupOptionBool(const std::string& iName, bool iVal)
    {
        name_ = iName;
        typeName_ = STARTUP_OPTIONS_TYPENAME_BOOL;
        value_ = iVal;
        defaultValue_ = iVal;
        
        IStartupOptions::instance().add(this);
    }
    
    virtual ~StartupOptionBool() {}
    
private:
    
};

/**
 * StartupOptionInt32 for int32_t types
 */
class StartupOptionInt32 : public StartupOptionTyped<int32_t>
{
public:
    
    StartupOptionInt32(const std::string& iName, int32_t iVal)
    {
        name_ = iName;
        typeName_ = STARTUP_OPTIONS_TYPENAME_INT32;
        value_ = iVal;
        defaultValue_ = iVal;
        
        IStartupOptions::instance().add(this);
    }
    
    virtual ~StartupOptionInt32() {}
    
private:
    
};

/**
 * StartupOptionInt64 for int64_t types
 */
class StartupOptionInt64 : public StartupOptionTyped<int64_t>
{
public:
    
    StartupOptionInt64(const std::string& iName, int64_t iVal)
    {
        name_ = iName;
        typeName_ = STARTUP_OPTIONS_TYPENAME_INT64;
        value_ = iVal;
        defaultValue_ = iVal;
        
        IStartupOptions::instance().add(this);
    }
    
    virtual ~StartupOptionInt64() {}
    
private:
    
};

/**
 * StartupOptionFloat for float (32-bit floating point) types
 */
class StartupOptionFloat : public StartupOptionTyped<float>
{
public:
    
    StartupOptionFloat(const std::string& iName, float iVal)
    {
        name_ = iName;
        typeName_ = STARTUP_OPTIONS_TYPENAME_FLOAT;
        value_ = iVal;
        defaultValue_ = iVal;
        
        IStartupOptions::instance().add(this);
    }
    
    virtual ~StartupOptionFloat() {}
    
private:
    
};

/**
 * StartupOptionString for std::string types
 */
class StartupOptionString : public StartupOptionTyped<std::string>
{
public:
    
    StartupOptionString(const std::string& iName, const std::string& iVal)
    {
        name_ = iName;
        typeName_ = STARTUP_OPTIONS_TYPENAME_STRING;
        value_ = iVal;
        defaultValue_ = iVal;
        
        IStartupOptions::instance().add(this);
    }
    
    virtual ~StartupOptionString() {}
private:
    
};

/**
 * Concrete implemntation for StartupOptions.
 */
class StartupOptions : protected IStartupOptions
{
public:
    
    // Basic information and format for a StartupOptions file
    //
    /*
     <?xml version="1.0" encoding="utf-8"?>
     <StartupOptions>
         <Option type="StartupOptionBool" name="StartupOptionBoolName" value="true"/>
         <Option type="StartupOptionInt32" name="StartupOptionInt32Name" value="0xffffffff"/f>
         <Option type="StartupOptionInt64" name="StartupOptionInt64Name" value="0xffffffffffffffff"/>
         <Option type="StartupOptionFloat" name="StartupOptionFloatName" value="3.14"/>
         <Option type="StartupOptionString" name="StartupOptionStringName" value="Hello world!"/>
     </StartupOptions>
     */
    
    bool initialize(const std::string& iStartupOptionsFile)
    {
        if (initalized_)
            return true;

#ifdef BBC_USE_TINYXML2
        XMLDocument doc;
        if (doc.LoadFile(iStartupOptionsFile.c_str()) != XML_SUCCESS)
            return false;
        
        for (auto option : options_)
        {
            XMLElement* root = doc.FirstChildElement(STARTUP_OPTIONS_STARTUPOPTIONS);
            
            if (root == nullptr)
                continue;
            
            for (XMLElement* e = root->FirstChildElement(STARTUP_OPTIONS_OPTION);
                 e != NULL;
                 e = e->NextSiblingElement(STARTUP_OPTIONS_OPTION))
            {
                std::string type = e->Attribute(STARTUP_OPTIONS_TYPE);
                std::string name = e->Attribute(STARTUP_OPTIONS_NAME);
                
                if (option->typeName() == type && option->name() == name)
                {
                    // We found a match
                    // Update the know values
                    //
                    if (option->typeName() == STARTUP_OPTIONS_TYPENAME_BOOL)
                    {
                        bool value = false;
                        value = e->BoolAttribute(STARTUP_OPTIONS_VALUE, value);
                        
                        StartupOptionBool *tmp = dynamic_cast<StartupOptionBool*>(option);
                        BBC_ASSERT(tmp);
                        tmp->set(value);
                        break;
                    }
                    else
                    if (option->typeName() == STARTUP_OPTIONS_TYPENAME_INT32)
                    {
                        int32_t value = 0;
                        value = e->IntAttribute(STARTUP_OPTIONS_VALUE, value);
                        
                        StartupOptionInt32 *tmp = dynamic_cast<StartupOptionInt32*>(option);
                        BBC_ASSERT(tmp);
                        tmp->set(value);
                        break;
                    }
                    else
                    if (option->typeName() == STARTUP_OPTIONS_TYPENAME_INT64)
                    {
                        int64_t value = 0;
                        value = e->Int64Attribute(STARTUP_OPTIONS_VALUE, value);
                        
                        StartupOptionInt64 *tmp = dynamic_cast<StartupOptionInt64*>(option);
                        BBC_ASSERT(tmp);
                        tmp->set(value);
                        break;
                    }
                    else
                    if (option->typeName() == STARTUP_OPTIONS_TYPENAME_FLOAT)
                    {
                        float value = 0;
                        value = e->FloatAttribute(STARTUP_OPTIONS_VALUE, value);
                        
                        StartupOptionFloat *tmp = dynamic_cast<StartupOptionFloat*>(option);
                        BBC_ASSERT(tmp);
                        tmp->set(value);
                        break;
                    }
                    else
                    if (option->typeName() == STARTUP_OPTIONS_TYPENAME_STRING)
                    {
                        std::string value;
                        if (const char* returnVal = e->Attribute(STARTUP_OPTIONS_VALUE))
                        {
                            value = std::string(returnVal);
                        }
                        
                        StartupOptionString *tmp = dynamic_cast<StartupOptionString*>(option);
                        BBC_ASSERT(tmp);
                        tmp->set(value);
                        break;
                    }
                    else
                    {
                        BBC_ASSERT_R(!"startup_options::initialize - Unknown typeName");
                    }
                }
            }
        }
        
        initalized_ = true;
        
        return true;
#endif
        
#ifdef BBC_USE_BOOST
        try
        {
            // Create empty property tree object
            ptree tree;
            
            // Parse the XML into the property tree.
            read_xml(iStartupOptionsFile, tree);
            
            for (auto option : options_)
            {
                BOOST_FOREACH(ptree::value_type &node, tree.get_child(STARTUP_OPTIONS_STARTUPOPTIONS))
                {
                    if (node.first != STARTUP_OPTIONS_OPTION)
                    {
                        continue;
                    }
                    
                    std::string type = node.second.get(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_TYPE, "");
                    std::string name = node.second.get(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_NAME, "");
                    if (option->typeName() == type && option->name() == name)
                    {
                        // We found a match
                        // Update the know values
                        //
                        if (option->typeName() == STARTUP_OPTIONS_TYPENAME_BOOL)
                        {
                            bool value = false;
                            value = node.second.get(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, value);

                            StartupOptionBool *tmp = dynamic_cast<StartupOptionBool*>(option);
                            BBC_ASSERT(tmp);
                            tmp->set(value);
                            break;
                        }
                        else
                        if (option->typeName() == STARTUP_OPTIONS_TYPENAME_INT32)
                        {
                            int32_t value = 0;
                            value = node.second.get(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, value);

                            StartupOptionInt32 *tmp = dynamic_cast<StartupOptionInt32*>(option);
                            BBC_ASSERT(tmp);
                            tmp->set(value);
                            break;
                        }
                        else
                        if (option->typeName() == STARTUP_OPTIONS_TYPENAME_INT64)
                        {
                            int64_t value = 0;
                            value = node.second.get(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, value);

                            StartupOptionInt64 *tmp = dynamic_cast<StartupOptionInt64*>(option);
                            BBC_ASSERT(tmp);
                            tmp->set(value);
                            break;
                        }
                        else
                        if (option->typeName() == STARTUP_OPTIONS_TYPENAME_FLOAT)
                        {
                            float value = 0;
                            value = node.second.get(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, value);

                            StartupOptionFloat *tmp = dynamic_cast<StartupOptionFloat*>(option);
                            BBC_ASSERT(tmp);
                            tmp->set(value);
                            break;
                        }
                        else
                        if (option->typeName() == STARTUP_OPTIONS_TYPENAME_STRING)
                        {
                            std::string value;
                            value = node.second.get(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, value);

                            StartupOptionString *tmp = dynamic_cast<StartupOptionString*>(option);
                            BBC_ASSERT(tmp);
                            tmp->set(value);
                            break;
                        }
                        else
                        {
                            BBC_ASSERT_R(!"StartupOptions::initialize - Unknown typeName");
                        }
                    }
                }
            }

            initalized_ = true;
            
            return true;
        }
        catch(...)
        {
            std::cout << "StartupOptions::read - error reading startup options file." << std::endl;
            return false;
        }
#else
        //static_assert(false, "StartupOptions::initialize - Requires BBC_USE_BOOST!");
        return false;
#endif
    }
    
    bool writeDefaultStartupOptionsFile(const std::string& iStartupOptionsFile)
    {
#ifdef BBC_USE_TINYXML2
        XMLDocument doc;
        
        XMLElement *element = doc.NewElement(STARTUP_OPTIONS_STARTUPOPTIONS);
        doc.InsertFirstChild(element);
        
        for (auto option : options_)
        {
            if (StartupOptionBool* opt = dynamic_cast<StartupOptionBool*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_BOOL));
                
                XMLElement *optElement = element->InsertNewChildElement(STARTUP_OPTIONS_OPTION);
                
                optElement->SetAttribute(STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_BOOL);
                optElement->SetAttribute(STARTUP_OPTIONS_NAME, opt->name().c_str());
                optElement->SetAttribute(STARTUP_OPTIONS_VALUE, static_cast<bool>(*opt));
            }
            
            if (StartupOptionInt32* opt = dynamic_cast<StartupOptionInt32*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_INT32));
                
                XMLElement *optElement = element->InsertNewChildElement(STARTUP_OPTIONS_OPTION);
                
                optElement->SetAttribute(STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_INT32);
                optElement->SetAttribute(STARTUP_OPTIONS_NAME, opt->name().c_str());
                optElement->SetAttribute(STARTUP_OPTIONS_VALUE, static_cast<int32_t>(*opt));
            }
            
            if (StartupOptionInt64* opt = dynamic_cast<StartupOptionInt64*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_INT64));
                
                XMLElement *optElement = element->InsertNewChildElement(STARTUP_OPTIONS_OPTION);
                
                optElement->SetAttribute(STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_INT64);
                optElement->SetAttribute(STARTUP_OPTIONS_NAME, opt->name().c_str());
                optElement->SetAttribute(STARTUP_OPTIONS_VALUE, static_cast<int64_t>(*opt));
            }
            
            if (StartupOptionFloat* opt = dynamic_cast<StartupOptionFloat*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_FLOAT));
                
                XMLElement *optElement = element->InsertNewChildElement(STARTUP_OPTIONS_OPTION);
                
                optElement->SetAttribute(STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_FLOAT);
                optElement->SetAttribute(STARTUP_OPTIONS_NAME, opt->name().c_str());
                optElement->SetAttribute(STARTUP_OPTIONS_VALUE, static_cast<float>(*opt));
            }
            
            if (StartupOptionString* opt = dynamic_cast<StartupOptionString*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_STRING));
                
                XMLElement *optElement = element->InsertNewChildElement(STARTUP_OPTIONS_OPTION);
                
                optElement->SetAttribute(STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_STRING);
                optElement->SetAttribute(STARTUP_OPTIONS_NAME, opt->name().c_str());
                optElement->SetAttribute(STARTUP_OPTIONS_VALUE, static_cast<std::string>(*opt).c_str());
            }
        }
        
        // Format and write the XML
        //
        doc.SaveFile(iStartupOptionsFile.c_str());
#endif
        
#ifdef BBC_USE_BOOST
        // Create an empty property tree object.
        ptree optionsTree;
        
        optionsTree.add(STARTUP_OPTIONS_STARTUPOPTIONS, "");

        for (auto option : options_)
        {
            if (StartupOptionBool* opt = dynamic_cast<StartupOptionBool*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_BOOL));

                ptree &subtree = optionsTree.add(STARTUP_OPTIONS_STARTUPOPTIONS "." STARTUP_OPTIONS_OPTION, "");
                
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_BOOL);
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_NAME, opt->name());
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, static_cast<bool>(*opt));
            }
            
            if (StartupOptionInt32* opt = dynamic_cast<StartupOptionInt32*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_INT32));
                
                ptree &subtree = optionsTree.add(STARTUP_OPTIONS_STARTUPOPTIONS "." STARTUP_OPTIONS_OPTION, "");
                
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_INT32);
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_NAME, opt->name());
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, static_cast<int32_t>(*opt));
            }

            if (StartupOptionInt64* opt = dynamic_cast<StartupOptionInt64*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_INT64));
                
                ptree &subtree = optionsTree.add(STARTUP_OPTIONS_STARTUPOPTIONS "." STARTUP_OPTIONS_OPTION, "");
                
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_INT64);
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_NAME, opt->name());
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, static_cast<int64_t>(*opt));
            }

            if (StartupOptionFloat* opt = dynamic_cast<StartupOptionFloat*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_FLOAT));
                
                ptree &subtree = optionsTree.add(STARTUP_OPTIONS_STARTUPOPTIONS "." STARTUP_OPTIONS_OPTION, "");
                
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_FLOAT);
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_NAME, opt->name());
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, static_cast<float>(*opt));
            }

            if (StartupOptionString* opt = dynamic_cast<StartupOptionString*>(option))
            {
                BBC_ASSERT(opt->typeName() == std::string(STARTUP_OPTIONS_TYPENAME_STRING));
                
                ptree &subtree = optionsTree.add(STARTUP_OPTIONS_STARTUPOPTIONS "." STARTUP_OPTIONS_OPTION, "");
                
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_TYPE, STARTUP_OPTIONS_TYPENAME_STRING);
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_NAME, opt->name());
                subtree.add(XMLATTR STARTUP_OPTIONS_DOT STARTUP_OPTIONS_VALUE, static_cast<std::string>(*opt));
            }
        }

        try
        {
            // Format and write the XML
            //
            xml_writer_settings<std::string> formatting(' ', 4);
            write_xml(iStartupOptionsFile, optionsTree, std::locale(), formatting);
        }
        catch(...)
        {
            std::cerr << "Failed to create default startup options file!\n";
            return false;
        }
#else
        //static_assert(false, "StartupOptions::initialize - Requires BBC_USE_BOOST!");
#endif
        return true;
    }
    
private:
    
    bool add(StartupOption* iOption)
    {
        BBC_ASSERT_R(iOption);
        for (auto iter : options_)
        {
            BBC_ASSERT(iOption->name() != iter->name());
        }
        
        options_.push_back(iOption);
        
        return true;
    }

    /// Guard against multiple initializations
    ///
    bool initalized_{false};
    
    /// List of added StartupOptions
    /// StartupOptions are added
    std::vector<StartupOption*> options_;
};

