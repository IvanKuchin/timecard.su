#ifndef __CHTML__H__
#define __CHTML__H__

#include <memory>
#include <string>
#include <functional>
#include <tuple>
#include <regex>
#include <curl/curl.h>

#include "clog.h"
#include "utilities.h"
#include "localy.h"

using namespace std;

// extern int convert_utf8_to_windows1251(const char* utf8, char* windows1251, size_t n);

class CHTML
{
private: 
	CURL							*curl;
	CURLcode						curlRes;
	string							url = "";
	string							metaTitle = "";
	string							metaDescription = "";
	string							metaPreviewImageURL = "";
	string							htmlPage = "";
	string							headTag = "";
	string							folderID = "", filePrefix = "", fileExtention = "";
	string							embedVideoURL = "";
	string							type = "feed"; // --- depends on type different /images/xxxx folders stores images
	bool							isParsingRequired = true;

	void							ResetMetaValues();
	bool							ParseHTMLPage();
	bool							ExtractHEAD();
	bool							ExtractTITLE();
	bool							ExtractDESCRIPTION();
	bool							ExtractPreviewImage();
	string							ExtractContentAttribute(string prefix, string postfix);
	tuple<string, string, string>	PickFileName();
	bool							DownloadFile(string urlPreview, FILE *f);
	bool							ExtractEmbedVideoURL();

	int 							GetNumberOfFoldersByType();
	string							GetDirectoryByType();

public:
									CHTML();
									CHTML(string _type_);

	void							AppendToHTMLPage(string param);
	bool							PerformRequest(string param);

	void							SetType(const string &tmp)				{ type = tmp; }
	void							SetType(string &&tmp) 		noexcept	{ type = move(tmp); }
	void							SetParsingDisable()			noexcept	{ isParsingRequired = false; };
	void							SetParsingEnable()			noexcept	{ isParsingRequired = true; };

	string							GetTitle()						const	{ return metaTitle; }
	string							GetDescription()				const	{ return metaDescription; }
	string							GetPreviewImageURL()			const	{ return metaPreviewImageURL; }
	string							GetPreviewImageFolder()			const	{ return folderID; };
	string							GetPreviewImagePrefix()			const	{ return filePrefix; };
	string							GetPreviewImageExtention()		const	{ return fileExtention; };
	string							GetContent() 					const	{ return htmlPage; };


	bool							isEmbedVideoHostedOnYoutube();
	string							GetEmbedVideoURL() { return embedVideoURL; };

									~CHTML();
};

#endif
