/*
 * Oreka -- A media capture and retrieval platform
 * 
 * Copyright (C) 2005, orecx LLC
 *
 * http://www.orecx.com
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License.
 * Please refer to http://www.gnu.org/copyleft/gpl.html
 *
 */

#define _WINSOCKAPI_		// prevents the inclusion of winsock.h

#include "Utils.h"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include "serializers/DomSerializer.h"
#include "ConfigManager.h"
#include "LogManager.h"

#define CONFIG_FILE_NAME "config.xml"


void ConfigManager::Initialize()
{
	bool failed = false;
	m_configTopNode = NULL;

    try
    {
        XMLPlatformUtils::Initialize();
		XercesDOMParser *parser = new XercesDOMParser;
		parser->parse(CONFIG_FILE_NAME);
		DOMNode	*doc = NULL;
		doc = parser->getDocument();

		if (doc)
		{
			DOMNode *firstChild = doc->getFirstChild();
			if (firstChild)
			{
				m_configTopNode = firstChild;
				m_config.DeSerializeDom(firstChild);

				/*
				// Write out config to a file
				DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(XStr("Core").unicodeForm());
				XERCES_CPP_NAMESPACE::DOMDocument* myDoc;
				   myDoc = impl->createDocument(
							   0,                    // root element namespace URI.
							   XStr("root").unicodeForm(),         // root element name
							   0);                   // document type object (DTD).
				m_config.SerializeDom(myDoc);
				CStdString toto = DomSerializer::DomNodeToString(myDoc);
				FILE* file = fopen("zzz.xml", "w");
				fwrite((PCSTR)toto,1,toto.GetLength(),file);
				fclose(file);	
				*/
			}
			else
			{
				LOG4CXX_ERROR(LOG.configLog, CStdString("Could not parse config file:") + CONFIG_FILE_NAME);
				failed = true;
			}
		}
		else
		{
			LOG4CXX_WARN(LOG.configLog, CStdString("Could not find config file:") + CONFIG_FILE_NAME);
		}
	}
	catch (const CStdString& e)
	{
		LOG4CXX_ERROR(LOG.configLog, e);
		failed = true;
	}
    catch(const XMLException& e)
    {
		LOG4CXX_ERROR(LOG.configLog, e.getMessage());
		failed = true;
    }
	if (failed)
	{
		exit(0);
	}
}


void ConfigManager::AddConfigureFunction(ConfigureFunction configureFunction)
{
	m_configureFunction = configureFunction;
	// Cal the external configure callback straight away
	m_configureFunction(m_configTopNode);
}

